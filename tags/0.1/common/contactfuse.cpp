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

#include "contactfuse.h"
#include "world.h"
#include "machine.h"
#include "common.h"

#include "explosion.h"

namespace Flyer
{

// ============================================================================
// Constructor
ContactFuse::ContactFuse ( Machine* pParent, const QString& name ) : System ( pParent, name )
{
	_energy = 0;
	_damageReceived = 0;
	_destroyed = false;
}

// ============================================================================
// Destructor
ContactFuse::~ContactFuse()
{
}

// ============================================================================
// Handles damage
void ContactFuse::damage ( double force )
{
	_damageReceived += force;
	//qDebug("fuse: %g/%g", _damageReceived, damageCapacity() ); TODO remove
	if ( ! _destroyed && _damageReceived >= damageCapacity() )
	{
		Explosion* pExplosion = new Explosion( parent()->world() );
		pExplosion->setEnergy( _energy );
		pExplosion->setCenter( parent()->position() );
		pExplosion->setRenderLayer( LayerForeground );
	
		//add explosion to the world, remove parent
		parent()->world()->addObject( pExplosion, World::ObjectSimulated | World::ObjectRendered );
		parent()->world()->removeObject( parent() );
		_destroyed = true;
	}
}

}
