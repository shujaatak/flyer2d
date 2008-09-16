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

#include "world.h"
#include "ground.h"

#include "cloud.h"

namespace Flyer
{

// ============================================================================
/// Constructor
Cloud::Cloud ( World* pWorld ) : WorldObject ( pWorld )
{
	_birdthDate = pWorld->time();
}

// ============================================================================
/// Destructor
Cloud::~Cloud()
{
}

// ============================================================================
// 
QRectF Cloud::boundingRect() const
{
	// calculate br for the fors time
	if ( _boundingRect.isNull() )
	{
		QRectF initialBR = QRectF( _position.x - _radius, _position.y - _radius, _radius*2, _radius*2 );
		
		b2Vec2 finalPos  = _position + _lifespan*_velocity;
		double finalRadius = _radius + _expansionSpeed*_lifespan;
		
		QRectF finalBR = QRectF( finalPos.x - finalRadius, finalPos.y - finalRadius, finalRadius*2, finalRadius*2 );
		
		QRectF br = initialBR | finalBR;
		_boundingRect = br;
	}

	return _boundingRect;
}

// ============================================================================
/// Renders cloud. 
void Cloud::render ( QPainter& painter, const QRectF& rect )
{
	double age = world()->time() - _birdthDate;
	if ( age > _lifespan )
	{
		world()->removeObject( this );
	}
	else
	{
		// find alpha
		double alpha = _color.alphaF() * (1.0 - age / _lifespan );
		QColor currentColor = _color;
		currentColor.setAlphaF( alpha );
		
		// find radius
		double currentRadius = _radius + _expansionSpeed * age;
		
		// find position
		b2Vec2 p = _position + age * _velocity;
		p.y = qMax( double(p.y), world()->ground()->height( p.x ) ); // prevents falling under ground
		
		painter.setBrush( currentColor );
		painter.setPen( Qt::NoPen );
		painter.drawEllipse( QRectF( p.x, p.y, currentRadius*2, currentRadius*2 ) );
	}
}

// ============================================================================
/// Creates smoke cloud, adds to world
Cloud* Cloud::createSmoke( World* pWorld, const b2Vec2& pos )
{
	Cloud* pSmoke = new Cloud( pWorld );
	
	int alpha = 64 + qrand()%64;
	
	pSmoke->setColor( QColor( 0, 0, 0, 128 ) );
	double vx = (qrand() % 400)/100.0 - 2; // -2 - +2
	double vy = (qrand() % 600)/100.0 - 1; // -1 - 5
	
	double radius = ( qrand() % 100 ) / 100.0 + 0.5;
	
	double lifespan = 1 + (qrand()%100)/100.0; // 1-2 s
	
	pSmoke->setVelocity( b2Vec2( vx, vy ) );
	pSmoke->setPosition( pos );
	pSmoke->setRadius( radius );
	pSmoke->setExpansion( 0.1 );
	pSmoke->setLifespan( 3 );
	
	pWorld->addObject( pSmoke, World::ObjectStatic | World::ObjectRenderedForeground );
	
	return pSmoke;
}

}
