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

// ============================================================================
// Constructor
Gun::Gun ( Machine* pParent, const QString& name ) : System ( pParent, name )
{
	_firing = false;
	_timeFromLastFiring = 0;
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
	qDebug("TODO: implement gun damage");
}

// ============================================================================
// Smulates gun
void Gun::simulate( double dt )
{
	_timeFromLastFiring += dt;
	
	if ( body()->b2body() )
	{
		// it's time for firing?
		if ( _firing && ( _timeFromLastFiring > _interval ) )
		{
			b2Body* pBody = body()->b2body();
			
			// create bullet
			Bullet* pBullet = new Bullet( parent()->world() );
			parent()->world()->addObject( pBullet );
			
			pBullet->setMass( _mass );
			pBullet->setLifespan( _lifespan );
			pBullet->setSize( _size );
			
			// fire bullet
			b2Vec2 startPoint = pBody->GetWorldPoint( point2vec( _muzzle ) );
			b2Vec2 endPoint = pBody->GetWorldPoint( point2vec( _muzzle + _normal ) );
			
			b2Vec2 normal = endPoint - startPoint;
			
			QPointF velocity = vec2point( normal ) * _velocity;
			
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

}
