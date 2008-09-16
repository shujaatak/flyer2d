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

#ifndef SHAPEWRAPPER_H
#define SHAPEWRAPPER_H

#include "shape.h"

#include "editablewrapper.h"

class EditableItem;

/**
 @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class ShapeWrapper : public EditableWrapper
{
	Q_OBJECT
	
	Q_PROPERTY( double Friction READ getFriction WRITE setFriction USER true );
	Q_PROPERTY( double Restitution READ getRestitution WRITE setRestitution USER true );
	Q_PROPERTY( double Density READ getDensity WRITE setDensity USER true );
	
public:
	ShapeWrapper( Flyer::Shape* pShape, QGraphicsScene* pScene, QObject* parent );
	virtual ~ShapeWrapper();

	// shape
	Flyer::Shape* shape() const { return _pShape; }

	// friction
	void setFriction( double f ) { _pShape->def()->friction = f; }
	double getFriction() const { return _pShape->def()->friction; }
	
	// restitution
	void setRestitution( double f ) { _pShape->def()->restitution = f; }
	double getRestitution() const { return _pShape->def()->restitution; }
	
	// density
	void setDensity( double f ) { _pShape->def()->density = f; }
	double getDensity() const { return _pShape->def()->density; }
	
	// vertices
	void addVertex();
	void removeVertex();

	virtual bool isValid() const;
	virtual void sync();
private:

	Flyer::Shape* _pShape;
};

#endif // SHAPEWRAPPER_H

// EOF


