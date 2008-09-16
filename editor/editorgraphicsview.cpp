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

#include "editorgraphicsview.h"

// ============================================================================
// Constructor
EditorGraphicsView::EditorGraphicsView( QWidget* parent ): QGraphicsView( parent )
{
}

// ============================================================================
// Destructor
EditorGraphicsView::~EditorGraphicsView()
{
}

// ============================================================================
// Draw background
void EditorGraphicsView::drawBackground( QPainter* painter, const QRectF& rect )
{
	// draw default bkgnd
	QGraphicsView::drawBackground( painter, rect );
	
	// draw grid
	// TODO only 00 for now
	QPen gridPen;
	gridPen.setCosmetic( true );
	gridPen.setColor( Qt::gray );
	gridPen.setWidth( 0 );
	
	painter->setPen( gridPen );
	
	// Y
	painter->drawLine
		( QPointF( 0, qMin( sceneRect().top(), rect.top() ) )
		, QPointF( 0, qMax( sceneRect().bottom(), rect.bottom() ) )
		);
	
	// X
	painter->drawLine
		( QPointF( qMin( sceneRect().left(), rect.left() ), 0 )
		, QPointF( qMax( sceneRect().right(), rect.right() ), 0 )
		);
}

