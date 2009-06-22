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

#include <QPainter>

#include "editorpoint.h"
#include "editablewrapper.h"

#include "editorcircle.h"

// ============================================================================
// Constructor
EditorCircle::EditorCircle(): EditableItem()
{
	_pCenter = new EditorPoint( this );
	_pRim = new EditorPoint( this );
}

// ============================================================================
/// Destructor
EditorCircle::~EditorCircle()
{
}

// ============================================================================
/// Point moved
void EditorCircle::pointMoved(EditorPoint* pPoint )
{
	prepareGeometryChange();
	// center
	if ( pPoint == _pCenter )
	{
		_pRim->setPos( _pCenter->pos() + _rimPos );
	}
	// rim
	else
	{
		_rimPos = _pRim->pos() - _pCenter->pos();
	}
	update();
	_pWrapper->itemChanged();
}

// ============================================================================
// Center
QPointF EditorCircle::center() const
{
	return _pCenter->pos();
}

// ============================================================================
// Calculatyes radius
double EditorCircle::radius() const
{
	QPointF r = _pCenter->pos() - _pRim->pos();
	
	return sqrt( r.x() * r.x() + r.y() * r.y() );
}

// ============================================================================
// Boundiong rect
QRectF EditorCircle::boundingRect () const
{
	double r = radius();
	QPointF c = center();
	const double m = r*0.1; // margin
	return QRectF( c.x()-r-m, c.y()-r-m, 2*(r+m), 2*(r+m) );
}

// ============================================================================
// Paints
void EditorCircle::paint ( QPainter * painter,
		 const QStyleOptionGraphicsItem * , QWidget *  )
{
	
	double r = radius();
	QPointF c = center();
	QRectF rect( c.x()-r, c.y()-r, 2*r, 2*r );
	
	painter->setBrush( Qt::NoBrush );
	if ( isSelected() )
	{
		painter->setPen( Qt::blue );
	}
	else
	{
		painter->setPen( Qt::black );
	}
	
	painter->drawEllipse( rect );
}

// ============================================================================
// initializes cicrcle
void EditorCircle::init( const QPointF& c, double r )
{
	_pCenter->setPos( c );
	_pRim->setPos( c.x() + r, c.y() );
	_rimPos = QPointF( r, 0 );
}

// EOF

