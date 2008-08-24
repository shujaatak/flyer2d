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

#include "explosion.h"

namespace Flyer
{

// ============================================================================
// Constructor
Explosion::Explosion ( World* pWorld ) : WorldObject ( pWorld )
{
	_radius = 0;
	_maxFireRadius = 0;
	_intialized = false;
	_currentStep = 0;
	
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
	return QRectF ( _center.x() - _radius, _center.y() - _radius, _radius*2, _radius*2 );
}

// ============================================================================
// Sets energy. Also - calculates maxima radius
void Explosion::setEnergy( double e )
{
	// Force will be equal e/(r*r+1),
	// soe at zero this will be e.
	// Let minimal sensible force is 100N
	// f = e/(r2+1)
	// f*r2+f = e
	// r2 = (e-f)/f
	// r = sqrt( (e-f)/f )
	double minForce = 10000;
	_maxRadius = sqrt( (e-minForce)/minForce );
	_maxFireRadius = _maxRadius/4; // dumb guess
	_energy = e;
	
}

// ============================================================================
// Renders
void Explosion::render ( QPainter& painter, const QRectF& )
{
	// render fire
	double fireRadius = qMin( _radius*0.9, _maxFireRadius );
	QRectF fire( _center.x() - fireRadius, _center.y() - fireRadius, fireRadius*2, fireRadius*2 );
	QRectF shockwave( _center.x() - _radius, _center.y() - _radius, _radius*2, _radius*2 );
	
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
	// inti itialize
	if ( ! _intialized )
	{
		findBodies();
		_intialized = true;
	}
	
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
// Finds boieds in explosion area
void Explosion::findBodies()
{
	// find out how much simulation steps explosion will last
	double timestep = world()->timestep();
	int steps = int( ceil( (_maxRadius/_speed) / timestep ) );
	_bodiesAffected.resize( steps );
	_damageManagers.resize( steps );
	
	// find box2d bodies of interest
	b2AABB aabb;
	aabb.lowerBound.Set( _center.x()-_maxRadius, _center.y()-_maxRadius );
	aabb.upperBound.Set( _center.x()+_maxRadius, _center.y()+_maxRadius );
	
	const int bufsize = 100; // firs 100 lucky ibjects. rest will go untouched. Maybe use some heuristiv value basing on radius?
	b2Shape* shapes[ bufsize ];
	int count = world()->b2world()->Query(aabb, shapes, bufsize);
	
	// ok, now i have 'count' shapes. Prepare list of bodies affected
	b2Vec2 center = point2vec( _center );
	for( int i = 0; i < count; i++ )
	{
		// get body
		b2Body* pBody = shapes[i]->GetBody();
		// find in which simulation step it will be reached by shockwave
		double distance = ( pBody->GetPosition() - center ).Length();
		int step = int( floor( distance / ( _speed*timestep ) ) );
		
		if ( step < steps )
		{
			// store body
			if ( ! _bodiesAffected[step].contains(pBody) )
			{
				_bodiesAffected[step].append( pBody );
			}
			// store DM
			if ( shapes[i]->GetUserData() )
			{
				DamageInfo info;
				info.damageManager = static_cast<DamageManager*>( shapes[i]->GetUserData() );
				info.position = pBody->GetPosition();
				_damageManagers[step].append( info );
			}
			
		}
	}
	// we have list of bodies now
	// TODO pray that no one of the bodiues will be removed during simulation steps!
}

// ============================================================================
/// Acts with a force on body
void Explosion::actWithForce()
{
	// act on bodies
	foreach( b2Body* pBody, _bodiesAffected[ _currentStep ] )
	{
		b2Vec2 force = forceForPoint( pBody->GetPosition() );
		pBody->ApplyForce( force, pBody->GetPosition() );
		qDebug("Acting with force %g on body %s", force.Length(),
			qPrintable( static_cast<Body*>(pBody->GetUserData())->name() )
			);
	}
	
	// act on damage managers
	foreach( DamageInfo info, _damageManagers[ _currentStep ] )
	{
		double force = forceForPoint( info.position ).Length();
		info.damageManager->contact( force );
	}
}

// ============================================================================
/// Calculates force vector for point
b2Vec2 Explosion::forceForPoint( const b2Vec2& point )
{
	b2Vec2 center = point2vec( _center );
	b2Vec2 diff = point - center;
		
	double distance = diff.Length();
	b2Vec2 normal = diff;
	normal.Normalize();
	
	double force = _energy / ( distance*distance+1);
	
	return force*normal;
}

}
