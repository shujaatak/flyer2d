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

#include "common.h"

#include "airfield.h"

namespace Flyer {

// ============================================================================
// Constructor
Airfield::Airfield( World* pWorld, double x1, double x2, double y )
	: WorldObject(pWorld)
{
	_x1 = x1;
	_x2 = x2;
	_y = y;
	setRenderLayer( LayerHud );
	setName("Aitfield");
}


// ============================================================================
// Destructor
Airfield::~Airfield()
{
}

// ============================================================================
// Returs boundign rect
QRectF Airfield::boundingRect()
{
	QRect br( _x1 - 1, _y -1, (_x2-_x1)+2, 2 );
	return br;
}

// ============================================================================
// Renders on map.
void Airfield::renderOnMap(QPainter& painter, const QRectF& )
{
	QRectF rect( _x1, _y-200, (_x2-_x1), 200 );
	
	painter.fillRect( rect, Qt::darkBlue );
}

// ============================================================================
// Renders
void Airfield::render(QPainter& painter, const QRectF&, const RenderingOptions& )
{
	QRectF rect( _x1, _y-1, (_x2-_x1), 1 );
	
	painter.fillRect( rect, Qt::darkBlue );
}

}
