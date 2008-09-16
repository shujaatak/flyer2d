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

#include <QGraphicsScene>

#include "b2dqt.h"

#include "shapewrapper.h"
#include "editorpolygon.h"
#include "editorcircle.h"

// ============================================================================
// Constructor
ShapeWrapper::ShapeWrapper( Flyer::Shape* pShape, QGraphicsScene* pScene, QObject* parent )
	: EditableWrapper( parent, pScene )
{
	Q_ASSERT( pShape );
	Q_ASSERT( pShape->def() );
	_pShape = pShape;
	setObjectName( "Shape" );
	
	if ( pShape->def()->type == e_circleShape )
	{
		b2CircleDef* pCircleDef = (b2CircleDef*)pShape->def();
		EditorCircle* pCircle = new EditorCircle();
		pCircle->init( Flyer::vec2point(  pCircleDef->localPosition ), pCircleDef->radius );
		_pItem = pCircle;
	}
	else
	{
		EditorPolygon* pPolygon = new EditorPolygon();
		pPolygon->init( 4 );
		_pItem = pPolygon;
	}
	
	pScene->addItem( _pItem );
	_pItem->setWrapper( this );
	
	connect( this, SIGNAL(selected(EditableWrapper*)), parent, SIGNAL(selected(EditableWrapper*)) );
}

// ============================================================================
// Destructor
ShapeWrapper::~ShapeWrapper()
{
}

// ============================================================================
/// Adds vertex to poly shape
void ShapeWrapper::addVertex()
{
	if( _pShape->def()->type == e_polygonShape ) // check type
	{
		EditorPolygon* pPolygonEditor = dynamic_cast< EditorPolygon* >( _pItem );
		Q_ASSERT( pPolygonEditor );
		QPolygonF polygon = pPolygonEditor->polygon();
			
		if ( polygon.size() < b2_maxPolygonVertices ) // check limit
		{
			QPointF newPoint = ( polygon.first() + polygon.last() ) / 2;
			polygon.append( newPoint );
			pPolygonEditor->setPolygon( polygon );
		}
	}
}

// ============================================================================
/// Removes vertex from poly shape
void ShapeWrapper::removeVertex()
{
	if( _pShape->def()->type == e_polygonShape ) // check type
	{
		EditorPolygon* pPolygonEditor = dynamic_cast< EditorPolygon* >( _pItem );
		Q_ASSERT( pPolygonEditor );
		QPolygonF polygon = pPolygonEditor->polygon();
			
		if ( polygon.size() > 3 ) // check limit
		{
			polygon.pop_back();
			pPolygonEditor->setPolygon( polygon );
		}
	}
}

// ============================================================================
/// Checks validity.
bool ShapeWrapper::isValid() const
{
	if( _pShape->def()->type == e_polygonShape )
	{
		EditorPolygon* pItem = (EditorPolygon*)_pItem;
		
		QPolygonF polygon = pItem->polygon();
		
		if ( polygon.size() < 3 || ! EditorPolygon::isConvex( polygon ) )
		{
			return false;
		}
	}
	
	// circle always valid
	return true;
}

// ============================================================================
/// Synchronizes shape.
void ShapeWrapper::sync()
{
	// sync polygon
	if( _pShape->def()->type == e_polygonShape )
	{
		b2PolygonDef* pPolygonDef = (b2PolygonDef*)_pShape->def();
		EditorPolygon* pItem = (EditorPolygon*)_pItem;
		QPolygonF polygon = pItem->polygon();
		
		pPolygonDef->vertexCount = polygon.size();
		for( int i = 0 ; i < pPolygonDef->vertexCount; i++ )
		{
			pPolygonDef->vertices[i] = Flyer::point2vec( polygon[i] );
		}
	}
	// sync cicrcle
	else
	{
		b2CircleDef* pCircleDef = (b2CircleDef*) _pShape->def();
		EditorCircle* pCircle = (EditorCircle* )_pItem;
		
		pCircleDef->radius = pCircle->radius();
		pCircleDef->localPosition = Flyer::point2vec( pCircle->center() );
	}
}

// EOF


