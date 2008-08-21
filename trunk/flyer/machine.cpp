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

namespace Flyer
{

// ============================================================================
// Constructor
Machine::Machine ( World* pWorld ) : WorldObject ( pWorld )
{
}

// ============================================================================
// Destructor
Machine::~Machine()
{
}

// ============================================================================
// Renders machine
void Machine::render ( QPainter& painter, const QRectF& rect )
{
	// draw bodies 
	
	foreach( Body* pBody, _bodies[ BodyRendered3 ] )
	{
		pBody->render( painter );
	}
	foreach( Body* pBody, _bodies[ BodyRendered2 ] )
	{
		pBody->render( painter );
	}
	foreach( Body* pBody, _bodies[ BodyRendered1 ] )
	{
		pBody->render( painter );
	}
	
	// draw systems 
	
	foreach( System* pSystem, _systems[ SystemRendered3 ] )
	{
		pSystem->render( painter, rect );
	}
	foreach( System* pSystem, _systems[ SystemRendered2 ] )
	{
		pSystem->render( painter, rect );
	}
	foreach( System* pSystem, _systems[ SystemRendered1 ] )
	{
		pSystem->render( painter, rect );
	}
	
}

// ============================================================================
// Simulates machine
void Machine::simulate ( double dt )
{
	// simul;ate sustrem (only this should be left here)
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
// Adds body to allappropriate lists
void Machine::addBody( Body* pBody, int types )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		int bit = 1<<i;
		if ( types & bit )
		{
			_bodies[ bit ].append( pBody );
		}
	}
	
	_allBodies.append( pBody );
}

// ============================================================================
// Removes body from all lists
void Machine::removeBody( Body* pBody )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		_bodies[ 1<<i ].removeAll( pBody );
	}
	
	_allBodies.removeAll( pBody );
}

// ============================================================================
// Adds joint to list
void Machine::addJoint( Joint* pJoint )
{
	_allJoints.append( pJoint );
}

// ============================================================================
// Removes joint from list
void Machine::removeJoint( Joint* pJoint )
{
	_allJoints.removeAll( pJoint );
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

}
