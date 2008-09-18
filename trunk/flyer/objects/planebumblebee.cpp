// Copyright (C) 2008 Maciej Gajewski <maciej.gajewski0@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include "planebumblebee.h"

#include "world.h"
#include "b2dqt.h"
#include "damagemanager.h"
#include "body.h"
#include "joint.h"
#include "mounting.h"
#include "engine.h"
#include "wing.h"
#include "controlsurface.h"
#include "wheelbrake.h"
#include "autopilot.h"
#include "gun.h"
#include "ironbomb.h"

namespace Flyer
{
static const double WHEEL_RADIUS = 0.5; // [m]
static const QPointF WHEEL_POS = QPointF( 0.5, -1.5 );
static const QPointF ELEVATOR_POS = QPointF( -4.0, 0.0 );
static const QPointF ENGINE_POS = QPointF( 1.5, 0.0 );

static const double ELEVATOR_LIFT = 10.0;
static const double ELEVATOR_LENGTH = 1.0;
static const double MAX_THRUST = 600; // kg force
static const double WHEEL_BRAKE_TORQUE = 1200; // not too big, to have nice effects
static const double ELEVATOR_STEP = 0.2;	/// elevator movement - in radians

static const QPointF ENGINE_JOINT_POS( 1.4, 0.0 );
static const QPointF LEG_JOINT_POS( 0.0, -0.3 );

static const double WING_DRAG_H	= 0.8;		// horizontal component of wing's drag
static const double WING_DRAG_V	= 50;		// vertical component of wing's drag
static const double WING_LIFT = 120;
static const double WING_INCLINATION = 0.04;	// hull-wings angle
static const double FLAPS_EXTRA_DRAG = 5;		// extra drag due to max flaps
static const double FLAPS_EXTRA_LIFT = 80;		// extra drag due to max flaps
static const double WING_WIDTH = 1.5;


static const double HULL_MASS = 1000.0; // 1t

// autopilot params (PID controller)
static const double AP_I = 1.5;	
static const double AP_P = 2.5;	
static const double AP_D = 0.3;	



// ============================================================================
// Constructor
PlaneBumblebee::PlaneBumblebee( World* pWorld, const QPointF& pos, double angle )
		: Plane( pWorld, pos, angle )
{
	createBodies( pos, angle );
	createSystems();
	createDamageManagers();
	createShapes();
	
	_jointBomb = NULL;
}

// ============================================================================
// Destructor
PlaneBumblebee::~PlaneBumblebee()
{
}

// TEMP - bopmb relaser
void PlaneBumblebee::releaseBomb()
{
	if ( _jointBomb )
	{
		_jointBomb->breakJoint();
		delete _jointBomb;
		_jointBomb = NULL;
	}
}


// ============================================================================
// Creates system
void PlaneBumblebee::createSystems()
{
	// engine
	_sysEngine = new Engine( this, "engine" );
	_sysEngine->setMaxThrust( MAX_THRUST );
	_sysEngine->setDamageCapacity( 200E3 );
	_sysEngine->setBody( _bodyEngine );
	addSystem( _sysEngine, SystemRendered2 | SystemSimulated );
	setEngine( _sysEngine );
	
	// wheel mounting
	_sysWheelMounting = new Mounting( this, "Wheel mounting" );
	_sysWheelMounting->setTolerance( 30E3 );
	_sysWheelMounting->setDamageCapacity( 200E3 );
	_sysWheelMounting->setJoint( _jointWheel );
	addSystem( _sysWheelMounting, SystemSimulated );
	
	// leg mounting
	Mounting* pLegMounting = new Mounting( this, "Leg mounting" );
	pLegMounting->setTolerance( 30E3 );
	pLegMounting->setDamageCapacity( 400E3 );
	pLegMounting->setJoint( _jointLeg );
	addSystem( pLegMounting, SystemSimulated );
	
	// wing
	_sysWing = new Wing( this, "Wing" );
	_sysWing->setWidth( WING_WIDTH );
	_sysWing->setDragCoeffH( WING_DRAG_H );
	_sysWing->setDragCoeffV( WING_DRAG_V );
	_sysWing->setLiftCoeff( WING_LIFT );
	_sysWing->setFlapsDrag( FLAPS_EXTRA_DRAG );
	_sysWing->setFlapsLift( FLAPS_EXTRA_LIFT );
	_sysWing->setBody( _bodyHull );
	_sysWing->setInclination( WING_INCLINATION );
	_sysWing->setDamageCapacity( 300E3 );
	addSystem( _sysWing, SystemRendered2 | SystemSimulated );
	setWing( _sysWing );
	
	// elevator
	_sysElevator = new ControlSurface( this, "elevator" );
	_sysElevator->setWidth( ELEVATOR_LENGTH );
	_sysElevator->setPosition( ELEVATOR_POS );
	_sysElevator->setStep( ELEVATOR_STEP );
	_sysElevator->setBody( _bodyHull );
	_sysElevator->setLiftCoeff( ELEVATOR_LIFT );
	_sysElevator->setDamageCapacity( 100E3 );
	addSystem( _sysElevator, SystemRendered2 | SystemSimulated );
	setElevator( _sysElevator );
	
	// brake
	_sysBrake = new WheelBrake( this, "wheel brake");
	_sysBrake->setBrakingTorque( WHEEL_BRAKE_TORQUE );
	_sysBrake->setJoint( _jointWheel );
	_sysBrake->setDamageCapacity( 100E3 );
	addSystem( _sysBrake, SystemSimulated );
	setWheelBrake( _sysBrake );
	
	// autopilot
	_sysAutopilot = new Autopilot( this, "autopilot" );
	_sysAutopilot->setElevator( _sysElevator );
	_sysAutopilot->setEngine( _sysEngine );
	_sysAutopilot->setWing( _sysWing );
	_sysAutopilot->setBody( _bodyHull );
	_sysAutopilot->setDamageCapacity( 50E3 );
	addSystem( _sysAutopilot, SystemSimulated | SystemRendered1 );
	setAutopilot( _sysAutopilot );
	
	Autopilot::Settings apsettings;
	apsettings.minSpeed = 15.0;
	apsettings.flapsOpenSpeed = 25.0;
	apsettings.flapsCloseSpeed = 15.0;
	apsettings.P = AP_P;
	apsettings.I = AP_I;
	apsettings.D = AP_D;
	apsettings.maxClimbRate = 0.2;
	apsettings.maxDescentRate = 0.5;
	apsettings.VVP = 1;
	apsettings.VVI = 0.5;
	apsettings.safeAngle = 0.3;
	
	_sysAutopilot->setSettings( apsettings );
	_sysAutopilot->setMode( Autopilot::FollowTrack );
	
	// add sample track - -from central airport to the far one
	
	// pre-climb
	Autopilot::TrackSegment s;
	s.start.Set( -400, 400 );
	s.end.Set( 400, 400 );
	s.airspeed = 60;
	_sysAutopilot->track().append( s );
	
	// climb
	s.start.Set( 400, 400 );
	s.end.Set( 3000, 700 );
	s.airspeed = 40;
	_sysAutopilot->track().append( s );
	
	// climb 2
	s.start.Set( 3000, 700 );
	s.end.Set( 5000, 1000 );
	s.airspeed = 40;
	_sysAutopilot->track().append( s );
	
	// route
	s.start.Set( 5000, 1000 );
	s.end.Set( 9300, 1000 );
	s.airspeed = 40;
	_sysAutopilot->track().append( s );
	
	// approach
	s.start.Set( 9300, 1000 );
	s.end.Set( 10050, 810 );
	s.airspeed = 18;
	_sysAutopilot->track().append( s );
	
	// touch down
	s.start.Set( 10050, 810 );
	s.end.Set( 10100, 802 );
	s.airspeed = 12;
	s.orders.insert( Autopilot::Land );
	_sysAutopilot->track().append( s );
	
	// stop
	s.start.Set( 10100, 802 );
	s.end.Set( 10300, 800 );
	s.airspeed = 0;
	_sysAutopilot->track().append( s );
	
	// gun
	_sysGun = Gun::berezin( this, "Berezin" );
	_sysGun->setBody( _bodyHull );
	_sysGun->setMuzzle( QPointF( 3.0, 0.0 ) );
	_sysGun->setNormal( QPointF( 1.0, 0.0 ) );
	addSystem( _sysGun, SystemSimulated );
	setGun( _sysGun );
}
// ============================================================================
// Creates bodies
void PlaneBumblebee::createBodies( const QPointF& pos, double angle )
{
	b2World* pWorld = world()->b2world();
	
	QTransform transform;
	transform.translate( pos.x(), pos.y() );
	transform.rotateRadians( angle );
	
	QPointF wheelCenter = transform.map( WHEEL_POS );
	QPointF legJointPos = transform.map( LEG_JOINT_POS );
	QPointF engineJointPos = transform.map( ENGINE_JOINT_POS );
	QPointF enginePos = transform.map( ENGINE_POS );
	
	// main body
	b2BodyDef mainDef;
	mainDef.massData.mass = HULL_MASS;
	mainDef.massData.center.SetZero();
	mainDef.massData.I = HULL_MASS * 2; // 2* mass
	mainDef.linearDamping = 0;
	mainDef.angularDamping = 0.4;
	mainDef.position.Set( pos.x(), pos.y() );
	mainDef.angle = angle;
	
	_bodyHull = new Body("Fuselage");
	_bodyHull->create( mainDef, pWorld );
	addBody( _bodyHull, BodyRendered2 );
	setMainBody( _bodyHull );
	
	// wheel
	b2BodyDef wheelDef;
	wheelDef.massData.mass = 10;	// 10kg
	wheelDef.massData.center.SetZero();
	wheelDef.massData.I = 1;
	wheelDef.linearDamping = 0;
	wheelDef.angularDamping = 0.01;
	
	wheelDef.position.Set( wheelCenter.x(), wheelCenter.y() ); // 1 meter below
	wheelDef.angle = angle;
	
	_bodyWheel = new Body("Wheel");
	_bodyWheel->create( wheelDef, pWorld );
	addBody( _bodyWheel, BodyRendered1 );
	

	// leg
	b2BodyDef legDef;
	legDef.massData.I = 40;
	legDef.position.Set( pos.x(), pos.y() );
	legDef.angle = angle;
	legDef.linearDamping = 0;
	legDef.angularDamping = 0;
	
	_bodyLeg = new Body("Leg");
	_bodyLeg->create( legDef, pWorld );
	addBody( _bodyLeg, BodyRendered3 );
	
	// engine
	b2BodyDef engineDef;
	engineDef.position.Set( enginePos.x(), enginePos.y() );
	engineDef.angle = angle;
	engineDef.linearDamping = 0;
	engineDef.angularDamping = 0;
	
	_bodyEngine = new Body("Engine");
	_bodyEngine->create( engineDef, pWorld );
	addBody( _bodyEngine, BodyRendered1 );
	

	// joints

	// joint between wheel and leg
	b2RevoluteJointDef wheelJoint;
	wheelJoint.Initialize( _bodyLeg->b2body(), _bodyWheel->b2body(), b2Vec2( wheelCenter.x(), wheelCenter.y() ) );
	wheelJoint.enableMotor = true;
	wheelJoint.maxMotorTorque = WHEEL_BRAKE_TORQUE;
	wheelJoint.enableLimit = false;
	wheelJoint.collideConnected = false;
	
	// joint between leg and body
	b2RevoluteJointDef legJoint;
	legJoint.Initialize( _bodyHull->b2body(), _bodyLeg->b2body(), b2Vec2( legJointPos.x(), legJointPos.y() ) );
	legJoint.enableMotor = true;
	legJoint.enableLimit = true;
	legJoint.lowerAngle = 0.0;
	legJoint.upperAngle = 0.0;
	legJoint.collideConnected = false;
	
	// joint between body and engine
	b2RevoluteJointDef engineJoint;
	engineJoint.Initialize( _bodyHull->b2body(), _bodyEngine->b2body(), b2Vec2( engineJointPos.x(), engineJointPos.y() ) );
	engineJoint.enableMotor = true;
	engineJoint.enableLimit = true;
	engineJoint.lowerAngle = 0.0;
	engineJoint.upperAngle = 0.0;
	engineJoint.collideConnected = false;
	
	
	_jointWheel = new Joint();
	_jointWheel->create( & wheelJoint, pWorld );
	_jointWheel->setBodies( _bodyLeg, _bodyWheel );
	addJoint( _jointWheel );
	
	_jointLeg = new Joint();
	_jointLeg->create( & legJoint, pWorld );
	_jointLeg->setBodies( _bodyHull, _bodyLeg );
	addJoint( _jointLeg );
	
	_jointEngine = new Joint();
	_jointEngine->create( & engineJoint, pWorld );
	_jointEngine->setBodies( _bodyHull, _bodyEngine );
	addJoint( _jointEngine );
	
}


// ============================================================================
// Creates shapes and joints, adds them to objects
void PlaneBumblebee::createShapes()
{
	// main body shape
	QPolygonF shapeBody;
	shapeBody.append( QPointF( 1.5, 0.5) );
	shapeBody.append( QPointF(-0.1, 0.9) );
	shapeBody.append( QPointF(-5.0, 0.0) );
	shapeBody.append( QPointF( 0.0,-0.7) );
	shapeBody.append( QPointF( 1.5,-0.5) );
		
	QPolygonF shapeSkid;
	shapeSkid.append( QPointF(-5.0,-0.3) );
	shapeSkid.append( QPointF(-4.7,-0.3) );
	shapeSkid.append( QPointF(-4.0, 0.0) );
	shapeSkid.append( QPointF(-4.7, 1.0) );
	shapeSkid.append( QPointF(-5.0, 1.0) );

	b2PolygonDef mainShape = shapeToDef( shapeBody, false );
	mainShape.friction = 0.5;
	mainShape.restitution = 0.3;
	mainShape.userData = _dmHull;
	
	b2PolygonDef skidShape = shapeToDef( shapeSkid, false ); 
	skidShape.friction = 0.01;
	skidShape.restitution = 0.1;
	skidShape.userData = _dmTail;
	
	
	_bodyHull->addShape( & mainShape );
	_bodyHull->addShape( & skidShape );
	
	// wheel 
	b2CircleDef wheelShape;
	wheelShape.radius = WHEEL_RADIUS;
	wheelShape.localPosition.SetZero();
	wheelShape.isSensor = false;
	wheelShape.restitution = 0.0;
	wheelShape.density = 10;
	wheelShape.friction = 0.7;
	wheelShape.userData = _dmWheel;
	
	_bodyWheel->addShape( & wheelShape );
	
	// leg
	QPolygonF shapeLeg;
	shapeLeg.append( QPointF(-0.2, -0.2) );
	shapeLeg.append( QPointF( 0.3, -1.6) );
	shapeLeg.append( QPointF( 0.7, -1.6) );
	shapeLeg.append( QPointF( 0.2, -0.2) );
	b2PolygonDef legShape = shapeToDef( shapeLeg, false );
	legShape.friction = 0.9;
	legShape.restitution = 0.1;
	legShape.density = 20;
	legShape.userData = _dmLeg;
	
	_bodyLeg->addShape( & legShape );
	
	// engine
	QPolygonF shapeEngine;
	shapeEngine.append( QPointF( -0.2, 0.6) );
	shapeEngine.append( QPointF( -0.2, -0.6) );
	shapeEngine.append( QPointF( 0.3, -0.5) );
	shapeEngine.append( QPointF( 0.3, 0.5) );
	b2PolygonDef engineShape = shapeToDef( shapeEngine, false );
	engineShape.friction	= mainShape.friction;
	engineShape.restitution	= mainShape.restitution;
	engineShape.density		= 10;
	engineShape.userData	= _dmEngine;
	_sysEngine->setPropellerBladeLength( 0.8 );
	_sysEngine->setPropellerCenter( QPointF( 0.5, 0.0 ) );
	_sysEngine->setPropllerAxis( QLineF( 0.3, 0.0, 0.7, 0.0 ) );

	_bodyEngine->addShape( & engineShape );

}

// ============================================================================
// Creates damage managers
void PlaneBumblebee::createDamageManagers()
{
	// engine
	_dmEngine = new DamageManager( 50E3 );
	// set 90% to engine, 10% unused
	_dmEngine->addSystem( _sysEngine, 9 );
	_dmEngine->addSystem( NULL, 0 );
	addDamageManager( _dmEngine );
	
	// wheel
	_dmWheel = new DamageManager( 5E4 );
	
	// use 25% for brake/ 25% for mounting
	_dmWheel->addSystem( _sysBrake, 1 );
	_dmWheel->addSystem( _sysWheelMounting, 1 );
	_dmWheel->addSystem( NULL, 2 );
	addDamageManager( _dmWheel );
	
	// leg
	_dmLeg = new DamageManager( 4E4 );
	
	// same as wheel
	_dmLeg->addSystem( _sysBrake, 1 );
	_dmLeg->addSystem( _sysWheelMounting, 1 );
	_dmLeg->addSystem( NULL, 2 );
	addDamageManager( _dmLeg );
	
	// hull
	_dmHull = new DamageManager( 4E4 );
	_dmHull->addSystem( _sysGun, 1 );
	_dmHull->addSystem( _sysWing, 5 );
	_dmHull->addSystem( _sysElevator, 1 );
	_dmHull->addSystem( _sysAutopilot, 1 );
	_dmHull->addSystem( _sysEngine, 2 );
	_dmHull->addSystem( NULL, 10 ); // 50% unused
	addDamageManager( _dmHull );
	
	// tail - wings and engin also could be damaged
	_dmTail = new DamageManager( 3E4 );
	_dmTail->addSystem( _sysEngine, 1 );
	_dmTail->addSystem( _sysWing, 3 );
	_dmTail->addSystem( _sysElevator, 2 );
	_dmTail->addSystem( NULL, 4 );
	addDamageManager( _dmTail );
	
}

// ============================================================================
// Renders bumblebee
void PlaneBumblebee::render( QPainter& painter, const QRectF& rect, const RenderingOptions& options )
{
	painter.setPen( Qt::black );
	painter.setBrush( Qt::gray );

	Machine::render( painter, rect, options );

}

}
