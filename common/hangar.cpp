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
#include "body.h"
#include "bodyprovider.h"
#include "common.h"
#include "ground.h"
#include "system.h"
#include "ironbomb.h"
#include "planebumblebee.h"
#include "activeattachpoint.h"
#include "passiveattachpoint.h"
#include "common.h"

#include "hangar.h"

namespace Flyer
{

// constants

static const double MAX_VELOCITY = 0.5;	// maximum speed for machine to get repaired

// ============================================================================
// Constructor
Hangar::Hangar( World* pWorld, double location ): Machine( pWorld )
{
	setName( "Hangar" );
	setLayers( PhysLayerBackground );
	setRenderLayer( LayerBackground );

	double locationY = pWorld->ground()->height( location );

	Body* pBody = BodyProvider::loadBody( "hangar20-1.body" );
	pBody->setPosition( b2Vec2( location, locationY ) );
	pBody->create( pWorld );
	addBody( pBody, BodyRendered1 );
	setMainBody( pBody );
	
	
	_activeArea  = pBody->boundingRect().translated( QPointF( location, locationY ) );
	
	pWorld->addToTimer1( this );
}

// ============================================================================
// Destructor
Hangar::~Hangar()
{
}

// ============================================================================
// Timer handler
void Hangar::timer1()
{
	// get machines in range
	QList<Machine*> machines = world()->findMachines( _activeArea, World::ObjectPlane );

	// check for speed and make list of systems to repair
	QMultiMap< double, System* > systems;
	
	foreach( Machine* pMachine, machines )
	{
		if ( pMachine->linearVelocity().Length() <= MAX_VELOCITY )
		{
			foreach( System* pSystem, pMachine->systems() )
			{
				if ( pSystem->status() < 1.0 )
				{
					systems.insert( -pSystem->status(), pSystem );
				}
			}
		}
	}
	
	// fix most broken system
	if ( ! systems.isEmpty() )
	{
		System* pSystem = systems.values().last();
		QString message = QString( QObject::tr("%1 repaired.") ).arg( pSystem->name() );
		
		pSystem->repair();
		
		// Send message (if repair sucessfull)
		if ( pSystem->status() == 1.0 )
		{
			pSystem->parent()->addSystemMessage( message );
		}
	
		return;
	}
	
	// if I'm here, that means that there is nothing to repair. add bomb to firs bumblebee found
	foreach( Machine* pMachine, machines )
	{
		PlaneBumblebee* pPlane = dynamic_cast< PlaneBumblebee* >( pMachine );
		if ( pPlane && pPlane->weaponPoint() && pMachine->linearVelocity().Length() <= MAX_VELOCITY )
		{
			ActiveAttachPoint* pPoint = pPlane->weaponPoint();
			if ( ! pPoint->attached() )
			{
				IronBomb* pBomb = new IronBomb( world() );
				pBomb->init( QPointF( 20, 302 ), 0.0 );
				world()->addObject( pBomb, World::ObjectSimulated );
				
				// TODO DEBUG
 				pBomb->setRenderLayer( LayerForeground ); 
				
				// attache the bomb
				pPoint->attach( pBomb->attachPoint() );
				
				pPlane->addSystemMessage("Bomb attached. Go get them!");
				return;
			}
		}
	}
	
	
}


}

// EOF

