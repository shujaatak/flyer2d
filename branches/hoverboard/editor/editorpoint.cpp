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
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "editableitem.h"

#include "editorpoint.h"

static const double SIZE = 10;// [px]

// ============================================================================
// Constructor
EditorPoint::EditorPoint(QGraphicsItem* parent)
		: QGraphicsItem(parent)
{
	setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIgnoresTransformations );
	setAcceptsHoverEvents( true );
	_hover = false;
	setZValue( 10.0 ); // some hgih value
}

// ============================================================================
// Destructor
EditorPoint::~EditorPoint()
{
}

// ============================================================================
/// Returns boundong rect.
QRectF EditorPoint::boundingRect () const
{
	return QRectF( -SIZE/2, -SIZE/2, SIZE, SIZE );
}

// ============================================================================
/// Paints point
void EditorPoint::paint ( QPainter * painter,
	 const QStyleOptionGraphicsItem * , QWidget *  )
{
	QRectF r( -SIZE/2, -SIZE/2, SIZE, SIZE );
	
	if ( _hover )
	{
		painter->setBrush( Qt::gray );
	}
	else
	{
		painter->setBrush( Qt::NoBrush );
	}
	
	painter->setPen( Qt::black );
	painter->drawRect( r );
}

// ============================================================================
// Hover enter
void EditorPoint::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
	_hover = true;
	update();
}

// ============================================================================
// Hover leave
void EditorPoint::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
	_hover = false;
	update();
}

// ============================================================================
// Mouse move event
void EditorPoint::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	QGraphicsItem::mouseMoveEvent( event );
	
	EditableItem* pParent = dynamic_cast<EditableItem*>( parentItem() );
	if ( pParent )
	{
		pParent->pointMoved( this );
	}
}

// ============================================================================
/// Mouse down handle
void EditorPoint::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
	QGraphicsItem::mousePressEvent( event );
	EditableItem* pParent = dynamic_cast<EditableItem*>( parentItem() );
	if ( pParent )
	{
		pParent->pointClicked( this );
	}
}

// EOF

