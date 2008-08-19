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

#include "Box2D.h"

#include "world.h"
#include "landinglight.h"

namespace Flyer {

LandingLight::LandingLight(World* pWorld, double x, double y, double angle )
	: WorldObject(pWorld)
{
	_x = x;
	_y = y;
	_angle = angle;
	
	// create body
	b2PolygonDef shape;
	shape.SetAsBox( 0.5, 0.5 );
	
	b2BodyDef def;
	def.position.Set( x, y + 0.5 );
	
	_pBody = pWorld->b2world()->CreateBody( & def );
	_pBody->CreateShape( & shape );
	
	_range = 1000;// 1 km
	_width = 0.1;
}

// ============================================================================
// Destructor
LandingLight::~LandingLight()
{
}

// ============================================================================
// Bounding rect
QRectF LandingLight::boundingRect()
{
	return QRectF( _x -_range*1.1, _y -_range*1.1, _range*2.2, _range*2.2 );
}

// ============================================================================
//  Renders object.
void LandingLight::render(QPainter& painter, const QRectF& rect)
{
	QTransform t;
	t.translate( _x, _y + 0.5 );
	t.rotateRadians( _angle );
	
	QPolygonF lightShape;
	lightShape.append( QPointF( 0,0 ) );
	lightShape.append( QPointF( _range, _range*_width/2 ) );
	lightShape.append( QPointF( _range, -_range*_width/2 ) );
	
	QLinearGradient gradient( QPointF(0,0), QPointF( _range, 0 ) );
	gradient.setColorAt( 0, QColor( 255, 255, 0, 120) );
	gradient.setColorAt( 1, QColor( 255, 255, 0, 0) );
	
	QPolygonF standShape;
	standShape.append( QPointF( _x-0.4, _y ) );
	standShape.append( QPointF( _x+0.4, _y ) );
	standShape.append( QPointF( _x, _y+0.6 ) );
	
	painter.save();
		
		painter.setTransform( t, true );
		
		// draw light
		painter.setPen( Qt::NoPen );
		painter.setBrush( gradient );
		painter.drawPolygon( lightShape );
		
		// draw box
		painter.setPen( Qt::black );
		painter.setBrush( Qt::gray );
		painter.drawRect( QRectF( -0.3, -0.3, 0.6, 0.6 ) );
		
	painter.restore();
	
	// draw stand
	painter.setPen( Qt::black );
	painter.setBrush( Qt::gray );
	painter.drawPolygon( standShape );
	
}

}
