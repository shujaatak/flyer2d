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

#include "textureprovider.h"

#include "body.h"

namespace Flyer
{

// ============================================================================
// Constructor
Body::Body( const QString& name )
{
	_name = name;
	_pBody = NULL;
	_layers = 0;
	_textureScale = 0.2; // TODO some value
}

// ============================================================================
// Destructor
Body::~Body()
{
	// destroy body
	if ( _pBody )
	{
		// remove all user data fiorst, to not interact with the body
		b2Shape* pShape = _pBody->GetShapeList();
		while ( pShape )
		{
			pShape->SetUserData( NULL );
			pShape = pShape->GetNext();
		}
		_pBody->SetUserData( NULL );
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
	foreach( const Shape& shape, _shapes )
	{
		_pBody->CreateShape( shape.def() );
	}
	
	// create mass
	if ( _definition.massData.mass == 0 )
	{
		_pBody->SetMassFromShapes();
	}
}

// ============================================================================
// Adds shape to body
// Returns pointer to shape in body's intenral shape list
Shape* Body::addShape( const Shape& shape, bool removeUserData )
{
	_shapes.append( shape ); // copy
	
	b2ShapeDef* pDef = _shapes.last().def();
	Q_ASSERT( pDef );
	
	if ( removeUserData )
	{
		pDef->userData = NULL;
	}
	
	pDef->filter.categoryBits = _layers;
	pDef->filter.maskBits = _layers;
	
	// create b2 shape if body available
	if ( _pBody )
	{
		_pBody->CreateShape( _shapes.last().def() );
		// create mass
		if ( _definition.massData.mass == 0 )
		{
			_pBody->SetMassFromShapes();
		}
	}
	
	return & _shapes.last();
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
	for( int i = 0; i < _shapes.size(); i++ )
	{
		_shapes[i].flip();
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
		
		// NOTE similar code used in autopilot
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
			if ( ! _texture.isNull() )
			{
				t.scale( _textureScale, -_textureScale );
				painter.setTransform( t, true );
				painter.drawPixmap( _texturePosition/_textureScale, _texture );
			}
			else
			{
				painter.setTransform( t, true );
				painter.drawPath( shape() );
			}
		painter.restore();
	
	}
}

// ============================================================================
/// Checks if body is connected to another through joints.
bool Body::isConnectedTo( Body* pBody ) const
{
	if ( ! pBody ) return false;
	if ( ! _pBody ) return false;
	
	QList<const Body*> visited ;
	return doIsConnectedTo( pBody, visited );
}
// ============================================================================
/// Checks if body is connected to another through joints.
/// List of visited bodies prevents infinite reccursion.
bool Body::doIsConnectedTo( Body* pBody, QList<const Body*>& visited ) const
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
	foreach( const Shape& shape, _shapes )
	{
		pCopy->addShape( shape, true );
	}
	
	// create
	if ( _pBody )
	{
		b2BodyDef definition = _definition;
		definition.position = _pBody->GetPosition();
		definition.angle = _pBody->GetAngle();
		
		pCopy->setLayers( _layers );
		pCopy->create( definition, _pBody->GetWorld() );
		pCopy->b2body()->SetLinearVelocity( _pBody->GetLinearVelocity() );
		pCopy->b2body()->SetAngularVelocity( _pBody->GetAngularVelocity() );
	}
	
	return pCopy;
}

// ============================================================================
/// Sets collision layers
void Body::setLayers( int layers )
{
	// modify stored definitions
	for( int i = 0; i < _shapes.size(); i++ )
	{
		_shapes[i].def()->filter.categoryBits = layers;
		_shapes[i].def()->filter.maskBits = layers;
	}
	
	// modify existing shapes
	if ( _pBody )
	{
		b2FilterData filter;
		filter.categoryBits = layers;
		filter.maskBits = layers;
		b2Shape* pShape = _pBody->GetShapeList();
		while( pShape )
		{
			pShape->SetFilterData( filter );
			pShape = pShape->GetNext();
		}
	}
	_layers = layers; // store for later use
}

// ============================================================================
/// Sets texture
void Body::setTexture( const QString& path )
{
	_texture = TextureProvider::loadTexture( path );
	_texturePath = path;
}

// ============================================================================
// Sets texture position
void Body::setTexturePosition( const QPointF& pos )
{
	_texturePosition = pos;
}

// ============================================================================
// Sets texture scale
void Body::setTextureScale( double s )
{
	_textureScale = s;
}

// ============================================================================
/// Stores body to stream.
void Body::toStream( QDataStream& stream ) const
{
	stream << _definition.position.x;
	stream << _definition.position.y;
	stream <<  _definition.massData.center.x;
	stream << _definition.massData.center.y;
	stream << _definition.massData.mass;
	stream << _definition.massData.I;
	stream << _definition.linearDamping;
	stream << _definition.angularDamping;
	stream << _definition.isBullet;
	stream << _definition.fixedRotation;
	stream << _definition.isSleeping;
	// dyamic data(?)
	stream << _definition.angle;
	stream << _definition.position.x;
	stream << _definition.position.y;
	
	// shapes
	stream << _shapes.size();
	foreach( const Shape& shape, _shapes )
	{
		stream << shape;
	}
	
	// texture
	stream << _texturePath;
	stream << _textureScale;
	stream << _texturePosition;
}

// ============================================================================
/// Loads body from stream.
/// Only definition os loaded. This method should be called before b2d object ios created.
void Body::fromStream( QDataStream& stream )
{
	// TODO what about body? should it be deleted now?
	
	stream >> _definition.position.x;
	stream >> _definition.position.y;
	stream >> _definition.massData.center.x;
	stream >> _definition.massData.center.y;
	stream >> _definition.massData.mass;
	stream >> _definition.massData.I;
	stream >> _definition.linearDamping;
	stream >> _definition.angularDamping;
	stream >> _definition.isBullet;
	stream >> _definition.fixedRotation;
	stream >> _definition.isSleeping;
	// dyamic data(?)
	stream >> _definition.angle;
	stream >> _definition.position.x;
	stream >> _definition.position.y;
	
	// shapes
	int shapeCount;
	stream >> shapeCount;
	_shapes.clear();
	for( int i = 0; i < shapeCount; i++ )
	{
		Shape shape;
		stream >> shape;
		_shapes.append( shape );
	}
	
	// texture
	stream >> _texturePath;
	stream >> _textureScale;
	stream >> _texturePosition;
}


}
