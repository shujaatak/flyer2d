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

#include "editableitem.h"

#include "editablewrapper.h"

// ============================================================================
// Constructor
EditableWrapper::EditableWrapper( QObject* parent, QGraphicsScene* pScene ): QObject( parent )
{
	_selected = false;
	_pScene = pScene;
	_pItem = NULL;
}

// ============================================================================
// Destructor
EditableWrapper::~EditableWrapper()
{
	if ( _pItem ) delete _pItem;
}

// ============================================================================
// Item is deleted - message from outside
void EditableWrapper::itemDeleted()
{
	_pItem = NULL;
}

// ============================================================================
// Selects/deselects item
void EditableWrapper::setSelected( bool s )
{
	_selected = s;
	if ( _pItem )
	{
		_pItem->setSelected( s );
		_pItem->update();
	}
}

// ============================================================================
// Message from item - item was selected.
void EditableWrapper::itemSelected()
{
	emit selected( this );
}
