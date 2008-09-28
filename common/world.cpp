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

#include "planebumblebee.h"
#include "ground.h"
#include "airfield.h"
#include "landinglight.h"
#include "damagemanager.h"
#include "antiairbattery.h"
#include "explosion.h"
#include "joint.h"
#include "building.h"
#include "renderingoptions.h"
#include "ironbomb.h"
#include "activeattachpoint.h"
#include "common.h"
#include "hangar.h"

#include "world.h"


namespace Flyer {

// Costants

static const double	TIMESTEP = 1.0/60.0; 	// [s]
static const int	ITERATIONS = 10;		// solver iterations




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
	
/// Destruction listener class.
class DestructionListener: public b2DestructionListener
{
	/// Handles physical joint destruction
	virtual void SayGoodbye( b2Joint* pb2Joint )
	{
		Joint* pJoint = static_cast<Joint*>( pb2Joint->GetUserData() );
		if ( pJoint )
		{
			pJoint->jointDestroyed();
		}
	}
	
	/// Handles shape destruction. Unused.
	virtual void SayGoodbye(b2Shape* ){}
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
	
	// bug workartound ?  first pait something with vcectors
	painter.drawLine( QPointF( 1, 1 ), QPointF( 2, 2 ) );
	RenderingOptions options;
	
	// get objects to be rendered in this bounding rect (TODO: use broadphase here)
	QMultiMap<int, WorldObject* > objectsToRender;
	// render
	foreach( WorldObject* pObject, _objects[ObjectRendered] )
	{
		QRectF br = pObject->boundingRect();
		if( br.isNull() || rect.intersects( br ) )
		{
			objectsToRender.insert( pObject->renderLayer(), pObject );
		}
	}
	// TODO debug
	//qDebug("Rendering %d of %d renderable objects"
	//	, objectsToRender.size(), _objects[ObjectRendered].size() );
	
	
	// render objects
	foreach( WorldObject* pObject, objectsToRender.values() )
	{
		// TODO looks ugly
		int layer = pObject->renderLayer();
		if ( layer == LayerBackground )
		{
			options.textureStyle = Texture::Background;
		}
		else
		{
			options.textureStyle = Texture::Normal;
		}
		
		pObject->render( painter, rect, options );
		//qDebug("Rendering item %s on layer %d", qPrintable( pObject->name() ), pObject->renderLayer() );
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
	_steps = 0;
	_timer1Time = 0.0;
	
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
	
	// add destruction listener
	_pb2World->SetDestructionListener( new DestructionListener() );
	
	
	// init ground
	_pGround = new Ground( this );
	addObject( _pGround, ObjectRendered | ObjectRenderedMap );
	
	// init plane
	_pPlayerPlane = new PlaneBumblebee( this, QPointF( 0, _pGround->height(300) + 2.5 ), 0.2 );
	addObject( _pPlayerPlane, ObjectRendered | ObjectSide1 | ObjectSimulated | ObjectPlane | ObjectRenderedMap );
	
	// add sample bomb floating near the plane
	IronBomb* pBomb = new IronBomb( this );
	pBomb->init( QPointF( 20, 302 ), 0.0 );
	addObject( pBomb, ObjectRendered | ObjectSimulated );
	
	// attache the bomb
	_pPlayerPlane->activeAttachPoints().first()->attach( pBomb->attachPoint() );
	
	// enemy plane (!)
	PlaneBumblebee* pEnemy = new PlaneBumblebee( this, QPointF( -200, 400 ), 0.0 );
	pEnemy->mainBody()->b2body()->SetLinearVelocity( b2Vec2( 30, 0 ) ); // some initial speed
	pEnemy->setAutopilot( true ); // turn on autopilot
	addObject( pEnemy, ObjectRendered | ObjectSide2 | ObjectSimulated | ObjectPlane | ObjectRenderedMap );
	_pEnemyPlane = pEnemy; // debug variable
	
	
	// airfields
	addObject( new Airfield( this, -50, 250, 300 ), ObjectRendered | ObjectAirfield | ObjectSide1 | ObjectRenderedMap  );
	addObject( new Airfield( this, 10050, 10300, 800 ), ObjectRendered | ObjectAirfield | ObjectSide1 | ObjectRenderedMap  );
	
	// landing lights
	addObject( new LandingLight( this, -50, 300, M_PI-0.25 ),  ObjectRendered | ObjectSimulated );
	addObject( new LandingLight( this, 250, 300, 0.25 ),  ObjectRendered| ObjectSimulated  );
	
	addObject( new LandingLight( this, 10050, 800, M_PI-0.25 ),  ObjectRendered | ObjectSimulated  );
	addObject( new LandingLight( this, 10300, 800, 0.25 ),  ObjectRendered | ObjectSimulated  );
	
	// AA batteries
	addObject( new AntiAirBattery( this, 3000, 2.4 ), ObjectInstallation | ObjectSimulated | ObjectRendered | ObjectSide2 | ObjectRenderedMap   );
	addObject( new AntiAirBattery( this, 3050, 2.4 ), ObjectInstallation | ObjectSimulated | ObjectRendered | ObjectSide2 | ObjectRenderedMap  );
	addObject( new AntiAirBattery( this, 3100, 2.4 ), ObjectInstallation | ObjectSimulated | ObjectRendered | ObjectSide2 | ObjectRenderedMap  );
	addObject( new AntiAirBattery( this, -1500, 1.2 ), ObjectInstallation | ObjectSimulated | ObjectRendered| ObjectSide2 | ObjectRenderedMap   );
	
	// sky gradient
	_skyGradient.setStart( _boundary.left() + _boundary.width()/ 2, _boundary.top() );
	_skyGradient.setFinalStop( _boundary.left() + _boundary.width()/ 2, _boundary.bottom() );
	_skyGradient.setColorAt( 0.0, QColor("#99B0F4"));
	_skyGradient.setColorAt( 1.0, QColor("#0D47F4"));
	
	// create towns
	createTown( 400, 800, true ); // TODO teporarly disanled to torack down texture problems
	createTown( 1300, 200, true );
	
	// hangar on runway
	addObject( new Hangar( this, 40 ), ObjectInstallation | ObjectRendered | ObjectSide1 );
	
	
}

// ============================================================================
// Simulation step
void World::simulate( double dt )
{
	// destroy objects from queue
	/*
	while( ! _objectsToDestroy.isEmpty() )
	{
		delete _objectsToDestroy.takeFirst();
	}
	*/
	// TODO new implementation
	qDeleteAll( _objectsToDestroy );
	_objectsToDestroy.clear();
	
	int iters = dt/TIMESTEP; // sub iterations here
	for( int i = 0; i < iters; i++ )
	{
		_pb2World->Step( dt/iters, ITERATIONS );
		foreach ( WorldObject* pObject, _objects[ ObjectSimulated] )
		{
			pObject->simulate( dt/iters );
		}
	}
	// call 1-second timer, if it is it's time
	if ( _timer1Time >= 1.0 )
	{
		_timer1Time -= 1.0;
		foreach( WorldObject* pObject, _timer1Objects )
		{
			pObject->timer1();
		}
	}
	_timer1Time += dt;
	
	
	
	_steps ++;
}

// ============================================================================
/// Returns current simulation time - seconds since simulato started
double World::time()
{
	return _steps * TIMESTEP;
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
	_timer1Objects.removeAll( pObject );
	
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

// ============================================================================
/// Creates town at specified locations
void World::createTown( double start, double end, bool /*small*/ )
{
	// foreground
	double x = start;
	while( x < end )
	{
		Building* pBuilding = Building::createSmallBuilding( this, x, false );
		double spacing = 2.0 + ((qrand()%400)/100.0);
		
		x += spacing * pBuilding->width();
	}
	
	// background
	x = start + 20.0 * ((qrand()%100)/100.0);
	while( x < end )
	{
		Building* pBuilding = Building::createSmallBuilding( this, x, true );
		double spacing = 2.0 + ((qrand()%400)/100.0);
		
		x += spacing * pBuilding->width();
	}
}

// ============================================================================
/// Finds machines which have their positions in specified area.
/// \b types is bitmask of object types which should be searched
QList<Machine*> World::findMachines( const QRectF& area, int types ) const
{
	// TODO use box2d AABB and back reference from main body
	
	// seek through all categories
	QList<Machine*> result;
	for( int i =0 ;i < 32; i++ )
	{
		int bit = 1 << i;
		
		if ( bit & types )
		{
			foreach( WorldObject* pObject, _objects[bit] )
			{
				Machine* pMachine = dynamic_cast<Machine*>( pObject );
				if	( pMachine 
					&& area.contains( pMachine->position() ) 
					&& ! result.contains( pMachine ) 
					)
				{
					result.append( pMachine );
				}
			}
		}
	}
	
	return result;
	
}

}
