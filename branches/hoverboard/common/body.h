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
class PhysicalObject;
class World;

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
	void create( const b2BodyDef& def, World* pWorld );
	
	/// Creates body (with pres-set def)
	void create( World* pWorld );
	
	/// Destroys physical representation
	void destroy();
	
	/// Creates copy of body
	virtual Body* createCopy() const;
	
	/// Retruns underlying box2d body
	b2Body* b2body() const { return _pBody; }
	
	
	World* world() const { return _pWorld; }		///< World in which the body exists
	
	/// Returns body definition. Usable only before creation.
	b2BodyDef& def() { return _definition; }
	
	/// Sets body position
	void setPosition( const b2Vec2& pos );
	
	b2Vec2 position() const;
	double angle() const;
	b2Vec2 velocity() const;
	double angularVelocity() const;
	
	/// Sets body angle
	void setAngle( double angle );
	
	/// Adds shape to body
	Shape* addShape( const Shape& shape, bool removeUserData  = false );
	
	/// Removes shape from body
	void removeShape( const Shape* shape );
	
	/// Returns list of shapes
	const QList<Shape>& shapes() const { return _shapes; }
	
	/// Returns reference to internal list of shapes
	QList<Shape>& shapes() { return _shapes; }
	
	/// Returns shape by name
	Shape* shapeByName( const QString& name );
	
	/// Returns body shape as painter path
	QPainterPath shape() const;
	
	/// Retruns approx/ body outline as polygon. In local coordinates
	QPolygonF outline() const;
	
	/// Returns body bounding rect
	QRectF boundingRect() const;
	
	/// Creates shape(s) fomr polygon.
	void setShape( const QPolygonF& shape, double friction, double restitution, double density );
	
	/// Renders body in the simplest ways - draws it;s shape
	void render( QPainter& painter, const RenderingOptions& options );
	
	/// Returns body position and orientation as QTranform
	QTransform transform() const;
	
	/// Flips body along defined axis
	void flip( const QPointF& p1, const QPointF& p2 );
	
	/// Returns current orientation
	double orientation() const { return _orientation; }
	
	/// Checks if body is connected to antoher through joints
	bool isConnectedTo( Body* pBody ) const;
	
	QString name() const { return _name; }
	void setName( const QString& s ) { _name = s; }
	
	/// Sets body's collision layers
	void setLayers( int layers );
	int layers() const { return _layers; }
	
	/// Sets texture
	void setTexture( const QString& path );
	QString texture() const { return _texturePath; }
	
	/// Sets texture's corner position
	void setTexturePosition( const QPointF& pos );
	const QPointF& texturePosition() const { return _texturePosition; }
	
	/// Sets 'limit to shape' flag
	void setLimitTextureToShape( bool b ) { _limitTextureToShape = b; }
	bool limitTextureToShap() const { return _limitTextureToShape; }
	
	PhysicalObject* parent() const { return _pParent; }
	void setParent( PhysicalObject* p ) { _pParent = p; }

	// simualtion
	
	void wakeUp();		///< Stars simulating body
	void sleep();		///< Stops simulating body
	
	/// Simualates body
	void simulate( double dt );

	// damage

	/// Associates damage manager with body
	void setDamageManager( DamageManager* pManager ){ _pDamageManager = pManager; }
	DamageManager* damageManager() const { return _pDamageManager; }
	
	void setDamageCapacity( double c ) { _damageCapacity = c; }
	double damageCapacity() const { return _damageCapacity; }
	
	void setDamageTolerance(  double t ) { _damageTolerance = t; }
	double damageTolerance() const { return _damageTolerance; }
	
	void setDamageMultiplier( double dm ) { _damageMultiplier = dm; }
	double damageMultiplier() const { return _damageMultiplier; }
	
	void setHeatCapacity( double h ) { _heatCapacity = h; }			///< Sets if body heats
	double heatCapacity() const { return _heatCapacity; }
	
	void setExplosionTemp( double d ){ _explosionTemp = d; }	///< Sets explosion temperature [k]
	double explosionTemp() const { return _explosionTemp; }		///< Sets explosion energy
	
	void setExplosionEnergy( double d ){ _explosionEnergy = d; }	///< Sets explosion temperature [k]
	double explosionEnergy() const { return _explosionEnergy; }		///< Sets explosion energy
	

	void contact( double force );		///< Information from physics engine - contact force
	void heat( double energy );			///< heats the body
	void breakBody();					///< Destroys body
	
	
protected:

	/// Puts object into stream
	virtual void toStream( QDataStream& stream ) const;
	/// Loads object from stream
	virtual void fromStream( QDataStream& stream );
	
	/// Provides ID which identifies object type
	virtual QString classId() const { return "Body"; }
	
private:

	// operations
	bool doIsConnectedTo( Body* pBody, QList<const Body*>& visited ) const;

	// config
	
	QString		_name;			///< Body name
	b2Body*		_pBody;
	b2BodyDef	_definition;
	QList<Shape>	_shapes;
	PhysicalObject*	_pParent;	///< Object which owns this body
	World*			_pWorld;
	
	// damage support
	DamageManager*	_pDamageManager;	///< Associated damage manager
	double			_damageCapacity;	///< How many damage body can take until broken
	double			_damageTolerance;	///< How big force is accepted w/o damaging body
	double			_damageMultiplier;	///< Multiplier of damage caused by this body
	double			_heatCapacity;		///< Heat capacity
	double			_explosionTemp;		///< Explosion temperature [K]
	double			_explosionEnergy;	///< Explosion energy
	
	// damage variables
	double			_damageReceived;		///< Damage creceived so far
	double			_temperature;		///< Current heat. Heat is a damage that cools over time. It couses smoke and fire!
	
	// variables
	int			_layers;		///< Collision layers
	double		_orientation;	///< Body orientation [-1 - flipped, +1 - not flipped]
	bool		_awake;			///< If body is awake (receives simulatin events)
	
	// texturing support
	
	Texture		_texture;
	QString		_texturePath;	///< Path to texture
	QPointF		_texturePosition;
	bool		_limitTextureToShape;	///< Cuts texture using shape.
};

}

#endif // FLYERBODY_H

// EOF


