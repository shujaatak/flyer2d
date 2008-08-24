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

#include "body.h"

namespace Flyer
{

// ============================================================================
// Constructor
Body::Body( const QString& name )
{
	_name = name;
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
	
	// destroy body
	if ( _pBody )
	{
		_pBody->GetWorld()->DestroyBody( _pBody );
	}
}

// ============================================================================
// Creates body
void Body::create( const b2BodyDef& def, b2World* pWorld )
{
	Q_ASSERT( pWorld );
	_definition = def;
	_definition.userData = this;
	_pBody = pWorld->CreateBody( & _definition );
	
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
void Body::addShape( b2ShapeDef* pShapeDef, bool removeUserData )
{
	Q_ASSERT( pShapeDef );
	if ( removeUserData )
	{
		pShapeDef->userData = NULL;
	}
	
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
				
				// also, add line across the circle
				path.closeSubpath();
				path.moveTo( center.x - r, 0 );
				path.lineTo( center.x + r, 0 );
				path.closeSubpath();
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
/// Flips body along defined axis.
/// Axis is defined as pair of points.
void Body::flip( const QPointF& p1, const QPointF& p2 )
{
	// flip shapes uside down
	foreach( b2ShapeDef* pShapeDef, _shapeDefinitions )
	{
		// polygon
		if ( pShapeDef->type == e_polygonShape )
		{
			b2PolygonDef* pPoygonDef = (b2PolygonDef*)pShapeDef;
			
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
		else if ( pShapeDef->type == e_circleShape )
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
		
		// get current position
		b2Vec2 pos = _pBody->GetPosition();
		double angle = _pBody->GetAngle();
		
		//transform position
		
		double axisAngle = atan2( p2.y() - p1.y(), p2.x() - p1.x() );
		double ds = (p2.y() - p1.y())*(p2.y() - p1.y()) + (p2.x() - p1.x())*(p2.x() - p1.x()); // distance squared between p1 nad p2
		double u = ( ( pos.x - p1.x() ) * (p2.x() - p1.x()) + (pos.y - p1.y()) * (p2.y() - p1.y())) / ds; // helper value
		// closest point on axis
		double cx = p1.x()+ u * (p2.x() - p1.x());
		double cy = p1.y()+ u * (p2.y() - p1.y());
		
		double newAngle = angle - 2 * (angle-axisAngle);
		b2Vec2 newPos( pos.x - 2*(pos.x-cx), pos.y - 2*(pos.y-cy) );
		

		// preserve dynamic data
		
		b2Vec2 linearSpeed = _pBody->GetLinearVelocity();
		double angularSpeed = _pBody->GetAngularVelocity();
		
		// modify definition
		_definition.position = newPos;
		_definition.angle = newAngle;
		
		// re-create with new def
		pWorld->DestroyBody( _pBody );
		_pBody = NULL;
		
		create( _definition, pWorld ); // note - shapes are added here
		
		// restore dynamic data
		_pBody->SetLinearVelocity( linearSpeed );
		_pBody->SetAngularVelocity( angularSpeed );
		
	}
}

// ============================================================================
// Renders body
void Body::render( QPainter& painter )
{
	if ( _pBody )
	{
		
		QTransform t = transform();
		
		painter.save();
			painter.setTransform( t, true );
			painter.drawPath( shape() );
		painter.restore();
	
	}
}

// ============================================================================
/// Checks if body is connected to another through joints.
bool Body::isConnectedTo( Body* pBody ) const
{
	if ( ! pBody ) return false;
	if ( ! _pBody ) return false;
	
	QList<Body*> visited ;
	return doIsConnectedTo( pBody, visited );
}
// ============================================================================
/// Checks if body is connected to another through joints.
/// List of visited bodies prevents infinite reccursion.
bool Body::doIsConnectedTo( Body* pBody, QList<Body*>& visited ) const
{
	visited.append( this );
	
	b2JointEdge* pJointEdge = _pBody->GetJointList();
	while( pJointEdge )
	{
		Body* pOtherBody = static_cast<Body*>( pJointEdge->other->GetUserData() );
		
		if ( pOtherBody == pBody )
		{
			return true;
		}
		else if ( pOtherBody && ! visited.contains( pOtherBody ) && pOtherBody->doIsConnectedTo( pBody, visited ) )
		{
			return true;
		}
		
		pJointEdge = pJointEdge->next;
	}
	
	return false;
}

// ============================================================================
/// Destroys phisical representation
void Body::destroy()
{
	if ( _pBody )
	{
		_pBody->GetWorld()->DestroyBody( _pBody );
		_pBody = NULL;
	}
}

// ============================================================================
/// Creates exact copy
Body* Body::createCopy() const
{
	Body* pCopy = new Body(); // TODO different types here
	
	// copy shapes
	foreach( b2ShapeDef* pShapeDef, _shapeDefinitions )
	{
		pCopy->addShape( pShapeDef, true );
	}
	
	// create
	if ( _pBody )
	{
		b2BodyDef definition = _definition;
		definition.position = _pBody->GetPosition();
		definition.angle = _pBody->GetAngle();
		
		pCopy->create( definition, _pBody->GetWorld() );
		pCopy->b2body()->SetLinearVelocity( _pBody->GetLinearVelocity() );
		pCopy->b2body()->SetAngularVelocity( _pBody->GetAngularVelocity() );
	}
	
	return pCopy;
}

}
