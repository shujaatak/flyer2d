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

#include "world.h"
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
#include "b2dqt.h"

#include "game.h"

namespace Flyer
{

// ============================================================================
// Constructor
Game::Game( QObject *parent )
		: QObject( parent )
{
	_pWorld = NULL;
}

// ============================================================================
// Destructor
Game::~Game()
{
}

// ============================================================================
/// Sets game world.
void Game::setWorld( World* pWorld )
{
	_pWorld = pWorld;
}

// ============================================================================
/// Restarts game
void Game::restart()
{
	_messages.clear();
}

// ============================================================================
/// Creates predefined game.
Game* Game::createGame()
{
	class StdGame : public Game
	{
		public:
		
		// creates world
		void initWorld()
		{
			// standard predefined world
			QRectF boundary( -15000, -500, 30000, 3000 ); // 30x3 km
			World* pWorld = new World( boundary );
			
			// init ground
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
				pWorld->initRandomGround( seed );
			}
			const Ground* pGround = pWorld->ground();
			
			// other objects
			// init plane
			Plane* pPlane = new PlaneBumblebee( pWorld, QPointF( 0, pGround->height(0) + 2.5 ), 0.2 );
			pWorld->addObject( pPlane,  World::ObjectSide1 | World::ObjectSimulated | World::ObjectPlane | World::ObjectRenderedMap );
			pWorld->setPlayerPlane( pPlane );
			
			// enemy plane (!)
			PlaneBumblebee* pEnemy = new PlaneBumblebee( pWorld, QPointF( -200, 400 ), 0.0 );
			pEnemy->mainBody()->b2body()->SetLinearVelocity( b2Vec2( 30, 0 ) ); // some initial speed
			pEnemy->setAutopilot( true ); // turn on autopilot
			pWorld->addObject( pEnemy, World::ObjectSide2 | World::ObjectSimulated | World::ObjectPlane | World::ObjectRenderedMap );
			pWorld->_pEnemyPlane = pEnemy; // TODO get rid of this variable already!
			
			// airfields
			pWorld->addObject( new Airfield( pWorld, -50, 250 ), World::ObjectAirfield | World::ObjectSide1 | World::ObjectRenderedMap  );
			pWorld->addObject( new Airfield( pWorld, 5050, 5300 ), World::ObjectAirfield | World::ObjectSide1 | World::ObjectRenderedMap  );
			
			// landing lights
			pWorld->addObject( new LandingLight( pWorld, -50, M_PI-0.25 ),  World::ObjectSimulated );
			pWorld->addObject( new LandingLight( pWorld, 250, 0.25 ), World::ObjectSimulated  );
			
			pWorld->addObject( new LandingLight( pWorld, 5050, M_PI-0.25 ),  World::ObjectSimulated  );
			pWorld->addObject( new LandingLight( pWorld, 5300, 0.25 ),  World::ObjectSimulated  );
			
			// AA batteries
			pWorld->addObject( new AntiAirBattery( pWorld, 4750, 2.4 ), World::ObjectInstallation | World::ObjectSimulated |  World::ObjectSide2 | World::ObjectRenderedMap   );
			pWorld->addObject( new AntiAirBattery( pWorld, 4800, 2.4 ), World::ObjectInstallation | World::ObjectSimulated |  World::ObjectSide2 | World::ObjectRenderedMap  );
			pWorld->addObject( new AntiAirBattery( pWorld, 5500, 2.4 ), World::ObjectInstallation | World::ObjectSimulated |  World::ObjectSide2 | World::ObjectRenderedMap  );
			pWorld->addObject( new AntiAirBattery( pWorld, -1500, 1.2 ), World::ObjectInstallation | World::ObjectSimulated |  World::ObjectSide2 | World::ObjectRenderedMap   );
			
			// create towns
			createTown( pWorld, 400, 800, true );
			createTown( pWorld, 2300, 2600, true );
			
			// hangar on runway
			pWorld->addObject( new Hangar( pWorld, 40 ), World::ObjectInstallation | World::ObjectSide1 );
			setWorld( pWorld );
			addMessage( tr("Taxi to hangar to get a bomb."), 2.0 ); // show it after 2 seconds
		}
		
		// re-creates world
		virtual void restart()
		{
			Game::restart();
			delete _pWorld;
			initWorld();
		}
	};

	
	
	// finally - create game
	StdGame* pGame = new StdGame();
	pGame->initWorld();
	return pGame;
}

// ============================================================================
/// Creates town at specified locations
void Game::createTown( World* pWorld, double start, double end, bool /*small*/ )
{
	// foreground
	double x = start;
	while( x < end )
	{
		Building* pBuilding = Building::createSmallBuilding( pWorld, x, false );
		double spacing = 2.0 + ((qrand()%400)/100.0);
		
		x += spacing * pBuilding->width();
	}
	
	// background
	x = start + 20.0 * ((qrand()%100)/100.0);
	while( x < end )
	{
		Building* pBuilding = Building::createSmallBuilding( pWorld, x, true );
		double spacing = 2.0 + ((qrand()%400)/100.0);
		
		x += spacing * pBuilding->width();
	}
}

// ============================================================================
/// Adds message. The message won't be displayed until minTime.
void Game::addMessage( const QString& text, double minTime )
{
	Q_ASSERT( _pWorld );
	double time = qMax( _pWorld->time(), minTime );
	_messages.append( Message( text, time, Message::Game ) );
}

}
