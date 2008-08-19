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

namespace Flyer
{


static const double WHEEL_RADIUS = 0.5; // [m]
static const QPointF WHEEL_POS = QPointF( 0.5, -1.5 );
static const QPointF ELEVATOR_POS = QPointF( -4.0, 0.0 );
static const double ELEVATOR_LENGTH = 1.0;
static const double MAX_THRUST = 500; // kg force
static const double WHEEL_BRAKE_TORQUE = 1000; // not too big, to have nice effects
static const double ELEVATOR_STEP = 0.2;	/// elevator movement - in radians

static const QPointF ENGINE_JOINT_POS( 1.4, 0.0 );
static const QPointF LEG_JOINT_POS( 0.0, -0.3 );

static const double WING_DRAG_H	= 0.08;		// horizontal component of wing's drag
static const double WING_DRAG_V	= 5;		// vertical component of wing's drag
static const double WING_LIFT = 12;
static const double WING_INCLINATION = 0.04;	// hull-wings angle
static const double FLAPS_EXTRA_DRAG = 0.5;		// extra drag due to max flaps
static const double FLAPS_EXTRA_LIFT = 8;		// extra drag due to max flaps


static const double HULL_MASS = 1000.0; // 1t

static const double MIN_FLIP_SPEED = 30;	///< min speed for flip
static const double MAX_TURN_SPEED = 0.1;	///< max speed for turn

// autopilot params (PID controller)
static const double AP_I = 1.5;	
static const double AP_P = 2.5;	
static const double AP_D = 0.3;	
static const double AP_MIN_AIRSPEED = 10;	// m/s



// ============================================================================
// Constructor
Plane::Plane( World* pWorld, const QPointF& pos, double angle ) : WorldObject( pWorld )
{
	// settings
	_elevator = 0;
	_wheelBrake = false;
	_orientation = 1;
	_flaps = 0.0;
	_autopilot = false;
	
	// create systems
	_pEngine = new Engine( "engine", MAX_THRUST );
	addSystem( _pEngine, SystemRendered2 | SystemSimulated );
	
	// create damage managers
	_pEngineDamageManager = new DamageManager( 1000 ); /// threshold of 1000 newtons
	// set 90% to engine, 10% unused
	_pEngineDamageManager->addSystem( _pEngine, 9 );
	_pEngineDamageManager->addSystem( NULL, 0 );
	
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
	transform.scale( 1.0, _orientation );
	
	QPointF wheelCenter = transform.map( WHEEL_POS );
	QPointF legJointPos = transform.map( LEG_JOINT_POS );
	QPointF engineJointPos = transform.map( ENGINE_JOINT_POS );
	
	// main body
	b2BodyDef mainDef;
	mainDef.massData.mass = HULL_MASS;
	mainDef.massData.center.SetZero();
	mainDef.massData.I = HULL_MASS * 2; // 2* mass
	mainDef.linearDamping = 0;
	mainDef.angularDamping = 0.4;
	mainDef.position.Set( pos.x(), pos.y() );
	mainDef.angle = angle;
	
	_pMainBody = pWorld->CreateBody( & mainDef );
	
	// wheel
	b2BodyDef wheelDef;
	wheelDef.massData.mass = 10;	// 10kg
	wheelDef.massData.center.Set(  wheelCenter.x(), wheelCenter.y() );
	wheelDef.massData.I = 1;
	wheelDef.linearDamping = 0;
	wheelDef.angularDamping = 0.01;
	
	wheelDef.position.Set( wheelCenter.x(), wheelCenter.y() ); // 1 meter below
	wheelDef.angle = angle;
	
	_pMainWheel = pWorld->CreateBody( & wheelDef );

	// leg
	b2BodyDef legDef;
	legDef.massData.I = 40;
	legDef.position.Set( pos.x(), pos.y() );
	legDef.angle = angle;
	legDef.linearDamping = 0;
	legDef.angularDamping = 0;
	
	_pMainLeg = pWorld->CreateBody( & legDef );
	
	// engine
	b2BodyDef engineDef;
	engineDef.position.Set( pos.x(), pos.y() );
	engineDef.angle = angle;
	engineDef.linearDamping = 0;
	engineDef.angularDamping = 0;
	
	_pEngineCase =  pWorld->CreateBody( & engineDef );
	_pEngine->setBody( _pEngineCase );
	

	// joints

	// joint between wheel and leg
	b2RevoluteJointDef wheelJoint;
	wheelJoint.Initialize( _pMainLeg, _pMainWheel, b2Vec2( wheelCenter.x(), wheelCenter.y() ) );
	wheelJoint.enableMotor = true;
	wheelJoint.maxMotorTorque = WHEEL_BRAKE_TORQUE;
	wheelJoint.enableLimit = false;
	wheelJoint.collideConnected = false;
	
	// joint between leg and body
	b2RevoluteJointDef legJoint;
	legJoint.Initialize( _pMainBody, _pMainLeg, b2Vec2( legJointPos.x(), legJointPos.y() ) );
	legJoint.enableMotor = true;
	legJoint.enableLimit = true;
	legJoint.lowerAngle = 0.0;
	legJoint.upperAngle = 0.0;
	legJoint.collideConnected = false;
	
	// joint between body and engine
	b2RevoluteJointDef engineJoint;
	engineJoint.Initialize( _pMainBody, _pEngineCase, b2Vec2( engineJointPos.x(), engineJointPos.y() ) );
	engineJoint.enableMotor = true;
	engineJoint.enableLimit = true;
	engineJoint.lowerAngle = 0.0;
	engineJoint.upperAngle = 0.0;
	engineJoint.collideConnected = false;
	
	
	_pMainWheelJoint = (b2RevoluteJoint*)pWorld->CreateJoint( & wheelJoint );
	_pLegJoin = (b2RevoluteJoint*)pWorld->CreateJoint( & legJoint );
	pWorld->CreateJoint( & engineJoint );
	
}

// ============================================================================
// Creates shapes, adds them to objects
void Plane::createShapes( const QPointF& pos, double angle )
{
	// main body shape
	_shapeBody.clear();
	_shapeBody.append( QPointF( 1.5, 0.5 * _orientation ) );
	_shapeBody.append( QPointF(-0.1, 0.9 * _orientation ) );
	_shapeBody.append( QPointF(-5.0, 0.0 * _orientation ) );
	_shapeBody.append( QPointF( 0.0,-0.7 * _orientation ) );
	_shapeBody.append( QPointF( 1.5,-0.5 * _orientation ) );
		
	_shapeSkid.clear();
	_shapeSkid.append( QPointF(-5.0,-0.3 * _orientation) );
	_shapeSkid.append( QPointF(-4.7,-0.3 * _orientation) );
	_shapeSkid.append( QPointF(-4.0, 0.0 * _orientation) );
	_shapeSkid.append( QPointF(-4.7, 1.0 * _orientation) );
	_shapeSkid.append( QPointF(-5.0, 1.0 * _orientation) );

	b2PolygonDef mainShape = shapeToDef( _shapeBody, _orientation < 0 );
	mainShape.friction = 0.5;
	mainShape.restitution = 0.3;
	
	b2PolygonDef skidShape = shapeToDef( _shapeSkid, _orientation < 0 ); 
	skidShape.friction = 0.01;
	skidShape.restitution = 0.1;
	
	
	_pMainBodyShape = _pMainBody->CreateShape( & mainShape );
	_pSkidShape = _pMainBody->CreateShape( & skidShape );
	
	// wheel 
	b2CircleDef wheelShape;
	wheelShape.radius = WHEEL_RADIUS;
	wheelShape.localPosition.SetZero();
	wheelShape.isSensor = false;
	wheelShape.restitution = 0.0;
	wheelShape.density = 10;
	wheelShape.friction = 0.7;
	
	_pWheelShape = _pMainWheel->CreateShape( & wheelShape );
	_pMainWheel->SetMassFromShapes();
	
	// leg
	_shapeLeg.clear();
	_shapeLeg.append( QPointF(-0.2, -0.2 * _orientation) );
	_shapeLeg.append( QPointF( 0.3, -1.6 * _orientation) );
	_shapeLeg.append( QPointF( 0.7, -1.6 * _orientation) );
	_shapeLeg.append( QPointF( 0.2, -0.2 * _orientation) );
	b2PolygonDef legShape = shapeToDef( _shapeLeg, _orientation < 0 );
	legShape.friction = 0;
	legShape.restitution = 0.1;
	legShape.density = 20;
	
	_pLegShape = _pMainLeg->CreateShape( & legShape );
	
	_pMainLeg->SetMassFromShapes();
	
	// engine
	_shapeEngine.clear();
	_shapeEngine.append( QPointF( 1.3, 0.6 *  _orientation) );
	_shapeEngine.append( QPointF( 1.3, -0.6 *  _orientation) );
	_shapeEngine.append( QPointF( 1.8, -0.5 *  _orientation) );
	_shapeEngine.append( QPointF( 1.8, 0.5 *  _orientation) );
	b2PolygonDef engineShape = shapeToDef( _shapeEngine, _orientation < 0 );
	engineShape.friction	= mainShape.friction;
	engineShape.restitution	= mainShape.restitution;
	engineShape.density		= 10;
	engineShape.userData	= _pEngineDamageManager;
	
	_pEngineShape = _pEngineCase->CreateShape( & engineShape );
	_pEngineCase->SetMassFromShapes();

}

// ============================================================================
// Destructor
Plane::~Plane()
{
	// none
}

// ============================================================================
// Converts shape to b2d polygon def
b2PolygonDef Plane::shapeToDef( const QPolygonF& shape, bool reversed )
{
	b2PolygonDef def;
	def.vertexCount = 0;
	
	if ( shape.size() > 8 )
	{
		qWarning("Shape has too many vertices");
		return def;
	}
	
	def.vertexCount = shape.size();
	for( int i = 0; i < def.vertexCount; i++ )
	{
		int vertexIndex = reversed ? (def.vertexCount - i - 1) : i;
		def.vertices[vertexIndex].Set( shape[i].x(), shape[i].y() );
	}
	
	return def;
}

// ============================================================================
// Draws a plane.
void Plane::render( QPainter& painter, const QRectF& rect )
{
	painter.setPen( Qt::black );
	painter.setBrush( Qt::gray );

	// draw body and leg
	const b2Vec2& pos = _pMainBody->GetPosition();
	QTransform mainTransform;
	mainTransform.translate( pos.x, pos.y );
	mainTransform.rotateRadians( _pMainBody->GetAngle() );
	
	QPainterPath path;
	path.addPolygon( _shapeBody );
	path.closeSubpath();
	path.addPolygon( _shapeLeg );
	path.closeSubpath();
	path.addPolygon( _shapeSkid );
	path.closeSubpath();
	
	path.setFillRule( Qt::WindingFill );
	
	// draw elevator
	double dx = cos( (_elevator*ELEVATOR_STEP*_orientation) ) * ELEVATOR_LENGTH;
	double dy = sin( (_elevator*ELEVATOR_STEP*_orientation) ) * ELEVATOR_LENGTH;
	
	// draw wings
	const double WING_WIDTH = 1.5;
	double wx = cos( (WING_INCLINATION*_orientation) ) * WING_WIDTH/2;
	double wy = sin( (WING_INCLINATION*_orientation) ) * WING_WIDTH/2;
	
	painter.save();
		painter.setTransform( mainTransform, true );
		painter.drawPath( path );
		painter.drawLine( QLineF( ELEVATOR_POS.x(), ELEVATOR_POS.y(), ELEVATOR_POS.x() - dx, ELEVATOR_POS.y()-dy ) );
		painter.drawLine( QLineF( -wx, -wy, wx, wy ) );
		
	painter.restore();

	// draw wheel
	const b2Vec2& wheelPos = _pMainWheel->GetPosition();
	double angle = _pMainWheel->GetAngle();
	QTransform wheelTransform;
	wheelTransform.translate( wheelPos.x, wheelPos.y );
	wheelTransform.rotateRadians( angle );
	
	painter.save();
		painter.setTransform( wheelTransform, true );
		painter.drawEllipse( QRectF(-WHEEL_RADIUS, -WHEEL_RADIUS, WHEEL_RADIUS*2, WHEEL_RADIUS*2 ) );
		painter.drawLine( QLineF( -WHEEL_RADIUS, 0.0, WHEEL_RADIUS, 0.0 ) );
		
	painter.restore();
	
	// draw systems (only this should left here )
	foreach( System* pSystem, _systems[ SystemRendered1 ] )
	{
		pSystem->render( painter, rect );
	}
	foreach( System* pSystem, _systems[ SystemRendered2 ] )
	{
		pSystem->render( painter, rect );
	}
	foreach( System* pSystem, _systems[ SystemRendered3 ] )
	{
		pSystem->render( painter, rect );
	}
	
	// draw forces
	double fs = 50 ; // force scale
	QPointF wf = wingsForce() / fs;
	QPointF ef = elevatorForce() / fs;
	
	painter.setPen( Qt::blue );
	painter.drawLine( QLineF( pos.x, pos.y, pos.x + wf.x(), pos.y + wf.y() ) );
	
	painter.setPen( Qt::darkYellow );
	painter.drawLine( QLineF( elevatorPos(), elevatorPos() + ef ) );
	
}

// ============================================================================
// Simulates plane
void Plane::simulate( double dt )
{
	const b2Vec2& pos = _pMainBody->GetPosition();
	
	QPointF wings	= wingsForce();
	_pMainBody->ApplyForce
		( 10.0 * b2Vec2( wings.x(), wings.y() ) // newtons per kg
		, pos );
	
	
	// elevator
	QPointF eforce = elevatorForce();
	QPointF epoint = elevatorPos();
	_pMainBody->ApplyForce
		( 10.0 * b2Vec2( eforce.x(), eforce.y() )
		, b2Vec2( epoint.x(), epoint.y() )
		);
	
	// wheel brake
	if ( _wheelBrake )
	{
		_pMainWheelJoint->SetMaxMotorTorque( WHEEL_BRAKE_TORQUE );
		_pMainWheelJoint->SetMotorSpeed( 0.0 );
	}
	else
	{
		_pMainWheelJoint->SetMaxMotorTorque( 0.0 );
	}
	
	// autopilot
	if ( _autopilot )
	{
		simulateAutopilot( dt );
	}
	
	// simul;ate sustrem (only this should be left here)
	foreach ( System* pSystem, _systems[ SystemSimulated ] )
	{
		pSystem->simulate( dt );
	}
}

// ============================================================================
// Sets throttle
void Plane::setThrottle( double t )
{
	_pEngine->setThrottle( t );
}

// ============================================================================
// Returns throtlle
double Plane::throttle() const
{
	return _pEngine->throttle();
}

// ============================================================================
// Sets elevator -1 - to 1
void Plane::setElevator( double e )
{
	if ( _autopilot )
	{
		return; // do nothing
	}
	
	if ( e < -1.0 )
		_elevator = -1.0;
	else if ( e > 1.0 )
		_elevator = 1.0;
	else 
		_elevator = e;
}

// ============================================================================
// Set flaps = 0-1
void Plane::setFlaps( double f )
{
	if ( f < 0 )
		_flaps = 0.0;
	else if ( f > 1.0 )
		_flaps = 1.0;
	else 
		_flaps = f;
}


// ============================================================================
// Aerodynamic force
QPointF Plane::wingsForce()
{
	double angle = _pMainBody->GetAngle(); // hull angle
	
	b2Vec2 velocity = _pMainBody->GetLinearVelocity();
	double v = sqrt( velocity.x*velocity.x + velocity.y*velocity.y );
	double velAngle = atan2( velocity.y, velocity.x );
	
	double attack = angle + (WING_INCLINATION*_orientation) - velAngle; // angle of attack
	double sina = sin(attack);

	// lift
	double lift = v*v * ( WING_LIFT + FLAPS_EXTRA_LIFT*_flaps ) * sina;
	
	// drag
	double dragH = WING_DRAG_H + FLAPS_EXTRA_DRAG*_flaps;
	double drag = v*v * (dragH +( WING_DRAG_V-dragH) * sina*sina );
	
	return QPointF
		( -drag*cos(angle) - lift*sin(angle)
		, -drag*sin(angle) + lift*cos(angle)
		);
	
}

// ============================================================================
// Returns plane position
QPointF Plane::pos() const
{
	const b2Vec2& p = _pMainBody->GetPosition();
	
	return QPointF( p.x, p.y );
}

// ============================================================================
// Calcultes and applies elevator force
QPointF Plane::elevatorForce()
{
	double angle = _pMainBody->GetAngle(); // hull angle
	
	b2Vec2 velocity = _pMainBody->GetLinearVelocity();
	double v = sqrt( velocity.x*velocity.x + velocity.y*velocity.y );
	double velAngle = atan2( velocity.y, velocity.x );
	
	double attack = angle + (_elevator*ELEVATOR_STEP*_orientation) - velAngle; // elevator's angle of attack
	double sina = sin(attack);

	// lift
	const double l = 0.5; // elev lift coeff
	double lift = v*v * l * sina;
	
	// no drag
	double drag = 0;
	
	return QPointF
		( -drag*cos(angle) - lift*sin(angle)
		, -drag*sin(angle) + lift*cos(angle)
		);
}

// ============================================================================
// Elevator position
QPointF Plane::elevatorPos()
{
	double angle = _pMainBody->GetAngle(); // hull angle
	b2Vec2 pos = _pMainBody->GetPosition();
	
	return QPointF( pos.x + ELEVATOR_POS.x()*cos(angle), pos.y + ELEVATOR_POS.x()*sin(angle) ); // TODO elvetor Y here
}

// ============================================================================
// Air speed
double Plane::airspeed()
{
	double angle = _pMainBody->GetAngle(); // hull angle
	b2Vec2 velocity = _pMainBody->GetLinearVelocity();
	double v = sqrt( velocity.x*velocity.x + velocity.y*velocity.y );
	double velAngle = atan2( velocity.y, velocity.x );
	
	return v * cos( angle - velAngle );
}

// ============================================================================
// Turbns wheel brake on/off
void Plane::applyWheelBrake( bool on )
{
	_wheelBrake = on;
}

// ============================================================================
// flips plane to the other side. destroys all shapes and creates them ab ovo
void Plane::flip()
{
	// first - ckeck airspeed
	if ( airspeed() < MIN_FLIP_SPEED && airspeed() > MAX_TURN_SPEED )
	{
		qDebug("too slow to flip, tu fats to turn");
		return; // no flip below this speed
	}
	
	// flip?
	bool turn = airspeed() <= MAX_TURN_SPEED; // turn otherwise
	
	
	_orientation = _orientation * -1;
	
	// get data which are to be preserved
	b2Vec2 pos = _pMainBody->GetPosition();
	double angle = _pMainBody->GetAngle();
	b2Vec2 linearSpeed = _pMainBody->GetLinearVelocity();
	double angularSpeed = _pMainBody->GetAngularVelocity();
	// TODO other bodies speeds
	
	//if turn, change angle
	if ( turn )
	{
		angle = M_PI-angle;
	}
	
	// destroy bodies
	b2World* pWorld = world()->b2world();
	
	pWorld->DestroyBody( _pMainBody );
	pWorld->DestroyBody( _pMainWheel );
	pWorld->DestroyBody( _pMainLeg );
	
	createBodies( QPointF( pos.x, pos.y ), angle );
	createShapes( QPointF( pos.x, pos.y ), angle );
	
	_pMainBody->SetLinearVelocity( linearSpeed );
	_pMainBody->SetAngularVelocity( angularSpeed );
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
	if ( ! _autopilot )
	{
		_apErrorIntegral = 0.0;
		_apPreviousError = 0.0;
	}
	_autopilot = on;
}

// ============================================================================
// Sets elevator using autopilot logic.
void Plane::simulateAutopilot( double dt )
{
	// first - get angle. this is our error
	b2Vec2 velocity = _pMainBody->GetLinearVelocity();
	
	// abort operation when moving too slow
	if ( airspeed() < AP_MIN_AIRSPEED )
	{
		_elevator = 0.0;
		return;
	}
	
	double error = velocity.y / fabs( velocity.x ); // approx angle in radians
	
	// update integral
	_apErrorIntegral += error*dt;
	
	// get derivative
	double der = ( error - _apPreviousError ) / dt;
	_apPreviousError = error;
	
	// apply steering
	_elevator = ( AP_P * error + AP_I * _apErrorIntegral + AP_D * der );
	if ( _elevator > 1.0 ) _elevator = 1.0;
	if ( _elevator < -1.0 ) _elevator = -1.0;

}

// ============================================================================
// Elevator angle
double Plane::elevatorAngle() const
{
	return _elevator * ELEVATOR_STEP;
}

// ============================================================================
//. Adds system to aircraft. types is  bitmask with system types.
void Plane::addSystem( System* pSystem, int types )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		int bit = 1<<i;
		if ( types & bit )
		{
			_systems[ bit ].append( pSystem );
		}
	}
}

// ============================================================================
// Removes system from all lists
void Plane::removeSystem( System* pSystem )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		_systems[ 1<<i ].removeAll( pSystem );
	}
}

}
