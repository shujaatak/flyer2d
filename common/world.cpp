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

#include "ground.h"
#include "renderingoptions.h"
#include "common.h"
#include "b2dqt.h"
#include "damagemanager.h"
#include "joint.h"
#include "machine.h"

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
		void *pData1 = pPoint->shape1->GetBody()->GetUserData();
		void *pData2 = pPoint->shape2->GetBody()->GetUserData();
		double force = pPoint->normalImpulse / TIMESTEP;
		
		Body* pBody1 = static_cast<Body*>( pData1 );
		Body* pBody2 = static_cast<Body*>( pData2 );
		
		DamageManager* pDM1 = pBody1 ? pBody1->damageManager() : NULL;
		DamageManager* pDM2 = pBody2 ? pBody2->damageManager() : NULL;
		 
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

/// Decoration pair callback (does nothing)
class NullPairCallback : public b2PairCallback
{
public:
	virtual ~NullPairCallback() {}

	// This should return the new pair user data. It is ok if the
	// user data is null.
	virtual void* PairAdded(void* /*proxyUserData1*/, void* /*proxyUserData2*/){ return NULL; }

	// This should free the pair's user data. In extreme circumstances, it is possible
	// this will be called with null pairUserData because the pair never existed.
	virtual void PairRemoved(void* /*proxyUserData1*/, void* /*proxyUserData2*/, void* /*pairUserData*/){}
};

/// Private data attached to each world object
class ObjectPrivateData
{
public:
	ObjectPrivateData()
	{
		lastRenderedIn = -1;
		proxyId = -1;
	}
	
	int lastRenderedIn;	///< No of step when the body was last renderd. Used ot prevent multiple rendering
	int proxyId;		///< Id of proxy in decorations broadphase
};

// ============================================================================
// Constructor
World::World( const QRectF& boundary )
{
	_steps = 0;
	_renders = 0;
	_timer1Time = 0.0;
	_decorationsDirty = false;
	
	_boundary = boundary;
	// create world
	b2AABB worldAABB = rect2aabb( _boundary );
	
	// gravity
	b2Vec2 gravity(0.0, -9.81);

	_pb2World = new b2World(  worldAABB, gravity, true );
	_pDecorationBroadPhase = new b2BroadPhase( worldAABB, new NullPairCallback() );
	
	// add contact listener to detect damage
	_pb2World->SetContactListener( new WorldContactListener() );
	
	// add destruction listener
	_pb2World->SetDestructionListener( new DestructionListener() );
	
	
	// init pointers
	_pGround		= NULL;
	_pPlayerPlane	= NULL;
	_pEnemyPlane	= NULL; // TODO get rid of this already
	
	// sky gradient
	_skyGradient.setStart( _boundary.left() + _boundary.width()/ 2, _boundary.top() );
	_skyGradient.setFinalStop( _boundary.left() + _boundary.width()/ 2, _boundary.bottom() );
	_skyGradient.setColorAt( 0.0, QColor("#99B0F4"));
	_skyGradient.setColorAt( 1.0, QColor("#0D47F4"));
	

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
	// check validity
	if ( rect.isNull() )
	{
		qWarning("World rectangle is null");
		return;
	}
	
	// render athmosphere
	renderAthmosphere( painter, rect );
	
	// bug workartound ?  first pait something with vcectors
	painter.drawLine( QPointF( 1, 1 ), QPointF( 2, 2 ) );
	RenderingOptions options;
	
	options.viewportSize = rect.size().toSize(); // NOTE this may be inaccurate
	
	// get objects to be rendered in this bounding rect (TODO: use broadphase here)
	QMultiMap<int, WorldObject* > objectsToRender;
	{
		const int MAX_SHAPES = 2048; // max shapes in visible area. wild guess
		QVector<b2Shape*> shapes( MAX_SHAPES );
		int found = _pb2World->Query( rect2aabb( rect ), shapes.data(), MAX_SHAPES );
		if ( found > MAX_SHAPES )
		{
			qDebug("More shapes than space in receiver!!!");
		}
		for( int i = 0; i < found; i++ )
		{
			b2Shape* pShape = shapes[i];
			Body* pBody = static_cast<Body*>( pShape->GetBody()->GetUserData() );
			if ( pBody )
			{
				// check if isnt already on the list
				PhysicalObject* pObject = pBody->parent();
				Q_ASSERT( pObject );
				
				ObjectPrivateData* pPrivate = static_cast<ObjectPrivateData*>( pObject->worldPrivateData );
				if ( pPrivate && pPrivate->lastRenderedIn != _renders ) // NOTE body may be alredy destroye and thus not have private data attached
				{
					pPrivate->lastRenderedIn = _renders;
					objectsToRender.insert( pObject->renderLayer(), pObject );
				}
			}
		}
	}
	
	// repeat search in decoration briad phase
	{
		const int MAX_OBJECTS = 1024; // max shapes in visible area. wild guess
		QVector<WorldObject*> ojects( MAX_OBJECTS );
		int found = _pDecorationBroadPhase->Query( rect2aabb( rect ), (void**)ojects.data(), MAX_OBJECTS );
		if ( found > MAX_OBJECTS )
		{
			qDebug("More decrotions than space in receiver!!!");
		}
		for( int i = 0; i < found; i++ )
		{
			WorldObject* pObject = ojects[i];
			
			ObjectPrivateData* pPrivate = static_cast<ObjectPrivateData*>( pObject->worldPrivateData );
			if ( pPrivate && pPrivate->lastRenderedIn != _renders ) // NOTE object may be alredy destroye and thus not have private data attached
			{
				pPrivate->lastRenderedIn = _renders;
				objectsToRender.insert( pObject->renderLayer(), pObject );
			}
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
	
	
	_renders ++;
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
// Initializes random ground
void World::initRandomGround( const QList<Ground::Section>& seed )
{
	Q_ASSERT( ! _pGround );
	_pGround = new Ground( this );
	_pGround->random( seed );
	addObject( _pGround, ObjectRenderedMap );
}

// ============================================================================
// Sets player planme
void World::setPlayerPlane( Plane* pPlane )
{
	Q_ASSERT( ! _pPlayerPlane );
	_pPlayerPlane = pPlane;
}

// ============================================================================
// Initializes world
	/*
void World::initWorld()
{
	_steps = 0;
	_renders = 0;
	_timer1Time = 0.0;
	_decorationsDirty = false;
	
	_boundary = QRectF( -15000, -500, 30000, 3000 ); // 30x3 km box
	// create world
	b2AABB worldAABB = rect2aabb( _boundary );
	
	// gravity
	b2Vec2 gravity(0.0, -9.81);

	_pb2World = new b2World(  worldAABB, gravity, true );
	_pDecorationBroadPhase = new b2BroadPhase( worldAABB, new NullPairCallback() );
	
	// add contact listener to detect damage
	_pb2World->SetContactListener( new WorldContactListener() );
	
	// add destruction listener
	_pb2World->SetDestructionListener( new DestructionListener() );
	
	
	// init ground
	_pGround = new Ground( this );
	addObject( _pGround, ObjectRendered | ObjectRenderedMap );
	{
		// init seed
		QList<Ground::Section> seed;
		Ground::Section section;
		
		// first section - mountains from -15 to -5 km
		section.x = -15000;
		section.y = 2500;
		section.maxSlope = 1.5;
		section.canBeDividedRight = true;
		section.minSectionSize = 50;	// dense terrain
		section.maxSectionSize = 200;
		section.maxHeight = 2400;
		section.minHeight = 400;
		seed.append( section );
		
		// second section - flat land fro -5 km to -50m
		section.x = -5000;
		section.y = 500;
		section.maxSlope = 0.1;
		section.canBeDividedRight = true;
		section.minSectionSize = 100;	// smooth terrain
		section.maxSectionSize = 400;
		section.maxHeight = 700;
		section.minHeight = 250;
		seed.append( section );
		
		// third section - airfield
		section.x = -100;
		section.y = 300;
		section.canBeDividedRight = false;
		seed.append( section );
		
		// fourth section - smooth flats gently going up to 10km
		section.x = 300; // this gives 400m for runway
		section.y = 300; // flat runway
		section.maxSlope = 0.1;
		section.canBeDividedRight = true;
		section.minSectionSize = 100;
		section.maxSectionSize = 400;
		section.maxHeight = 850;
		section.minHeight = 250;
		seed.append( section );
		
		// fifth section - next airfield
		section.x = 5000; // at 10th km // TODO temporarly reduced
		section.y = 300;
		section.canBeDividedRight = false;
		seed.append( section );
		
		// between airport and mountains
		section.x = 5350;
		section.y = 300;
		section.maxSlope = 0.2;
		section.canBeDividedRight = true;
		section.minSectionSize = 170;
		section.maxSectionSize = 500;
		section.maxHeight = 800;
		section.minHeight = 300;
		seed.append( section );
		
		// sixth section - mountins to the end
		section.x = 10000;
		section.y = 700;
		section.maxSlope = 1.5;
		section.canBeDividedRight = true;
		section.minSectionSize = 70;
		section.maxSectionSize = 200;
		section.maxHeight = 2400;
		section.minHeight = 700;
		seed.append( section );
		
		
		// very last section
		section.x = 15000;
		section.y = 2500;
		section.canBeDividedRight = false;
		seed.append( section );
		
		_pGround->random( seed );
	}
	
	// init plane
	_pPlayerPlane = new PlaneBumblebee( this, QPointF( 0, _pGround->height(0) + 2.5 ), 0.2 );
	addObject( _pPlayerPlane, ObjectRendered | ObjectSide1 | ObjectSimulated | ObjectPlane | ObjectRenderedMap );
	
	// add sample bomb floating near the plane
	IronBomb* pBomb = new IronBomb( this );
	pBomb->init( QPointF( 20, 302 ), 0.0 );
	addObject( pBomb, ObjectRendered | ObjectSimulated );
	
	// attache the bomb
	_pPlayerPlane->activeAttachPoints().first()->attach( pBomb->attachPoint() );
	
	// enemy plane (!)
	_pEnemyPlane = NULL;
	PlaneBumblebee* pEnemy = new PlaneBumblebee( this, QPointF( -200, 400 ), 0.0 );
	pEnemy->mainBody()->b2body()->SetLinearVelocity( b2Vec2( 30, 0 ) ); // some initial speed
	pEnemy->setAutopilot( true ); // turn on autopilot
	addObject( pEnemy, ObjectRendered | ObjectSide2 | ObjectSimulated | ObjectPlane | ObjectRenderedMap );
	_pEnemyPlane = pEnemy; // debug variable
	
	// airfields
	addObject( new Airfield( this, -50, 250 ), ObjectRendered | ObjectAirfield | ObjectSide1 | ObjectRenderedMap  );
	addObject( new Airfield( this, 5050, 5300 ), ObjectRendered | ObjectAirfield | ObjectSide1 | ObjectRenderedMap  );
	
	// landing lights
	addObject( new LandingLight( this, -50, M_PI-0.25 ),  ObjectRendered | ObjectSimulated );
	addObject( new LandingLight( this, 250, 0.25 ),  ObjectRendered| ObjectSimulated  );
	
	addObject( new LandingLight( this, 5050, M_PI-0.25 ),  ObjectRendered | ObjectSimulated  );
	addObject( new LandingLight( this, 5300, 0.25 ),  ObjectRendered | ObjectSimulated  );
	
	// AA batteries
	addObject( new AntiAirBattery( this, 4750, 2.4 ), ObjectInstallation | ObjectSimulated | ObjectRendered | ObjectSide2 | ObjectRenderedMap   );
	addObject( new AntiAirBattery( this, 4800, 2.4 ), ObjectInstallation | ObjectSimulated | ObjectRendered | ObjectSide2 | ObjectRenderedMap  );
	addObject( new AntiAirBattery( this, 5500, 2.4 ), ObjectInstallation | ObjectSimulated | ObjectRendered | ObjectSide2 | ObjectRenderedMap  );
	addObject( new AntiAirBattery( this, -1500, 1.2 ), ObjectInstallation | ObjectSimulated | ObjectRendered| ObjectSide2 | ObjectRenderedMap   );
	
	// create towns
	createTown( 400, 800, true ); // TODO teporarly disanled to torack down texture problems
	createTown( 1300, 200, true );
	
	// hangar on runway
	addObject( new Hangar( this, 40 ), ObjectInstallation | ObjectRendered | ObjectSide1 );
	
}
	*/

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
	
	// commit changes to decroation broadphase
	if ( _decorationsDirty )
	{
		_pDecorationBroadPhase->Commit();
		_decorationsDirty = false;
	}
	
	_steps += dt/TIMESTEP;
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
	
	// attach private data
	if ( ! pObject->worldPrivateData )
	{
		pObject->worldPrivateData = new ObjectPrivateData();
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
	
	// remove private data
	ObjectPrivateData* pPrivate = static_cast<ObjectPrivateData*>( pObject->worldPrivateData );
	if ( pPrivate )
	{
		if ( pPrivate->proxyId >= 0 )
		{
			removeDecoration( pObject );
		}
		delete pPrivate;
		pObject->worldPrivateData = NULL;
	}
}

// ============================================================================
// Returns timestep
double World::timestep() const
{
	return TIMESTEP;
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

// ============================================================================
/// Adds object to decartion list. Objects ounding rect will be used to lacate, and it will be
/// rendered when approrpiate.
/// Object should be already add to the world using addObject().
void World::addDecoration( WorldObject* pObject )
{
	Q_ASSERT( pObject );
	
	// create p[rovate data, if not present
	if ( ! pObject->worldPrivateData )
	{
		pObject->worldPrivateData = new ObjectPrivateData();
	}
	
	
	ObjectPrivateData* pPrivate = static_cast<ObjectPrivateData*>( pObject->worldPrivateData );
	
	
	if ( pPrivate->proxyId < 0 )
	{
		QRectF br = pObject->boundingRect();
		b2AABB aabb = rect2aabb( br );
		if ( aabb.IsValid() && ! br.isNull() )
		{
			pPrivate->proxyId = int
				( _pDecorationBroadPhase->CreateProxy( rect2aabb( br ), pObject )
				);
		}
		else
		{
			qWarning("Invalid decoration boundary");
		}
	}
	else
	{
		qWarning("Attempt to add object once again to decoration broadphase");
	}
}

// ============================================================================
/// Sneds ifnormation to decorartion broadphase that object has moved.
void World::decorationMoved( WorldObject* pObject )
{
	Q_ASSERT( pObject );
	ObjectPrivateData* pPrivate = static_cast<ObjectPrivateData*>( pObject->worldPrivateData );
	Q_ASSERT( pPrivate );
	
	if ( pPrivate->proxyId >= 0 )
	{
		uint16 id = uint16( pPrivate->proxyId );
		_pDecorationBroadPhase->MoveProxy( id, rect2aabb( pObject->boundingRect() ) );
		_decorationsDirty = true;
		
	}
	else
	{
		qWarning("Attempt to move decoration which is not in broadphase");
	}
}

// ============================================================================
/// Removes object from decortion boradphase. This doesn't destroy objetct, nor removes it from the world.
void World::removeDecoration( WorldObject* pObject )
{
	Q_ASSERT( pObject );
	ObjectPrivateData* pPrivate = static_cast<ObjectPrivateData*>( pObject->worldPrivateData );
	Q_ASSERT( pPrivate );
	
	if ( pPrivate->proxyId >= 0 )
	{
		uint16 id = uint16( pPrivate->proxyId );
		_pDecorationBroadPhase->DestroyProxy( id );
		pPrivate->proxyId = -1;
	}
	else
	{
		qWarning("Attempt to destroy decoration which is not in broadphase");
	}
}

}
