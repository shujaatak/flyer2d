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

#include "common.h"
#include "engine.h"
#include "damagemanager.h"
#include "b2dqt.h"
#include "body.h"
#include "joint.h"
#include "mounting.h"
#include "engine.h"
#include "wing.h"
#include "controlsurface.h"
#include "wheelbrake.h"
#include "autopilot.h"
#include "gun.h"
#include "activeattachpoint.h"

#include "plane.h"



namespace Flyer
{
static const double MIN_FLIP_SPEED = 30;	///< min speed for flip
static const double MAX_TURN_SPEED = 0.3;	///< max speed for turn




// ============================================================================
// Constructor
Plane::Plane( World* pWorld, const QPointF& /*pos*/, double /*angle*/ ) : Machine( pWorld )
{
	_pEngine	= NULL;
	_pWing		= NULL;
	_pBrake		= NULL;
	_pAutopilot	= NULL;
	_pElevator	= NULL;
	_pGun		= NULL;
	_pWeaponAttachPoint  = NULL;
	setLayers( PhysLayerVehicles | PhysLayerBuildings );
	setRenderLayer( LayerVehicles );
}

// ============================================================================
// Destructor
Plane::~Plane()
{
	// none
}


// ============================================================================
// Sets throttle
void Plane::setThrottle( double t )
{
	if ( _pEngine ) _pEngine->setThrottle( t );
}

// ============================================================================
// Returns throtlle
double Plane::throttle() const
{
	if ( _pEngine ) return _pEngine->throttle();
	return 0.0;
}

// ============================================================================
// Sets elevator -1 - to 1
void Plane::setElevator( double e )
{
	if ( autopilot() )
	{
		return; // do nothing
	}
	
	if ( _pElevator ) _pElevator->setValue( e );
}

// ============================================================================
// Set flaps = 0-1
void Plane::setFlaps( double f )
{
	if ( _pWing ) _pWing->setFlaps( f );
}

// ============================================================================
// Air speed
double Plane::airspeed()
{
	if ( mainBody() && mainBody()->b2body() )
	{
		double angle = mainBody()->b2body()->GetAngle(); // hull angle
		b2Vec2 velocity = mainBody()->b2body()->GetLinearVelocity();
		double v = sqrt( velocity.x*velocity.x + velocity.y*velocity.y );
		double velAngle = atan2( velocity.y, velocity.x );
		
		return v * cos( angle - velAngle );
	}
	
	return 0.0;
}

// ============================================================================
// flips plane to the other side. destroys all shapes and creates them ab ovo
void Plane::flipPlane()
{
	// do nothing if hull not defined
	if ( ! mainBody() || ! mainBody()->b2body() )
	{
		return;
	}
	
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
	
	b2Vec2 pos = mainBody()->b2body()->GetPosition();
	//if turn, change angle
	if ( turn )
	{
		// for turn - rotate around vertical axis
		p1 = QPointF( pos.x, pos.y );
		p2 = QPointF( pos.x, pos.y + 1.0 );
		
	}
	else
	{
		// for speed flip - rotate around velocity vector
		b2Vec2 vel = mainBody()->b2body()->GetLinearVelocity();
	
		p1 = QPointF ( pos.x, pos.y );
		p2 = QPointF ( pos.x + vel.x, pos.y + vel.y );
	}
	
	flip( p1, p2 );
}

// ============================================================================
// Renders plane on map
void Plane::renderOnMap( QPainter& painter, const QRectF& /*rect*/ )
{
	painter.setPen( Qt::red );
	painter.setBrush( Qt::white );
	
	QPointF p = position();
	double size = 200; // 20 meters
	QRectF ellipse( p.x() - size/2, p.y() - size/2, size, size );
	painter.drawEllipse( ellipse );
}

// ============================================================================
// Sets autopilot on/off
void Plane::setAutopilot( bool on )
{
	if ( _pAutopilot ) _pAutopilot->setOn( on );
}

// ============================================================================
// Elevator angle
double Plane::elevatorAngle() const
{
	if ( _pElevator ) return _pElevator->angle();
	return 0.0;
}

// ============================================================================
// Sets wheel brake on/off
void Plane::applyWheelBrake( bool on )
{
	if ( _pBrake ) _pBrake->setOn( on );
}

// ============================================================================
// Returns whele brake status
bool Plane::wheelBrake() const
{
	if ( _pBrake ) return _pBrake->on();
	return false;
}

// ============================================================================
// Returns flaps value
double Plane::flaps() const
{
	if ( _pWing ) return _pWing->flaps();
	return 0.0;
}

// ============================================================================
// Returns autopilot status
bool Plane::autopilot() const
{
	if ( _pAutopilot ) return _pAutopilot->on();
	return false;
}

// ============================================================================
// Sets gun status
void Plane::setFiring( bool on )
{
	if ( _pGun ) _pGun->setFiring( on );
}

// ============================================================================
// Releases attached weapon
void Plane::releaseWeapon()
{
	if ( _pWeaponAttachPoint ) _pWeaponAttachPoint->release();
}

}
