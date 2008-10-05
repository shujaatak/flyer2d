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

#include "machine.h"
#include "system.h"
#include "body.h"
#include "joint.h"
#include "damagemanager.h"
#include "b2dqt.h"
#include "shrapnel.h"
#include "world.h"
#include "activeattachpoint.h"
#include "passiveattachpoint.h"

namespace Flyer
{

// ============================================================================
// Constructor
Machine::Machine ( World* pWorld ) : PhysicalObject ( pWorld )
{
}

// ============================================================================
// Destructor
Machine::~Machine()
{
	// delete all
	foreach( DamageManager* pDM, _allDamageManagers )
	{
		//qDebug("deleting dm: %p", pDM );
		delete pDM;
	}
	
	foreach( System* pSystem, _allSystems )
	{
		//qDebug("deleting system: %p (%s)", pSystem, qPrintable( pSystem->name() ) );
		delete pSystem;
	}
	foreach( ActiveAttachPoint* pPoint, _activeAttachPoints )
	{
		delete pPoint;
	}
	
	foreach( PassiveAttachPoint* pPoint, _passiveAttachPoints )
	{
		delete pPoint;
	}
}

// ============================================================================
// Renders machine
void Machine::render( QPainter& painter, const QRectF& rect, const RenderingOptions& options )
{
	PhysicalObject::render( painter, rect, options );
	
	// draw systems 
	
	foreach( System* pSystem, _systems[ SystemRendered3 ] )
	{
		pSystem->render( painter, rect, options );
	}
	foreach( System* pSystem, _systems[ SystemRendered2 ] )
	{
		pSystem->render( painter, rect, options );
	}
	foreach( System* pSystem, _systems[ SystemRendered1 ] )
	{
		pSystem->render( painter, rect, options );
	}
	
}

// ============================================================================
// Simulates machine
void Machine::simulate ( double dt )
{
	PhysicalObject::simulate( dt );
	
	// simulate systems
	foreach ( System* pSystem, _systems[ SystemSimulated ] )
	{
		pSystem->simulate( dt );
	}
}

// ============================================================================
//. Adds system to machine. types is  bitmask with system types.
void Machine::addSystem( System* pSystem, int types )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		int bit = 1<<i;
		if ( types & bit )
		{
			_systems[ bit ].append( pSystem );
		}
	}
	
	_allSystems.append( pSystem );
}

// ============================================================================
// Removes system from all lists
void Machine::removeSystem( System* pSystem )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		_systems[ 1<<i ].removeAll( pSystem );
	}
	
	_allSystems.removeAll( pSystem );
}

// ============================================================================
// Adds damage manager
void Machine::addDamageManager( DamageManager* pManager )
{
	_allDamageManagers.append( pManager );
}

// ============================================================================
// Removes damage mamnager
void Machine::removeDamageManager( DamageManager* pManager )
{
	_allDamageManagers.removeAll( pManager );
}

// ============================================================================
/// Flips machine aronud axis defined by p1 and p2.
/// Each flip changes the _orientation sign.
void Machine::flip( const QPointF& p1, const QPointF& p2 )
{
	PhysicalObject::flip( p1, p2 );
	
	// flip attached machines
	foreach( ActiveAttachPoint* pPoint, _activeAttachPoints )
	{
		pPoint->flip( p1, p2 );
	}
}

// ============================================================================
/// Adds attach point
void Machine::addActiveAttachPoint( ActiveAttachPoint* pPoint )
{
	if ( ! pPoint->parent() )
	{
		pPoint->setParent( this );
	}
	_activeAttachPoints.append( pPoint );
}

// ============================================================================
/// Adds attach point
void Machine::addPassiveAttachPoint( PassiveAttachPoint* pPoint )
{
	if ( ! pPoint->parent() )
	{
		pPoint->setParent( this );
	}
	_passiveAttachPoints.append( pPoint );
}

// ============================================================================
/// Ads system message to message queue.
void Machine::addSystemMessage( const QString& text )
{
	_messages.append( Message( text, world()->time() ) );
}


}
