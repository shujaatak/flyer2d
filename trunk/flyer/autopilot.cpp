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

#include "autopilot.h"
#include "body.h"
#include "controlsurface.h"

namespace Flyer
{

static const double AP_MIN_AIRSPEED = 10;	// m/s

// ============================================================================
// Constructor
Autopilot::Autopilot ( Plane* pParent, const QString& name ) : System ( pParent, name )
{
	_previousError = 0;
	_errorIntegral = 0;
	_on = false;
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
	qDebug("TODO: implement autopilot damage");
}

// ============================================================================
// Simulates autopilot
void Autopilot::simulate ( double dt )
{
	if ( _on && body() && body()->b2body() && _pElevator )
	{
		// TODO get data from instruments, do not use real data
		// first - get angle. this is our error
		b2Vec2 velocity = body()->b2body()->GetLinearVelocity();
		
		// abort operation when moving too slow
		if ( velocity.Length() < AP_MIN_AIRSPEED )
		{
			_pElevator->setValue( 0.0 );
			return;
		}
		
		double error = velocity.y / fabs( velocity.x ); // approx angle in radians
		
		// update integral
		_errorIntegral += error*dt;
		
		// get derivative
		double der = ( error - _previousError ) / dt;
		_previousError = error;
		
		// apply steering
		_pElevator->setValue( _P * error + _I * _errorIntegral + _D * der );
		
		// usefule debug:
		//qDebug("AP: error: %g, integral: %g, derrivative: %g. Elevator: %g"
		//	, error, _errorIntegral, der, _pElevator->value());
	}
	else
	{
		_errorIntegral = 0.0;
		_previousError = 0.0;
	}
}

}
