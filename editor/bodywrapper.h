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

#ifndef BODYWRAPPER_H
#define BODYWRAPPER_H

#include <QGraphicsPixmapItem>

#include "editablewrapper.h"
class ShapeWrapper;

#include "body.h"

/**
	QObject wrapper around Body object, exposing it's internals as QObject's properties
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class BodyWrapper : public EditableWrapper
{
	Q_OBJECT

	Q_PROPERTY( QString Name READ getName WRITE setName USER true )
	
	Q_PROPERTY( double Mass READ getMass WRITE setMass USER true )
	Q_PROPERTY( double MassCenterX READ getMassCenterX WRITE setMassCenterX USER true )
	Q_PROPERTY( double MassCenterY READ getMassCenterY WRITE setMassCenterY USER true )
	Q_PROPERTY( double AngularMomentum READ getAngularMomentum WRITE setAngularMomentum USER true )

	Q_PROPERTY( double LinearDamping READ getLinearDamping WRITE setLinearDamping USER true )
	Q_PROPERTY( double AngularDamping READ getAngularDamping WRITE setAngularDamping USER true )

	Q_PROPERTY( bool IsBullet READ getIsBullet WRITE setIsBullet USER true )
	
	Q_PROPERTY( double TextureX READ getTextureX WRITE setTextureX USER true )
	Q_PROPERTY( double TextureY READ getTextureY WRITE setTextureY USER true )
	Q_PROPERTY( double TextureScale READ getTextureScale WRITE setTextureScale USER true )
	
	Q_PROPERTY( double DamageCapacityKN READ getDamageCapacity WRITE setDamageCapacity USER true )
	Q_PROPERTY( double DamageToleranceKN READ getDamageTolerance WRITE setDamageTolerance USER true )
	Q_PROPERTY( bool Heats READ getHeats WRITE setHeats USER true )

public:
	BodyWrapper( Flyer::Body *pBody, QGraphicsScene* pScene, QObject* parent = NULL );
	virtual ~BodyWrapper();

	// attached body
	Flyer::Body* body() const { return _pBody; }
	
	// shape management
	ShapeWrapper* addShape( b2ShapeDef* pDef );

	// name	
	QString getName() const { return _pBody->name(); }
	void setName( const QString& s ){ _pBody->setName( s ); }
	
	// mass center TODO
	double getMassCenterX() const { return _pBody->def().massData.center.x; }
	void setMassCenterX( double p ){ _pBody->def().massData.center.x = p; }
	double getMassCenterY() const { return _pBody->def().massData.center.y; }
	void setMassCenterY( double p ){ _pBody->def().massData.center.y = p; }
	void setAngularMomentum( double p ) { _pBody->def().massData.I = p; }
	double getAngularMomentum() const{ return _pBody->def().massData.I; }
	
	// mass
	double getMass() const { return _pBody->def().massData.mass; }
	void setMass( double m ){ _pBody->def().massData.mass = m; }
	
	// damping
	double getLinearDamping() const { return _pBody->def().linearDamping; }
	double getAngularDamping() const { return _pBody->def().angularDamping; }
	
	void setLinearDamping( double d ) { _pBody->def().linearDamping = d; }
	void setAngularDamping( double d ) { _pBody->def().angularDamping = d; }

	// bullet
	bool getIsBullet() const { return _pBody->def().isBullet; }
	void setIsBullet( bool b ) { _pBody->def().isBullet = b; }
	
	// texture
	void loadTexture( const QString& path );
	
	void setTextureX( double d );
	void setTextureY( double d );
	void setTextureScale( double d );
	
	double getTextureX() const { return  _pBody->texturePosition().x(); }
	double getTextureY() const { return  _pBody->texturePosition().y(); }
	double getTextureScale() const { return _pBody->textureScale(); }
	
	double getDamageCapacity() const { return _pBody->damageCapacity()/1000; }
	void setDamageCapacity( double d ) { _pBody->setDamageCapacity( d*1000 ); }
	double getDamageTolerance() const { return _pBody->damageTolerance()/1000; }
	void setDamageTolerance( double d ) { _pBody->setDamageTolerance( d*1000 ); }
	
	void setHeats( bool b ) { _pBody->setHeats(b); }
	bool getHeats() const { return _pBody->heats(); }

	virtual bool isValid() const;
	virtual void sync();
private:

	void showTexture( const QString& path );

	Flyer::Body* _pBody;
	QGraphicsPixmapItem* _pTextureItem;
};

#endif // BODYWRAPPER_H

// EOF


