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

#include "Box2D.h"

#include "world.h"
#include "plane.h"
#include "ground.h"
#include "airfield.h"
#include "landinglight.h"
#include "damagemanager.h"


namespace Flyer {

/// Contact listener. Internal class which implements listenrr interface and 
/// propagates contact information to system.
class WorldContactListener : public b2ContactListener
{
	/// Contact callback
	virtual void Result(const b2ContactResult* pPoint )
	{
		// apply damage to directly involved shapes
		void *pData1 = pPoint->shape1->GetUserData();
		void *pData2 = pPoint->shape2->GetUserData();
		
		if ( pData1 )
		{
			static_cast<DamageManager*>( pData1 )->contact( pPoint->normalImpulse );
		}
		if ( pData2 )
		{
			static_cast<DamageManager*>( pData2 )->contact( pPoint->normalImpulse );
		}
		
		// propagate damage
		propagateDamage( pPoint->shape1->GetBody(), pPoint->normalImpulse );
		// propagate damage
		propagateDamage( pPoint->shape2->GetBody(), pPoint->normalImpulse );
	}
	
	// Propagates damage to other bodies connected with joints
	void propagateDamage( b2Body* pBody, double force )
	{
		/* TODO loops infinitely
		b2JointEdge* pJointEdge = pBody->GetJointList();
		while( pJointEdge )
		{
			b2Body* pOtherBody	= pJointEdge->other;
			double force		= pJointEdge->joint->GetReactionForce().Length();
			
			b2Shape* pShape = pOtherBody->GetShapeList();
			while( pShape )
			{
				void *pData = pShape->GetUserData();
				if ( pData )
				{
					static_cast<DamageManager*>( pData )->contact( force / 2 ); // propagate half the force
				}
				
				pShape = pShape->GetNext();
			}
			// TODO maybe propagate further here
		}
			*/
	}
};
	


// ============================================================================
// Constructor
World::World()
{
	initWorld();
}

// ============================================================================
// Destructor
World::~World()
{
}

// ============================================================================
// Renders region of the world on painter. Region is defined in world coorindates.
void World::render( QPainter& painter, const QRectF& rect )
{
	foreach( WorldObject* pObject, _objects )
	{
		QRectF br = pObject->boundingRect();
		if( br.isNull() || rect.intersects( br ) )
		{
			pObject->render( painter, rect );
		}
	}
}

// ============================================================================
// Renders mini-map of region
void World::renderMap( QPainter& painter, const QRectF& rect )
{
	foreach( WorldObject* pObject, _objects )
	{
		if( pObject->boundingRect().isNull() || rect.intersects( pObject->boundingRect() ) )
		{
			pObject->renderOnMap( painter, rect );
		}
	}
}

// ============================================================================
// Initializes world
void World::initWorld()
{
	_boundary = QRectF( -15000, -500, 30000, 3000 ); // 30x3 km box
	// create world
	b2AABB worldAABB;
	worldAABB.lowerBound.Set( _boundary.left(), _boundary.top() );
	worldAABB.upperBound.Set(_boundary.right(), _boundary.bottom() );
	
	// gravity
	b2Vec2 gravity(0.0, -9.81);

	_pb2World = new b2World(  worldAABB, gravity, true );
	
	// add contact listener to detect damage
	_pb2World->SetContactListener( new WorldContactListener() );
	
	
	// init ground
	_pGround = new Ground( this );
	_objects.append( _pGround );
	
	// init plane
	_pPlayerPlane = new Plane( this, QPointF( 0, _pGround->height(300) + 2.5 ), 0.2 );
	_objects.append( _pPlayerPlane );
	
	
	// airfields
	_objects.append( new Airfield( this, -50, 250, 300 ) );
	_objects.append( new Airfield( this, 10050, 10300, 800 ) );
	
	// landing lights
	_objects.append( new LandingLight( this, -50, 300, M_PI-0.25 ) );
	_objects.append( new LandingLight( this, 250, 300, 0.25 ) );
	
	_objects.append( new LandingLight( this, 10050, 800, M_PI-0.25 ) );
	_objects.append( new LandingLight( this, 10300, 800, 0.25 ) );
}

// ============================================================================
// Simulation step
void World::simulate( double dt )
{
	double timestep = 1.0/60.0;	// desired simulation step
	
	int iters = dt/timestep; // sub iterations here
	for( int i = 0; i < iters; i++ )
	{
		_pb2World->Step( dt/iters, 20 );
		foreach ( WorldObject* pObject, _objects )
		{
			pObject->simulate( dt/iters );
		}
	}
}

// ============================================================================
// Adds oject to the world
void World::addObject( WorldObject* pObject )
{
	_objects.append( pObject );
}

// ============================================================================
// Removes object from world
void World::removeObject( WorldObject* pObject )
{
	_objects.removeAll( pObject );
}

}
