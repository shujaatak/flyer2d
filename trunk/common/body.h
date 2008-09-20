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

#ifndef FLYERBODY_H
#define FLYERBODY_H

#include <QList>
#include <QPainterPath>
#include <QTransform>
#include <QString>
#include <QPixmap>
class QPainter;

#include "Box2D.h"

#include "texture.h"
#include "serializable.h"
#include "shape.h"


namespace Flyer
{

class RenderingOptions;

/**
	Wrap around b2Body. It rembeers it's shapes and know how to re-create itself.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Body : public Serializable
{
public:
	Body( const QString& name = "" );
	Body( const Body& src );
	virtual ~Body();
	
	/// Creates body
	void create( const b2BodyDef& def, b2World* pWorld );
	
	/// Creates body (with pres-set def)
	void create( b2World* pWorld );
	
	/// Destroys physical representation
	void destroy();
	
	/// Creates copy of body
	virtual Body* createCopy() const;
	
	/// Retruns underlying box2d body
	b2Body* b2body() const { return _pBody; }
	
	/// Returns body definition. Usable only before creation.
	b2BodyDef& def() { return _definition; }
	
	/// Sets body position
	void setPosition( const b2Vec2& pos );
	
	/// Sets body angle
	void setAngle( double angle );
	
	/// Adds shape to body
	Shape* addShape( const Shape& shape, bool removeUserData  = false );
	
	/// Returns list of shapes
	const QList<Shape>& shapes() const { return _shapes; }
	
	/// Returns reference to internal list of shapes
	QList<Shape>& shapes() { return _shapes; }
	
	/// Returns shape by name
	Shape* shapeByName( const QString& name );
	
	/// Returns body shape as painter path
	QPainterPath shape() const;
	
	/// Renders body in the simplest ways - draws it;s shape
	void render( QPainter& painter, const RenderingOptions& options );
	
	/// Returns body position and orientation as QTranform
	QTransform transform() const;
	
	/// Flips body along defined axis
	void flip( const QPointF& p1, const QPointF& p2 );
	
	/// Checks if body is connected to antoher through joints
	bool isConnectedTo( Body* pBody ) const;
	
	QString name() const { return _name; }
	void setName( const QString& s ) { _name = s; }
	
	/// Sets body's collision layers
	void setLayers( int layers );
	
	/// Sets texture
	void setTexture( const QString& path );
	QString texture() const { return _texturePath; }
	
	/// Sets texture's corner position
	void setTexturePosition( const QPointF& pos );
	const QPointF& texturePosition() const { return _texturePosition; }
	
	/// Sets texture scale [meters per pixel]
	void setTextureScale( double s );
	double textureScale() const { return _textureScale; }
	
protected:

	/// Puts object into stream
	virtual void toStream( QDataStream& stream ) const;
	/// Loads object from stream
	virtual void fromStream( QDataStream& stream );
	
	/// Provides ID which identifies object type
	virtual QString classId() const { return "Body"; }

private:

	// opoerations
	bool doIsConnectedTo( Body* pBody, QList<const Body*>& visited ) const;

	QString		_name;			///< Body name
	b2Body*		_pBody;
	b2BodyDef	_definition;
	QList<Shape>	_shapes;
	int			_layers;		///< Collision layers
	double		_orientation;	///< Body orientation [-1 - flipped, +1 - not flipped]
	
	// texturing support
	
	Texture		_texture;
	QString		_texturePath;	///< Path to texture
	QPointF		_texturePosition;
	double		_textureScale;
};

}

#endif // FLYERBODY_H

// EOF


