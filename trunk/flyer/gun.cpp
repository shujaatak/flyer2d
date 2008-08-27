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

#include "gun.h"
#include "bullet.h"
#include "world.h"
#include "plane.h"
#include "b2dqt.h"

namespace Flyer
{

static const double DAMAGED_INTERVAL	= 10.0;	///< Firing interval when damaged
static const double DAMAGED_VELOCITY	= 0.5;	///< Bullet velocity when damaged

// ============================================================================
// Constructor
Gun::Gun ( Machine* pParent, const QString& name ) : System ( pParent, name )
{
	_firing = false;
	_timeFromLastFiring = 0;
	_broken = false;
}

// ============================================================================
// Destructor
Gun::~Gun()
{
}

// ============================================================================
// Damages gun
void Gun::damage ( double force )
{
	if ( ! _broken )
	{
		double reduce = force / damageCapacity();
		
		switch ( qrand() % 2 )
		{
			// reduce fire rate
			case 0:
			{
				_currentnInterval += _interval * reduce * ( DAMAGED_INTERVAL - 1.0 );
				//qDebug("GUN: firing interval extended to %g from %g, force: %g", _currentnInterval, _interval, force );
				break;
			}
			// reduce velocity
			case 1:
			{
				_currentVelocity -= _velocity * reduce * ( 1.0 - DAMAGED_VELOCITY );
				//qDebug("GUN: Velocity reduced to %g form %g, force %g",  _currentVelocity, _velocity, force );
			}
			// TODO add deflection
		}
		
		// and maybe is totally borken?
		if ( ( qrand() % 1000 ) < ( reduce * 1000 ) )
		{
			_broken = true;
			//qDebug("Gun totally broken (chances where %g)", reduce);
		}
	}
	
}

// ============================================================================
// Smulates gun
void Gun::simulate( double dt )
{
	_timeFromLastFiring += dt;
	
	if ( body()->b2body() )
	{
		// it's time for firing?
		if ( ! _broken && _firing && ( _timeFromLastFiring > _currentnInterval ) )
		{
			b2Body* pBody = body()->b2body();
			
			// create bullet
			Bullet* pBullet = new Bullet( parent()->world() );
			parent()->world()->addObject( pBullet, World::ObjectSimulated | World::ObjectRenderedForeground );
			
			pBullet->setMass( _mass );
			pBullet->setLifespan( _lifespan );
			pBullet->setSize( _size );
			
			// fire bullet
			b2Vec2 startPoint = pBody->GetWorldPoint( point2vec( _muzzle ) );
			b2Vec2 endPoint = pBody->GetWorldPoint( point2vec( _muzzle + _normal ) );
			
			b2Vec2 normal = endPoint - startPoint;
			
			QPointF velocity = vec2point( normal ) * _currentVelocity;
			
			pBullet->fire( vec2point( startPoint ), velocity );
			
			// apply reverse impulse to parent body
			//a = f/m;
			//v = a*t;
			
			//v = f*t/m
			//f = v*m/t
			double reaction = _velocity*_mass / dt;
			pBody->ApplyImpulse( -reaction*normal, startPoint );
			
			// reset timer
			_timeFromLastFiring = 0;
			
		}
		
	}
	
}

// ============================================================================
/// Estimates damage status, from 1.0 - fully operational to 0.1 - fully damaged.
double Gun::status() const
{
	if ( _broken )
	{
		return 0.0;
	}
	else
	{
		double intervalDamage = 1.0 - ( 1.0 - _interval / _currentnInterval ) / ( DAMAGED_INTERVAL - 1.0 );
		double velocityDamage = 1.0 - ( 1.0 - _currentVelocity / _velocity ) / ( 1.0 - DAMAGED_VELOCITY ); 
		
		return 0.5 * ( intervalDamage + velocityDamage );
	}
}

}
