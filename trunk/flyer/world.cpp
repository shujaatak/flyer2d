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
#include "planebumblebee.h"
#include "ground.h"
#include "airfield.h"
#include "landinglight.h"
#include "damagemanager.h"
#include "antiairbattery.h"
#include "explosion.h"


namespace Flyer {

// Costants

static const double TIMESTEP = 1.0/60.0; 	// [s]




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
		double force = pPoint->normalImpulse / TIMESTEP;
		
		DamageManager* pDM1 = static_cast<DamageManager*>( pData1 );
		DamageManager* pDM2 = static_cast<DamageManager*>( pData2 );
		 
		double multiplier1 = 1.0;
		double multiplier2 = 1.0;
		
		if ( pDM1 ) multiplier1 = pDM1->damageMultiplier();
		if ( pDM2 ) multiplier2 = pDM2->damageMultiplier();
		
		// DEBUG
		//if ( multiplier1 * multiplier2 > 1 && pDM2 && pDM1  ) qDebug("bullet contact, force: %g", force );
		
		if ( pDM1 )
		{
			pDM1->contact( multiplier2 * force );
		}
		if ( pDM2 )
		{
			pDM2->contact(multiplier1 * force );
		}
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
	// render athmosphere
	renderAthmosphere( painter, rect );
	
	// render objectts
	QList<int> layers;
	layers << ObjectRenderedSky << ObjectRenderedBackground << ObjectRenderedBuildings
		<< ObjectRenderedVehicles << ObjectRenderedForeground;
	
	foreach ( int layer, layers )
	{
		foreach( WorldObject* pObject, _objects[layer] )
		{
			QRectF br = pObject->boundingRect();
			if( br.isNull() || rect.intersects( br ) )
			{
				pObject->render( painter, rect );
			}
		}
	}
}

// ============================================================================
/// Renders world's athmosphere
void World::renderAthmosphere( QPainter& painter, const QRectF& )
{
	painter.fillRect( _boundary, _skyGradient );
}

// ============================================================================
// Renders mini-map of region
void World::renderMap( QPainter& painter, const QRectF& rect )
{
	foreach( WorldObject* pObject, _objects[ObjectRenderedMap] )
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
	addObject( _pGround, ObjectRenderedForeground | ObjectRenderedMap );
	
	// init plane
	_pPlayerPlane = new PlaneBumblebee( this, QPointF( 0, _pGround->height(300) + 2.5 ), 0.2 );
	addObject( _pPlayerPlane, ObjectRenderedVehicles | ObjectSide1 | ObjectSimulated | ObjectPlane | ObjectRenderedMap );
	
	
	// airfields
	addObject( new Airfield( this, -50, 250, 300 ), ObjectRenderedForeground | ObjectAirfield | ObjectSide1 | ObjectRenderedMap  );
	addObject( new Airfield( this, 10050, 10300, 800 ), ObjectRenderedForeground | ObjectAirfield | ObjectSide1 | ObjectRenderedMap  );
	
	// landing lights
	addObject( new LandingLight( this, -50, 300, M_PI-0.25 ),  ObjectRenderedBuildings | ObjectSimulated );
	addObject( new LandingLight( this, 250, 300, 0.25 ),  ObjectRenderedBuildings | ObjectSimulated  );
	
	addObject( new LandingLight( this, 10050, 800, M_PI-0.25 ),  ObjectRenderedBuildings | ObjectSimulated  );
	addObject( new LandingLight( this, 10300, 800, 0.25 ),  ObjectRenderedBuildings | ObjectSimulated  );
	
	// AA batteries
	addObject( new AntiAirBattery( this, 1000, 2.4 ), ObjectInstallation | ObjectSimulated | ObjectRenderedBuildings | ObjectSide2 | ObjectRenderedMap   );
	addObject( new AntiAirBattery( this, 1050, 2.4 ), ObjectInstallation | ObjectSimulated | ObjectRenderedBuildings | ObjectSide2 | ObjectRenderedMap  );
	addObject( new AntiAirBattery( this, 1100, 2.4 ), ObjectInstallation | ObjectSimulated | ObjectRenderedBuildings | ObjectSide2 | ObjectRenderedMap  );
	addObject( new AntiAirBattery( this, -2000, 1.2 ), ObjectInstallation | ObjectSimulated | ObjectRenderedBuildings | ObjectSide2 | ObjectRenderedMap   );
	
	// sky gradient
	_skyGradient.setStart( _boundary.left() + _boundary.width()/ 2, _boundary.top() );
	_skyGradient.setFinalStop( _boundary.left() + _boundary.width()/ 2, _boundary.bottom() );
	_skyGradient.setColorAt( 0.0, QColor("#99B0F4"));
	_skyGradient.setColorAt( 1.0, QColor("#0D47F4"));
}

// ============================================================================
// Simulation step
void World::simulate( double dt )
{
	// destroy objects from queue
	while( ! _objectsToDestroy.isEmpty() )
	{
		delete _objectsToDestroy.takeFirst();
	}
	
	int iters = dt/TIMESTEP; // sub iterations here
	for( int i = 0; i < iters; i++ )
	{
		_pb2World->Step( dt/iters, 20 );
		foreach ( WorldObject* pObject, _objects[ ObjectSimulated] )
		{
			pObject->simulate( dt/iters );
		}
	}
}

// ============================================================================
// Adds oject to the world
void World::addObject( WorldObject* pObject, int objectClass )
{
	_allObjects.append( pObject );
	
	// add to specific lists
	for( int i =0 ;i < 32; i++ )
	{
		int bit = 1 << i;
		if ( objectClass & bit )
		{
			_objects[bit].append( pObject );
		}
	}
}

// ============================================================================
// Removes object from world
void World::removeObject( WorldObject* pObject, bool destroy )
{
	_allObjects.removeAll( pObject );
	
	// remove from specific lists
	for( int i =0 ;i < 32; i++ )
	{
		int bit = 1 << i;
		_objects[bit].removeAll( pObject );
	}
	
	// add to destruction queue, if desired
	if ( destroy )
	{
		_objectsToDestroy.append( pObject );
	}
}

// ============================================================================
// Returns timestep
double World::timestep() const
{
	return TIMESTEP;
}

}
