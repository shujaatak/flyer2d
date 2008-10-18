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

#include "Box2D.h"

#include "world.h"
#include "b2dqt.h"
#include "body.h"
#include "damagemanager.h"
#include "common.h"

#include "explosion.h"

namespace Flyer
{

static const double DAMAGE_MULTIPLIER	= 10; ///< Explosion damage multiplier

// ============================================================================
// Constructor
Explosion::Explosion ( World* pWorld ) : WorldObject ( pWorld )
{
	_radius = 0;
	_maxFireRadius = 0;
	_currentStep = 0;
	_maxRadius = 0;
	
	_speed = 85; // 1/4 the speed of sound
}

// ============================================================================
// Destructor
Explosion::~Explosion()
{
}

// ============================================================================
// Bounding rect
QRectF Explosion::boundingRect() const
{
	// return max possible bounding rect, to avoid updating broadphase each step
	return QRectF ( _center.x - _maxRadius, _center.y - _maxRadius, _maxRadius*2, _maxRadius*2 );
}

// ============================================================================
// Sets energy. Also - calculates maxima radius
void Explosion::setEnergy( double e )
{
	// Force will be equal e/(r*r+1),
	// soe at zero this will be e.
	// Let minimal sensible force is 100N
	// f = e/(r*r+1)
	// f*r*r+f = e
	// r = sqrt( (e-f)/f )
	double minForce = qMin( 10E3*DAMAGE_MULTIPLIER, e );
	_maxRadius = qMax( 5.0, sqrt( (e-minForce)/minForce ) );
	_maxFireRadius = _maxRadius/4; // dumb guess
	_energy = e;
}

// ============================================================================
// Renders
void Explosion::render ( QPainter& painter, const QRectF&, const RenderingOptions&  )
{
	// render fire
	double fireRadius = qMin( _radius*0.9, _maxFireRadius );
	QRectF fire( _center.x - fireRadius, _center.y - fireRadius, fireRadius*2, fireRadius*2 );
	QRectF shockwave( _center.x - _radius, _center.y - _radius, _radius*2, _radius*2 );
	
	painter.setPen( Qt::NoPen );
	painter.setBrush( QColor( 128, 0, 0, 200 ) );
	
	painter.drawEllipse( fire );
	
	// render shockwave
	painter.setPen( QPen( QColor( 0,0,0,32 ), 1 ) );
	painter.setBrush( Qt::NoBrush );
	
	painter.drawEllipse( shockwave );
	

}

// ============================================================================
// Simulates
void Explosion::simulate ( double dt )
{
	// affect bodies, spread damage
	actWithForce();
	
	// move simulation forward
	_radius += dt*_speed;
	_currentStep++;
	
	
	if ( _radius > _maxRadius )
	{
		world()->removeObject( this );
	}
}

// ============================================================================
/// Acts with a force on body
void Explosion::actWithForce()
{
	// find range of force in current step
	double minRange = _radius; // current radius
	double maxRange = qMin( _radius + _speed * world()->timestep(), _maxRadius );
	
	if ( maxRange < 0.01 ) return; // do nothing for small damage
	
	// find bodies within range
	b2AABB aabb;
	aabb.lowerBound.Set( _center.x-maxRange, _center.y-maxRange );
	aabb.upperBound.Set( _center.x+maxRange, _center.y+maxRange );
	
	const int bufsize = 100; // firs 100 lucky ibjects. rest will go untouched. Maybe use some heuristiv value basing on radius?
	b2Shape* shapes[ bufsize ];
	int count = world()->b2world()->Query(aabb, shapes, bufsize);
	
	// ok, now i have 'count' shapes. Acti with force on bodies and damage managers
	
	QList<b2Body*> bodiesTouched; // to not repeat
	
	for( int i = 0; i < count; i++ )
	{
		// get body
		b2Body* pb2Body = shapes[i]->GetBody();
		
		// find distance, normal vector and force value
		double distance = ( pb2Body->GetPosition() - _center ).Length();

		if ( distance < maxRange &&  distance >= minRange )
		{
			b2Vec2 diff = pb2Body->GetPosition() - _center;
			b2Vec2 normal = diff;
			normal.Normalize();
			
			double force = _energy / ( distance*distance+1);
		
			// act with force on body (but only once)
			if ( ! bodiesTouched.contains( pb2Body ) )
			{
				pb2Body->ApplyForce( force*normal, pb2Body->GetPosition() );
				bodiesTouched.append( pb2Body );
			}
			
			// damge DM
			Body* pBody = static_cast<Body*>( pb2Body->GetUserData() );
			if ( pBody )
			{
				pBody->contact( force * DAMAGE_MULTIPLIER );
				//qDebug("Explosion: contact with body %s, force: %g", qPrintable(pBody->name()),force * DAMAGE_MULTIPLIER ); 
			}
			
		}
	}
}

// ============================================================================
/// Creates explosion
void Explosion::explode( World* pWorld, const b2Vec2& center, double energy )
{
	Explosion* pExplosion = new Explosion( pWorld );
	pExplosion->setEnergy( energy );
	pExplosion->setCenter( center );
	pExplosion->setRenderLayer( LayerForeground );

	//add explosion to the world
	pWorld->addObject( pExplosion, World::ObjectSimulated );
	pWorld->addDecoration( pExplosion );
	
	//qDebug("BOOOOM!!!");
}

}
