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

#include "body.h"
#include "world.h"
#include "common.h"

#include "shrapnel.h"

namespace Flyer
{

// ============================================================================
// Constructor
Shrapnel::Shrapnel( World* pWorld )
		: PhysicalObject(pWorld)
{
	_lifespan = 30; // resonable default
	_age = 0;
	setRenderLayer( LayerForeground );
}

// ============================================================================
// Destructor
Shrapnel::~Shrapnel()
{
}

// ============================================================================
// Simulates shrapnel
void Shrapnel::simulate( double dt )
{
	_age += dt;
	if ( _age >= _lifespan )
	{
		world()->removeObject( this );
	}
}

// ============================================================================
/// Adds body to shrapnel.
void Shrapnel::addBody( Body* pBody )
{
	PhysicalObject::addBody( pBody, BodyRendered1 );
	if ( ! mainBody() )
	{
		setMainBody( pBody );
	}
}

}
