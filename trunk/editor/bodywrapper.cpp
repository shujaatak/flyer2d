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

#include "shape.h"
#include "shapewrapper.h"
#include "textureprovider.h"
#include "texture.h"

#include "bodywrapper.h"


// ============================================================================
// Constructor
BodyWrapper::BodyWrapper( Flyer::Body *pBody, QGraphicsScene* pScene, QObject* parent )
		: EditableWrapper( parent, pScene )
{
	Q_ASSERT( pBody );
	_pBody = pBody;
	setObjectName( "Body" );
	_pTextureItem = NULL;
	
	// create wrappers for pre-exisintg shapes
	QList<Flyer::Shape>& rShapes = pBody->shapes();
	for( int i = 0; i < rShapes.size(); i ++ )
	{
		new ShapeWrapper( &(rShapes[i]), scene(), this );
	}
	
	// show texture, if body has it
	QString texture = pBody->texture();
	if ( ! texture.isNull() )
	{
		showTexture( texture );
	}
}

// ============================================================================
// Destructor
BodyWrapper::~BodyWrapper()
{
	delete _pTextureItem;
}

// ============================================================================
// Adds shape to body
ShapeWrapper* BodyWrapper::addShape( b2ShapeDef* pDef )
{
	Flyer::Shape* pShape = _pBody->addShape( Flyer::Shape( pDef ) );
	ShapeWrapper* pWrapper = new ShapeWrapper( pShape, scene(), this ); // create shape wrapper
	return pWrapper;
}

// ============================================================================
// Loads texture from path
void BodyWrapper::loadTexture( const QString& path )
{
	// put data into body
	_pBody->setTexture( path );
	showTexture( path );
}

// ============================================================================
// Shows texture in editor item
void BodyWrapper::showTexture( const QString& path )
{
	// show texture
	QImage image;
	Flyer::Texture texture = Flyer::TextureProvider::loadTexture( path );
	image = texture.image( Flyer::Texture::Normal );
	if ( ! image.isNull() )
	{
		// create item
		if ( ! _pTextureItem )
		{
			_pTextureItem = new QGraphicsPixmapItem();
			_pTextureItem->setZValue( -2.0 ); // background
			_pScene->addItem( _pTextureItem );
			_pTextureItem->setPos( _pBody->texturePosition() );
			
			QTransform t;
			t.scale( texture.resolution() , -texture.resolution() );
			_pTextureItem->setTransform( t );
		}
		
		_pTextureItem->setPixmap( QPixmap::fromImage( image ) );
	}
}

// ============================================================================
// Sets texture x position
void BodyWrapper::setTextureX( double d )
{
	QPointF newPos = _pBody->texturePosition();
	newPos.setX( d );
	_pBody->setTexturePosition( newPos );
	if ( _pTextureItem ) _pTextureItem->setPos( newPos );
}

// ============================================================================
// Sets texture y position
void BodyWrapper::setTextureY( double d )
{
	QPointF newPos = _pBody->texturePosition();
	newPos.setY( d );
	_pBody->setTexturePosition( newPos );
	if ( _pTextureItem ) _pTextureItem->setPos( newPos );
}

// ============================================================================
/// Is valid.
bool BodyWrapper::isValid() const
{
	QList<ShapeWrapper*> shapes = findChildren<ShapeWrapper*>();
	foreach( ShapeWrapper* pShape, shapes )
	{
		if ( ! pShape->isValid() )
		{
			return false;
		}
	}
	
	return true;
}

// ============================================================================
/// Syncs
void BodyWrapper::sync()
{
	QList<ShapeWrapper*> shapes = findChildren<ShapeWrapper*>();
	foreach( ShapeWrapper* pShape, shapes )
	{
		pShape->sync();
	}
}

// EOF

