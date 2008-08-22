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

#include <math.h>

#include <QPainter>

#include "plane.h"
#include "world.h"
#include "engine.h"
#include "damagemanager.h"
#include "b2dqt.h"

namespace Flyer
{


static const double WHEEL_RADIUS = 0.5; // [m]
static const QPointF WHEEL_POS = QPointF( 0.5, -1.5 );
static const QPointF ELEVATOR_POS = QPointF( -4.0, 0.0 );
static const QPointF ENGINE_POS = QPointF( 1.5, 0.0 );

static const double ELEVATOR_LIFT = 10.0;
static const double ELEVATOR_LENGTH = 1.0;
static const double MAX_THRUST = 500; // kg force
static const double WHEEL_BRAKE_TORQUE = 1000; // not too big, to have nice effects
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

static const double MIN_FLIP_SPEED = 30;	///< min speed for flip
static const double MAX_TURN_SPEED = 0.1;	///< max speed for turn

// autopilot params (PID controller)
static const double AP_I = 1.5;	
static const double AP_P = 2.5;	
static const double AP_D = 0.3;	



// ============================================================================
// Constructor
Plane::Plane( World* pWorld, const QPointF& pos, double angle ) : Machine( pWorld )
	, _sysEngine( this )
	, _sysWheelMounting( this )
	, _sysWing( this )
	, _sysElevator( this )
	, _sysBrake( this )
	, _sysAutopilot( this )
	, _sysGun( this )
{
	// settings
	_orientation = 1.0;
	
	// create systems
	_sysEngine.setName("engine");
	_sysEngine.setMaxThrust( MAX_THRUST );
	_sysEngine.setDamageCapacity( 2000 );
	addSystem( & _sysEngine, SystemRendered2 | SystemSimulated );
	
	_sysWheelMounting.setName("wheel mounting");
	_sysWheelMounting.setTolerance( 30E3 );
	_sysWheelMounting.setDamageCapacity( 200E3 );
	_sysWheelMounting.setJoint( & _jointWheel );
	addSystem( & _sysWheelMounting, SystemSimulated );
	
	_sysWing.setWidth( WING_WIDTH );
	_sysWing.setDragCoeffH( WING_DRAG_H );
	_sysWing.setDragCoeffV( WING_DRAG_V );
	_sysWing.setLiftCoeff( WING_LIFT );
	_sysWing.setFlapsDrag( FLAPS_EXTRA_DRAG );
	_sysWing.setFlapsLift( FLAPS_EXTRA_LIFT );
	_sysWing.setBody( & _bodyHull );
	_sysWing.setInclination( WING_INCLINATION );
	addSystem( & _sysWing, SystemRendered2 | SystemSimulated );
	
	_sysElevator.setWidth( ELEVATOR_LENGTH );
	_sysElevator.setPosition( ELEVATOR_POS );
	_sysElevator.setStep( ELEVATOR_STEP );
	_sysElevator.setBody( & _bodyHull );
	_sysElevator.setLiftCoeff( ELEVATOR_LIFT );
	addSystem( & _sysElevator, SystemRendered2 | SystemSimulated );
	
	_sysBrake.setBrakingTorque( WHEEL_BRAKE_TORQUE );
	_sysBrake.setJoint( & _jointWheel );
	addSystem( & _sysBrake, SystemSimulated );
	
	_sysAutopilot.setPID( AP_P, AP_I, AP_D );
	_sysAutopilot.setElevator( & _sysElevator );
	_sysAutopilot.setBody( & _bodyHull );
	addSystem( & _sysAutopilot, SystemSimulated );
	
	_sysGun.setBody( & _bodyHull );
	_sysGun.setMuzzle( QPointF( 2.0, 0.0 ) );
	_sysGun.setNormal( QPointF( 1.0, 0.0 ) );
	_sysGun.setBulletMass( 7.91E-3 ); 
	_sysGun.setBulletVelocity( 735 ); // NOTE too fast to see
	//_sysGun.setBulletVelocity( 60 );
	_sysGun.setBulletSize( 7.85E-3 ); //NOTE: too smnal, cant see // all values from kalashnikov ;)
	//_sysGun.setBulletSize( 0.1 ); // ridicoulously big, but i need to see them :)
	_sysGun.setFiringInterval( 0.4 ); // 2.5 shots/sec
	_sysGun.setBulletLifespan( 2.0 ); // TODO experimantal
	addSystem( & _sysGun, SystemSimulated );

	
	// create damage managers
	_pEngineDamageManager = new DamageManager( 100 );
	// set 90% to engine, 10% unused
	_pEngineDamageManager->addSystem( & _sysEngine, 9 );
	_pEngineDamageManager->addSystem( NULL, 0 );
	
	// init joints list
	addJoint( & _jointWheel );
	addJoint( & _jointEngine );
	addJoint( & _jointLeg );
	
	// create objects
	
	createBodies( pos, angle );
	createShapes( pos, angle );
	
	
}

// ============================================================================
// Creates bodies
void Plane::createBodies( const QPointF& pos, double angle )
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
	
	_bodyHull.create( mainDef, pWorld );
	addBody( & _bodyHull, BodyRendered2 );
	
	// wheel
	b2BodyDef wheelDef;
	wheelDef.massData.mass = 10;	// 10kg
	wheelDef.massData.center.SetZero();
	wheelDef.massData.I = 1;
	wheelDef.linearDamping = 0;
	wheelDef.angularDamping = 0.01;
	
	wheelDef.position.Set( wheelCenter.x(), wheelCenter.y() ); // 1 meter below
	wheelDef.angle = angle;
	
	_bodyWheel.create( wheelDef, pWorld );
	addBody( & _bodyWheel, 0 );
	

	// leg
	b2BodyDef legDef;
	legDef.massData.I = 40;
	legDef.position.Set( pos.x(), pos.y() );
	legDef.angle = angle;
	legDef.linearDamping = 0;
	legDef.angularDamping = 0;
	
	//_pMainLeg = pWorld->CreateBody( & legDef );
	_bodyLeg.create( legDef, pWorld );
	addBody( & _bodyLeg, BodyRendered3 );
	
	// engine
	b2BodyDef engineDef;
	engineDef.position.Set( enginePos.x(), enginePos.y() );
	engineDef.angle = angle;
	engineDef.linearDamping = 0;
	engineDef.angularDamping = 0;
	
	_bodyEngine.create( engineDef, pWorld );
	_sysEngine.setBody( & _bodyEngine );
	addBody( & _bodyEngine, BodyRendered1 );
	

	// joints

	// joint between wheel and leg
	b2RevoluteJointDef wheelJoint;
	wheelJoint.Initialize( _bodyLeg.b2body(), _bodyWheel.b2body(), b2Vec2( wheelCenter.x(), wheelCenter.y() ) );
	wheelJoint.enableMotor = true;
	wheelJoint.maxMotorTorque = WHEEL_BRAKE_TORQUE;
	wheelJoint.enableLimit = false;
	wheelJoint.collideConnected = false;
	
	// joint between leg and body
	b2RevoluteJointDef legJoint;
	legJoint.Initialize( _bodyHull.b2body(), _bodyLeg.b2body(), b2Vec2( legJointPos.x(), legJointPos.y() ) );
	legJoint.enableMotor = true;
	legJoint.enableLimit = true;
	legJoint.lowerAngle = 0.0;
	legJoint.upperAngle = 0.0;
	legJoint.collideConnected = false;
	
	// joint between body and engine
	b2RevoluteJointDef engineJoint;
	engineJoint.Initialize( _bodyHull.b2body(), _bodyEngine.b2body(), b2Vec2( engineJointPos.x(), engineJointPos.y() ) );
	engineJoint.enableMotor = true;
	engineJoint.enableLimit = true;
	engineJoint.lowerAngle = 0.0;
	engineJoint.upperAngle = 0.0;
	engineJoint.collideConnected = false;
	
	
	//_pMainWheelJoint = (b2RevoluteJoint*)pWorld->CreateJoint( & wheelJoint );
	_jointWheel.create( & wheelJoint, pWorld );
	_jointWheel.setBodies( & _bodyLeg, & _bodyWheel );
	
	//_pLegJoin = (b2RevoluteJoint*)pWorld->CreateJoint( & legJoint );
	_jointLeg.create( & legJoint, pWorld );
	_jointLeg.setBodies( & _bodyHull, & _bodyLeg );
	
	
	//pWorld->CreateJoint( & engineJoint );
	_jointEngine.create( & engineJoint, pWorld );
	_jointEngine.setBodies( & _bodyHull, &_bodyEngine );
	
}

// ============================================================================
// Creates shapes, adds them to objects
void Plane::createShapes( const QPointF& pos, double angle )
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
	
	b2PolygonDef skidShape = shapeToDef( shapeSkid, false ); 
	skidShape.friction = 0.01;
	skidShape.restitution = 0.1;
	
	
	_bodyHull.addShape( & mainShape );
	_bodyHull.addShape( & skidShape );
	
	// wheel 
	b2CircleDef wheelShape;
	wheelShape.radius = WHEEL_RADIUS;
	wheelShape.localPosition.SetZero();
	wheelShape.isSensor = false;
	wheelShape.restitution = 0.0;
	wheelShape.density = 10;
	wheelShape.friction = 0.7;
	
	_bodyWheel.addShape( & wheelShape );
	
	// leg
	QPolygonF shapeLeg;
	shapeLeg.append( QPointF(-0.2, -0.2) );
	shapeLeg.append( QPointF( 0.3, -1.6) );
	shapeLeg.append( QPointF( 0.7, -1.6) );
	shapeLeg.append( QPointF( 0.2, -0.2) );
	b2PolygonDef legShape = shapeToDef( shapeLeg, false );
	legShape.friction = 0;
	legShape.restitution = 0.1;
	legShape.density = 20;
	
	_bodyLeg.addShape( & legShape );
	
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
	engineShape.userData	= _pEngineDamageManager;
	_sysEngine.setPropellerBladeLength( 0.8 );
	_sysEngine.setPropellerCenter( QPointF( 0.5, 0.0 ) );
	_sysEngine.setPropllerAxis( QLineF( 0.3, 0.0, 0.7, 0.0 ) );

	_bodyEngine.addShape( & engineShape );

}

// ============================================================================
// Destructor
Plane::~Plane()
{
	// none
}


// ============================================================================
// Draws a plane.
void Plane::render( QPainter& painter, const QRectF& rect )
{
	painter.setPen( Qt::black );
	painter.setBrush( Qt::gray );

	Machine::render( painter, rect );

	// draw wheel
	QTransform wheelTransform = _bodyWheel.transform();
	
	painter.save();
		painter.setPen( Qt::black );
		painter.setBrush( Qt::gray );
		painter.setTransform( wheelTransform, true );
		painter.drawEllipse( QRectF(-WHEEL_RADIUS, -WHEEL_RADIUS, WHEEL_RADIUS*2, WHEEL_RADIUS*2 ) );
		painter.drawLine( QLineF( -WHEEL_RADIUS, 0.0, WHEEL_RADIUS, 0.0 ) );
	painter.restore();
	
}

// ============================================================================
// Sets throttle
void Plane::setThrottle( double t )
{
	_sysEngine.setThrottle( t );
}

// ============================================================================
// Returns throtlle
double Plane::throttle() const
{
	return _sysEngine.throttle();
}

// ============================================================================
// Sets elevator -1 - to 1
void Plane::setElevator( double e )
{
	if ( autopilot() )
	{
		return; // do nothing
	}
	
	_sysElevator.setValue( e );
}

// ============================================================================
// Set flaps = 0-1
void Plane::setFlaps( double f )
{
	_sysWing.setFlaps( f );
}


// ============================================================================
// Returns plane position
QPointF Plane::pos() const
{
	const b2Vec2& p = _bodyHull.b2body()->GetPosition();
	
	return QPointF( p.x, p.y );
}


// ============================================================================
// Air speed
double Plane::airspeed()
{
	double angle = _bodyHull.b2body()->GetAngle(); // hull angle
	b2Vec2 velocity = _bodyHull.b2body()->GetLinearVelocity();
	double v = sqrt( velocity.x*velocity.x + velocity.y*velocity.y );
	double velAngle = atan2( velocity.y, velocity.x );
	
	return v * cos( angle - velAngle );
}

// ============================================================================
// flips plane to the other side. destroys all shapes and creates them ab ovo
void Plane::flipPlane()
{
	// first - ckeck airspeed
	if ( airspeed() < MIN_FLIP_SPEED && airspeed() > MAX_TURN_SPEED )
	{
		qDebug("too slow to flip, tu fats to turn");
		return; // no flip below this speed
	}
	
	// flip or turn?
	bool turn = airspeed() <= MAX_TURN_SPEED; // turn otherwise
	
	// find axis of flip
	QPointF p1, p2;
	
	//if turn, change angle
	if ( turn )
	{
		// for turn - rotate around vertical axis
		b2Vec2 pos = _bodyHull.b2body()->GetPosition();
		p1 = QPointF( pos.x, pos.y );
		p2 = QPointF( pos.x, pos.y + 1.0 );
		
	}
	else
	{
		// for speed flip - rotate around velocity vector
		b2Vec2 pos = _bodyHull.b2body()->GetPosition();
		b2Vec2 vel = _bodyHull.b2body()->GetLinearVelocity();
	
		p1 = QPointF ( pos.x, pos.y );
		p2 = QPointF ( pos.x + vel.x, pos.y + vel.y );
	}
	
	flip( p1, p2 );
}

// ============================================================================
// Renders plane on map
void Plane::renderOnMap( QPainter& painter, const QRectF& rect )
{
	painter.setPen( Qt::red );
	painter.setBrush( Qt::white );
	
	QPointF p = pos();
	double size = 200; // 20 meters
	QRectF ellipse( p.x() - size/2, p.y() - size/2, size, size );
	painter.drawEllipse( ellipse );
}

// ============================================================================
// Sets autopilot on/off
void Plane::setAutopilot( bool on )
{
	_sysAutopilot.setOn( on );
}

// ============================================================================
// Elevator angle
double Plane::elevatorAngle() const
{
	return _sysElevator.angle();
}


}