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
	: Machine(pWorld)
{
	_x = x;
	_y = y;
	_angle = angle;
	
	// init manager
	_damageManager = new DamageManager();
	_damageManager->setTolerance( 5E4 ); // 1t 

	
	// create body
	b2PolygonDef shape;
	shape.SetAsBox( 0.5, 0.5 );
	shape.restitution = 0.5;
	shape.userData = _damageManager;
	
	b2BodyDef def;
	def.position.Set( x, y + 0.5 );
	
	_body = new Body("Main");
	_body->create( def, pWorld->b2world() );
	_body->addShape( & shape );
	addBody( _body, 0 );
	setMainBody( _body );
	
	_range = 1000;// 1 km
	_width = 0.1;
	
	// init spotlight
	_sysLight = new Spotlight( this, "Spotlight" );
	_sysLight->setAngle( angle );
	_sysLight->setRange( _range );
	_sysLight->setColor( Qt::yellow );
	_sysLight->setWidth( _width );
	_sysLight->setOn( true );
	_sysLight->setBody( _body );
	_sysLight->setDamageCapacity( 100E3 ); // 10t
	addSystem( _sysLight, SystemRendered1 );
	
	_damageManager->addSystem( _sysLight, 1 ); // 50/50
	_damageManager->addSystem( NULL, 1 );
}

// ============================================================================
// Destructor
LandingLight::~LandingLight()
{
}

// ============================================================================
// Bounding rect
QRectF LandingLight::boundingRect() const
{
	return QRectF( _x -_range*1.1, _y -_range*1.1, _range*2.2, _range*2.2 );
}

// ============================================================================
//  Renders object.
void LandingLight::render(QPainter& painter, const QRectF& rect)
{
	Machine::render( painter, rect ); // this will render light cone
	
	QTransform t;
	t.translate( _x, _y + 0.5 );
	t.rotateRadians( _angle );
	
	QPolygonF standShape;
	standShape.append( QPointF( _x-0.4, _y ) );
	standShape.append( QPointF( _x+0.4, _y ) );
	standShape.append( QPointF( _x, _y+0.6 ) );
	
	painter.save();
		
		painter.setTransform( t, true );
		
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
