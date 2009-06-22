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

#include "system.h"
#include "machine.h"
#include "pilot.h"

#include "damagemanager.h"

namespace Flyer
{

// ============================================================================
// Constructor
DamageManager::DamageManager()
{
}

// ============================================================================
// Destructor
DamageManager::~DamageManager()
{
}

// ============================================================================
// Contact callback
void DamageManager::damage( double force )
{
	if ( _systems.size() > 0 )
	{
		// how many times repeat damage spreading.
		// This is used to inflict smaller damage on multiple system instead of one big damage on single system
		// i case of single big contacs, like in explosion
		int repeats = 5; // some sane value
		
		//qDebug("damage: %g, force: %g", damage, force );
		// propagate damage to systems 
		for ( int i = 0; i < ( _systems.size() * repeats ); i++ )
		{
			int systemIndex = qrand() % _systems.size();
			
			System* pSystem = _systems[systemIndex];
			
			if ( pSystem )
			{
				double before = pSystem->status();
				pSystem->damage( force/(_systems.size()*repeats) );
				double after = pSystem->status();
				
				// emit messages
				if ( before > 0.5 && after < 0.5 && after > 0.0 )
				{
					QString message;
					if ( dynamic_cast<Pilot*>( pSystem ) )
					{
						message = QString(QObject::tr("%1 severly wounded")).arg( pSystem->name() );
					}
					else
					{
						message = QString(QObject::tr("%1 severly damaged")).arg( pSystem->name() );
					}
					pSystem->parent()->addSystemMessage( message );
				}
				else if ( before > 0.0 && after <= 0.0 )
				{
					QString message;
					if ( dynamic_cast<Pilot*>( pSystem ) )
					{
						message = QString(QObject::tr("%1 killed")).arg( pSystem->name() );
					}
					else
					{
						message = QString(QObject::tr("%1 destroyed")).arg( pSystem->name() );
					}
					pSystem->parent()->addSystemMessage( message );
				}
			}
		}
	}
}

// ============================================================================
/// Adds system to damage manager. Propability of getting damage is proportional to
/// system weight. You can add NULL system with non-zero weight to tune propability distribution.
/// If \b critical is set, system is destroyed when body containing the manages is destroyed.
void DamageManager::addSystem( System* pSystem, int weight, bool critical )
{
	for ( int i = 0; i < weight; i++ )
	{
		_systems.append( pSystem );
	}
	if ( critical )
	{
		_criticalSystems[ pSystem ] = 0;
	}
}

// ============================================================================
/// Body is destroyed. Al critical systems are destrotyed too.
void DamageManager::destroy()
{
	foreach( System* pSystem, _criticalSystems.keys() )
	{
		pSystem->destroy();
	}
	
	// cease further operation
	_criticalSystems.clear();
	_systems.clear();
}

}
