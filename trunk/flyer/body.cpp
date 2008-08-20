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

#include "body.h"

namespace Flyer
{

// ============================================================================
// Constructor
Body::Body()
{
	_pBody = NULL;
}

// ============================================================================
// Destructor
Body::~Body()
{
	// remove cached shapes definitions
	foreach( b2ShapeDef* pShapeDef, _shapeDefinitions )
	{
		delete pShapeDef;
	}
}

// ============================================================================
// Creates body
void Body::create( const b2BodyDef& def, b2World* pWorld )
{
	Q_ASSERT( pWorld );
	_pBody = pWorld->CreateBody( & def );
	_definition = def;
	
	// if shapes added - create them
	foreach( b2ShapeDef* pShapeDef, _shapeDefinitions )
	{
		_pBody->CreateShape( pShapeDef );
	}
	
	// create mass
	if ( _definition.massData.mass == 0 )
	{
		_pBody->SetMassFromShapes();
	}
}

// ============================================================================
// Adds shape to body
void Body::addShape( b2ShapeDef* pShapeDef )
{
	Q_ASSERT( pShapeDef );
	// copy shape
	if ( pShapeDef->type == e_polygonShape )
	{
		b2PolygonDef* pPolygonDef = new b2PolygonDef( * ((b2PolygonDef*)pShapeDef) );
		_shapeDefinitions.append( pPolygonDef );
	}
	else if ( pShapeDef->type == e_circleShape )
	{
		b2CircleDef* pCircleDef = new b2CircleDef( * ((b2CircleDef*)pShapeDef ) );
		_shapeDefinitions.append( pCircleDef );
	}
	else
	{
		qFatal("Unknown shape type");
	}
	
	// create shaope if body available
	if ( _pBody )
	{
		_pBody->CreateShape( pShapeDef );
		// create mass
		if ( _definition.massData.mass == 0 )
		{
			_pBody->SetMassFromShapes();
		}
	}
}

// ============================================================================
/// Returns body shape as painter path, in object-local local coordinates
QPainterPath Body::shape() const
{
	QPainterPath path;
	if ( _pBody )
	{
		
		b2Shape* pShape = _pBody->GetShapeList();
		while ( pShape )
		{
			
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
				
				path.addPolygon( polygon );
				path.closeSubpath();
			}
			// cicrlce shape
			else if ( pShape->GetType() == e_circleShape )
			{
				b2CircleShape* pCircle = (b2CircleShape*)pShape;
				
				b2Vec2 center = pCircle->GetLocalPosition();
				double r = pCircle->GetRadius();
				path.addEllipse( center.x - r, center.y - r, 2*r, 2*r );
			}
			else
			{
				qFatal("Other shapes not implemented");
			}
			
			// next
			pShape = pShape->GetNext();
		} // while(pShape)
	
	} // _pBody

	path.setFillRule( Qt::WindingFill );
	return path;
}

// ============================================================================
/// Returns body coorindtaes as transofrmation.
QTransform Body::transform() const
{
	QTransform t;
	if ( _pBody )
	{
		double angle = _pBody->GetAngle();
		b2Vec2 pos = _pBody->GetPosition();
		
		t.translate( pos.x, pos.y );
		t.rotateRadians( angle );
	}
	
	return t;
}

// ============================================================================
/// Flips body along it's horizontal axis.
void Body::flipVertical()
{
	// flip shapes
	foreach( b2ShapeDef* pShapeDef, _shapeDefinitions )
	{
		// polygon
		if ( pShapeDef->type = e_polygonShape )
		{
			b2PolygonDef* pPoygonDef = (b2PolygonDef*)pShapeDef;
			for ( int i = 0; i < pPoygonDef->vertexCount; i++ )
			{
				pPoygonDef->vertices[i].y = - pPoygonDef->vertices[i].y;
			}
		}
		// circle
		else if ( pShapeDef->type = e_circleShape )
		{
			b2CircleDef* pCircleDef = (b2CircleDef*)pShapeDef;
			
			pCircleDef->localPosition.y = - pCircleDef->localPosition.y;
		}
	}

	// re-create body
	if ( _pBody )
	{
		// get world
		b2World* pWorld = _pBody->GetWorld();
		
		// preserve sensitive data
		b2Vec2 pos = _pBody->GetPosition();
		double angle = _pBody->GetAngle();
		b2Vec2 linearSpeed = _pBody->GetLinearVelocity();
		double angularSpeed = _pBody->GetAngularVelocity();
		
		// modify definition
		_definition.position = pos;
		_definition.angle = angle;
		
		// re-create with new def
		pWorld->DestroyBody( _pBody );
		_pBody = NULL;
		
		create( _definition, pWorld ); // note - shapes are added here
		
		// restore dynamic data
		_pBody->SetLinearVelocity( linearSpeed );
		_pBody->SetAngularVelocity( angularSpeed );
		
	}

}

}
