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

#include "gexception.h"
#include "body.h"

#include "shape.h"

namespace Flyer
{

// ============================================================================
// Constructor
Shape::Shape( b2ShapeDef* pDef ) : Serializable()
{
	_pDef = NULL;
	_pShape = NULL;
	if ( pDef )
	{
		_pDef = createDefCopy( pDef );
	}
}

// ============================================================================
// Copy constructor
Shape::Shape( const Shape& src ) : Serializable( src )
{
	_pDef = createDefCopy( src._pDef );
	_name = src._name;
	_pShape = NULL; // TODO created object not copied
}


// ============================================================================
// Destructor
Shape::~Shape()
{
	if ( _pDef ) delete _pDef;
}

// ============================================================================
/// Creates copy of shape definition. Caller owns created object.
b2ShapeDef* Shape::createDefCopy( b2ShapeDef* pShapeDef )
{
	if ( ! pShapeDef ) return NULL;
	
	// copy shape
	if ( pShapeDef->type == e_polygonShape )
	{
		b2PolygonDef* pPolygonDef = new b2PolygonDef( * ((b2PolygonDef*)pShapeDef) );
		return pPolygonDef;
	}
	else if ( pShapeDef->type == e_circleShape )
	{
		b2CircleDef* pCircleDef = new b2CircleDef( * ((b2CircleDef*)pShapeDef ) );
		return pCircleDef;
	}
	
	//qFatal("Unknown shape type");
	return NULL;
}

// ============================================================================
/// Flips shape upside-down along the X axis
void Shape::flip()
{
	// polygon
	if ( _pDef->type == e_polygonShape )
	{
		b2PolygonDef* pPoygonDef = (b2PolygonDef*)_pDef;
		
		// copy vertices to buffer
		b2Vec2 buffer[ b2_maxPolygonVertices ];
		for ( int i = 0; i < pPoygonDef->vertexCount; i++ )
		{
			buffer[i] = pPoygonDef->vertices[i];
		}

		// copy back from byffer, but reversed and flipped
		for ( int j = 0; j < pPoygonDef->vertexCount; j++ )
		{
			pPoygonDef->vertices[j].x = buffer[ pPoygonDef->vertexCount - j - 1 ].x;
			pPoygonDef->vertices[j].y = - buffer[ pPoygonDef->vertexCount - j - 1 ].y;
		}
	}
	// circle
	else if ( _pDef->type == e_circleShape )
	{
		b2CircleDef* pCircleDef = (b2CircleDef*)_pDef;
		
		pCircleDef->localPosition.y = - pCircleDef->localPosition.y;
	}
}

// ============================================================================
/// Saves to stream.
void Shape::toStream( QDataStream& stream ) const
{
	if ( _pDef )
	{
		// common part
		stream << _pDef->type;
		stream << _pDef->restitution;
		stream << _pDef->isSensor;
		stream << _pDef->friction;
		stream << _pDef->density;
		// polygon
		if ( _pDef->type == e_polygonShape )
		{
			b2PolygonDef* pPolygonDef = (b2PolygonDef*)_pDef;
			stream << int( pPolygonDef->vertexCount );
			for( int i = 0; i < pPolygonDef->vertexCount; i++ )
			{
				stream << pPolygonDef->vertices[i].x;
				stream << pPolygonDef->vertices[i].y;
			}
		}
		// circle
		else if ( _pDef->type == e_circleShape )
		{
			b2CircleDef* pCircleDef = (b2CircleDef*)_pDef;
			stream << pCircleDef->localPosition.x;
			stream << pCircleDef->localPosition.y;
			stream << pCircleDef->radius;
		}
		else
		{
			throw GValueError("Shape::toStream: Unknown shape type");
		}
	}
	
	stream << _name;
}

// ============================================================================
/// Loads from stream
void Shape::fromStream( QDataStream& stream )
{
	// delete current definition
	if ( _pDef )
	{
		delete _pDef;
		_pDef = NULL;
	}
	
	// read type
	int type;
	stream >> type;
	if ( type == e_polygonShape )
	{
		_pDef = new b2PolygonDef();
	}
	else if ( type == e_circleShape )
	{
		_pDef = new b2CircleDef();
	}
	else
	{
		throw GDatasetError("Shape::fromStream: Unknown shape type");
	}
	
	// common part
	stream >> _pDef->restitution;
	stream >> _pDef->isSensor;
	stream >> _pDef->friction;
	stream >> _pDef->density;
	
	// polygon
	if ( _pDef->type == e_polygonShape )
	{
		b2PolygonDef* pPolygonDef = (b2PolygonDef*)_pDef;
		stream >> (int&) pPolygonDef->vertexCount;
		for( int i = 0; i < pPolygonDef->vertexCount; i++ )
		{
			stream >>  pPolygonDef->vertices[i].x;
			stream >> pPolygonDef->vertices[i].y;
		}
	}
	// circle
	else
	{
		b2CircleDef* pCircleDef = (b2CircleDef*)_pDef;
		stream >> pCircleDef->localPosition.x;
		stream >> pCircleDef->localPosition.y;
		stream >> pCircleDef->radius;
	}
	
	stream >> _name;
	
}

// ============================================================================
/// Associates damage manager with body
void Shape::setDamageManager( DamageManager* pManager )
{
	if ( _pDef ) _pDef->userData = pManager;
	if ( _pShape ) _pShape->SetUserData( pManager );
}

// ============================================================================
/// Creates b2d shape object using body as context.
void Shape::create( Body* pBody )
{
	Q_ASSERT( pBody );
	Q_ASSERT( pBody->b2body() );
	
	_pShape = pBody->b2body()->CreateShape( _pDef );
}

}
