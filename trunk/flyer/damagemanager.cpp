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

#include "damagemanager.h"
#include "system.h"

namespace Flyer
{

// ============================================================================
// Constructor
DamageManager::DamageManager( double tolerance ) : _tolerance( tolerance )
{
}

// ============================================================================
// Destructor
DamageManager::~DamageManager()
{
}

// ============================================================================
// Contact callback
void DamageManager::contact( double force )
{
	qDebug("contact force: %g", force );
	if ( force > _tolerance && _systems.size() > 0 )
	{
		double damage = force - _tolerance;
		qDebug("damage: %g", damage );
		// propagate damage to systems 
		for ( int i = 0; i < _systems.size(); i++ )
		{
			int systemIndex = qrand() % _systems.size();
			
			System* pSystem = _systems[systemIndex];
			
			if ( pSystem )
			{
				pSystem->damage( damage/_systems.size() );
			}
		}
	}
}

// ============================================================================
/// Adds system to damage manager. Propability ogf getting damage is proportional to
/// system weight. You can add NULL system with non-ero weight to tune propability distribution.
void DamageManager::addSystem( System* pSystem, int weight )
{
	for ( int i = 0; i < weight; i++ )
	{
		_systems.append( pSystem );
	}
}

}