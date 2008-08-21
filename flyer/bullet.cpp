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

#include "b2dqt.h"
#include "bullet.h"
#include "world.h"

namespace Flyer
{

// ============================================================================
// Constructor
Bullet::Bullet ( World* pWorld ) : WorldObject ( pWorld )
{
	_lifespan = 0;
	_mass = 0;
	_size = 0;
	_age = 0;
}

// ============================================================================
// Destructor
Bullet::~Bullet()
{
}

// ============================================================================
// Returns bullet's boundong rect
QRectF Bullet::boundingRect()
{
	if ( _body.b2body() )
	{
		// bullet fired, return rect
		b2Vec2 pos = _body.b2body()->GetPosition();
		
		return QRectF( pos.x - _size/2, pos.y - _size/2, _size, _size );
	}
	else
	{
		// bullet not fired yet, don't paint
		return QRectF();
	}
}

// ============================================================================
// Renders bullet
void Bullet::render ( QPainter& painter, const QRectF& )
{
	if ( _body.b2body() )
	{
		// bullet fired, return rect
		b2Vec2 pos = _body.b2body()->GetPosition();
		painter.setPen( Qt::black );
		painter.setBrush( Qt::black );
		
		QRectF r( pos.x - _size/2, pos.y - _size/2, _size, _size );
		painter.drawEllipse( r );
	}
}

// ============================================================================
// Simulates
void Bullet::simulate ( double dt )
{
	_age += dt;
	if ( _age > _lifespan )
	{
		// TODO delete here
	}
}
// ============================================================================
/// Fires bullet from given point with given velocity
void Bullet::fire( const QPointF& point, const QPointF& velocity )
{
	// create body
	b2BodyDef def;
	def.position = point2vec( point );
	def.massData.mass = _mass;
	
	_body.create( def, world()->b2world() );
	
	// create shape
	b2CircleDef shape;
	shape.localPosition.SetZero();
	shape.radius = _size/2;
	
	_body.addShape( & shape );
	
	// init body
	_body.b2body()->SetLinearVelocity( point2vec( velocity ) );
	
	
}

}
