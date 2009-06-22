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

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include "editablewrapper.h"

#include "editableitem.h"

EditableItem::EditableItem()
		: QGraphicsItem()
{
	setFlag( QGraphicsItem::ItemIsSelectable, true );
	setZValue( 0.0 ); // some small value
	_pWrapper = NULL;
}


EditableItem::~EditableItem()
{
	if ( _pWrapper )
	{
		_pWrapper->itemDeleted();
	}
}

// ============================================================================
/// Mouse press event handler.
void EditableItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
	// ignoer, so item isn;t selected, and event is passed to editor points that can lie underneath
	event->ignore();
}

// ============================================================================
//. Message from point - point clicked.
void EditableItem::pointClicked( EditorPoint* )
{
	if ( _pWrapper )
	{
		_pWrapper->itemSelected();
	}
	scene()->clearSelection();
	setSelected( true );
}

// ============================================================================
// Handles point movement
void EditableItem::pointMoved( EditorPoint* )
{
	prepareGeometryChange();
	update();
	_pWrapper->itemChanged();
}
