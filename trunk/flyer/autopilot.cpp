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

#include <QPainter>

#include "body.h"
#include "controlsurface.h"
#include "engine.h"
#include "b2dqt.h"
#include "wing.h"

#include "autopilot.h"

namespace Flyer
{

static const double AP_MIN_AIRSPEED = 10;	// m/s

// ============================================================================
// Constructor
Autopilot::Autopilot ( Machine* pParent, const QString& name ) : System ( pParent, name )
{
	_previousError = 0;
	_errorIntegral = 0;
	_posErrorIntegral = 0;
	_on = false;
	_mode = KeepLevel;
	_pEngine = NULL;
	_pElevator = NULL;
	_pWing = NULL;
}

// ============================================================================
// Destructor
Autopilot::~Autopilot()
{
}

// ============================================================================
// Simulates damage
void Autopilot::damage ( double force )
{
	// none
}

// ============================================================================
// Simulates autopilot
void Autopilot::simulate ( double dt )
{
	if ( _on )
	{
		if ( _mode == KeepLevel )
		{
			controlKeepLevel( dt );
		}
		else if ( _mode == FollowTrack )
		{
			controlFollowPath( dt );
		}
	}
	else
	{
		_errorIntegral = 0.0;
		_previousError = 0.0;
		_posErrorIntegral = 0.0;
	}
}

// ============================================================================
// Provides plane position
b2Vec2 Autopilot::position()
{
	if ( body() && body()->b2body() )
	{
		return body()->b2body()->GetPosition();
	}
	
	return b2Vec2( 0, 0 );
}

// ============================================================================
// Provides plane velocity
b2Vec2 Autopilot::velocity()
{
	if ( body() && body()->b2body() )
	{
		return body()->b2body()->GetLinearVelocity();
	}
	
	return b2Vec2( 0, 0 );
}

// ============================================================================
/// Impelments 'keep level' mode
void Autopilot::controlKeepLevel( double dt )
{
	if ( _pElevator )
	{
		// TODO get data from instruments, do not use real data
		// first - get angle. this is our error
		b2Vec2 vel = velocity();
		
		// abort operation when moving too slow
		if ( vel.Length() < AP_MIN_AIRSPEED )
		{
			_pElevator->setValue( 0.0 );
			return;
		}
		
		double error = vel.y / fabs( vel.x ); // approx angle in radians
		
		// update integral
		_errorIntegral += error*dt;
		
		// get derivative
		double der = ( error - _previousError ) / dt;
		_previousError = error;
		
		// apply steering
		_pElevator->setValue( _settings.P * error + _settings.I * _errorIntegral + _settings.D * der );
		
		// usefule debug:
		//qDebug("AP: error: %g, integral: %g, derrivative: %g. Elevator: %g"
		//	, error, _errorIntegral, der, _pElevator->value());
	}
}

// ============================================================================
/// implements 'follow path' mode
void Autopilot::controlFollowPath( double dt )
{
	// check preconditions
	if ( _track.isEmpty() )
	{
		qDebug("Autipolot has no track to follow. Switching back to 'keep level' mode.");
		controlKeepLevel( dt );
		return;
	}
	
	// get current segment anf plane pos
	TrackSegment& segment = _track.first();
	b2Vec2 pos = position();
	b2Vec2 vel = velocity();
	double airspeed = vel.Length();
	
	// do nothing if plane too slow
	if ( airspeed < _settings.minSpeed && ! segment.orders.contains( Land ) )
	{
		// speedup
		if ( _pElevator ) _pElevator->setValue( 0.0 );
		if ( _pEngine ) _pEngine->setThrottle( _pEngine->throttle() + 0.01 );
		return;
	}
	
	// find nearest point to line
	// NOTE similar code used in body flip
	b2Vec2& p1 = segment.start;
	b2Vec2& p2 = segment.end;
	
	double segmentAngle = atan2( p2.y - p1.y, p2.x - p1.x );
	double ds = (p2 - p1).LengthSquared(); // distance squared between p1 nad p2
	double u = ( ( pos.x - p1.x ) * (p2.x - p1.x) + (pos.y - p1.y) * (p2.y - p1.y)) / ds; // helper value
	
	// u is conveniet value to detect relative position to segment ends
	if ( u > 1.0 )
	{
		_track.pop_front();
		qDebug("autopilot: taking next segment");
		controlFollowPath( dt ); // aim at next segment
		return;
	}
	
	// closest point on track
	double cx = p1.x+ u * (p2.x - p1.x);
	double cy = p1.y+ u * (p2.y - p1.y);

	// decide what to do - do aerobatics or simple flight
	if ( segmentAngle < _settings.safeAngle )
	{
		// simple flight, minimize vertical distance from track
		
		double posError = cy - pos.y;
		if ( fabs( posError ) < 10 ) // TODO some heuristic value
		{
			_posErrorIntegral += posError*dt;
		}
		else
		{
			_posErrorIntegral = 0;
		}
		
		double desiredVSpeed = posError * _settings.VVP + _posErrorIntegral * _settings.VVI;
		double maxVSpeed = _settings.maxClimbRate * fabs( vel.x );
		double minVSpeed = - _settings.maxDescentRate * fabs( vel.x );
		
		if ( desiredVSpeed > maxVSpeed ) desiredVSpeed = maxVSpeed;
		else if ( desiredVSpeed < minVSpeed ) desiredVSpeed = minVSpeed;
		
		//qDebug("vertical error: %g, desired v speed: %g, current v speed: %g", posError, desiredVSpeed, vel.y );
		
		// now control vertical speed using The Good Old Way (tm ) (a.k.a. PID )
		double error = (  vel.y - desiredVSpeed ) / fabs( vel.x );
		
		// update integral
		if ( fabs( error ) < 10 )
		{
			_errorIntegral += error*dt;
		}
		else
		{
			_errorIntegral = 0.0;
		}
		
		// get derivative
		double der = ( error - _previousError ) / dt;
		_previousError = error;
		
		// apply steering
		double elevator = _settings.P * error + _settings.I * _errorIntegral + _settings.D * der;
		if ( _pElevator )
		{
			_pElevator->setValue( elevator );
		}
		//qDebug("PID: pos error: %g, error: %g, integral: %g, der: %g, input: %g",posError, error, _errorIntegral, der, elevator );
	}
	else
	{
		// aerobatics - minimize angle error
		// TODO
		qDebug("don't know how to do aerobatics");
	}
	
	// control speed
	if ( segment.airspeed >= 0 && _pEngine )
	{
		// TODO this is stupid]
		double airspeed = vel.Length();
		double speedError = airspeed - segment.airspeed;
		double toleratedError = segment.airspeed * 0.03; // +/- 3% error
		if ( speedError < -toleratedError )
		{
			_pEngine->setThrottle( _pEngine->throttle() + 0.01 );
		}
		else if ( speedError > toleratedError )
		{
			_pEngine->setThrottle( _pEngine->throttle() - 0.01 );
		}
		
		// if current or desired speed below flpas speed - opne flaps
		if ( ( segment.airspeed >= 0 ) 
			&& ( airspeed < _settings.flapsCloseSpeed || segment.airspeed < _settings.flapsOpenSpeed )
			)
		{
			//qDebug("opening flasp: speed: %g, desired speed: %g", airspeed, segment.airspeed );
			if ( _pWing ) _pWing->setFlaps( 1.0 );
		}
		else
		{
			//qDebug("closing claps, speed: %g", airspeed );
			if ( _pWing ) _pWing->setFlaps( 0.0 );
		}
	}

	// control flaps
}

// ============================================================================
/// Debug render autopliot track
void Autopilot::render( QPainter& painter, const QRectF&, const RenderingOptions& )
{
	painter.setPen( QPen( Qt::blue, 0 ) );
	
	foreach( const TrackSegment& segment, _track )
	{
		painter.drawLine( vec2point( segment.start ), vec2point( segment.end ) );
	}
}

}
