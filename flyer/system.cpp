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

#include "Box2D.h"

#include "system.h"


namespace Flyer
{

static const double DEFAULT_DAMAGE_CAPACITY = 5000;


// ============================================================================
// Constructor
System::System( const QString& name ):  _name( name )
{
	// init members
	_pBody = NULL;
	_damageCapacity = DEFAULT_DAMAGE_CAPACITY;
}

// ============================================================================
// Destructor
System::~System()
{
	// nope
}

// ============================================================================
// Renders system body
void System::renderBody( QPainter& painter )
{
	if ( _pBody )
	{
		double angle = _pBody->GetAngle();
		b2Vec2 pos = _pBody->GetPosition();
		
		QTransform t;
		t.translate( pos.x, pos.y );
		t.rotateRadians( angle );
		
		painter.save();
			painter.setTransform( t, true );
			
			b2Shape* pShape = _pBody->GetShapeList();
			while ( pShape )
			{
				renderShape( painter, pShape );
				pShape = pShape->GetNext();
			}
		painter.restore();
	
	}
}

// ============================================================================
// Renders shape
void System::renderShape( QPainter& painter, b2Shape* pShape )
{
	if ( ! pShape ) return;
	
	// polygon shape
	if ( pShape->GetType() == e_polygonShape )
	{
		b2PolygonShape* pPolygon = (b2PolygonShape*)pShape;
		
		QPolygonF polygon;
		const b2Vec2* pVertices = pPolygon->GetVertices();
		for ( int i = 0; i < pPolygon->GetVertexCount(); i++ )
		{
			polygon.append( QPointF( pVertices[i].x, pVertices[i].y ) );
		}
		
		painter.drawPolygon( polygon );
	}
	
}

}
