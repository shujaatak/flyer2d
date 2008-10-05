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

#include "worldobject.h"

namespace Flyer {

// ============================================================================
/// Constructor, creates top-level object.
WorldObject::WorldObject( World* pWorld )
{
	Q_ASSERT( pWorld );
	_pWorld = pWorld;
	_pParent = NULL;
	_renderLayer = 0;
	worldPrivateData = NULL;
}

// ============================================================================
/// Constructor, creates item with parent.
WorldObject::WorldObject( WorldObject* pParent )
{
	Q_ASSERT( pParent );
	_pParent = pParent;
	_pWorld = pParent->world();
	pParent->_children.append( this );
	_renderLayer = 0;
}

// ============================================================================
// Destructor
WorldObject::~WorldObject()
{
	if ( _pParent ) _pParent->_children.removeAll( this );
}


}

// EOF

