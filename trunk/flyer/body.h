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
class QPainter;

#include "Box2D.h"

namespace Flyer
{

/**
	Wrap around b2Body. It rembeers it's shapes and know how to re-create itself.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Body
{
public:
	Body();
	virtual ~Body();
	
	/// Creates body
	void create( const b2BodyDef& def, b2World* pWorld );
	
	/// Retruns unrlying box2d body
	b2Body* b2body() const { return _pBody; }
	
	/// Adds shape to body
	void addShape( b2ShapeDef* pShapeDef );
	
	/// Returns body shape as painter path
	QPainterPath shape() const;
	
	/// Renders body in the simplest ways - draws it;s shape
	void render( QPainter& painter );
	
	/// Returns body position and orientation as QTranform
	QTransform transform() const;
	
	/// Flips body along defined axis
	void flip( const QPointF& p1, const QPointF& p2 );

private:

	b2Body*		_pBody;
	b2BodyDef	_definition;
	QList< b2ShapeDef*>	_shapeDefinitions;
};

}

#endif // FLYERBODY_H

// EOF


