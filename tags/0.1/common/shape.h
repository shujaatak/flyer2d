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

#ifndef FLYERSHAPE_H
#define FLYERSHAPE_H

#include "Box2D.h"

#include "serializable.h"

namespace Flyer
{

class DamageManager;
class Body;

/**
	Wrapper around b2 shape definition.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Shape : public Serializable
{

public:
	Shape( b2ShapeDef* pDef = NULL );
	Shape( const Shape& src );
	virtual ~Shape();
	
	/// Returns associated definition	
	b2ShapeDef* def() const{ return _pDef; }
	
	/// Creates shape
	void create( Body* pBody );
	
	/// Returns associated b2d object
	b2Shape* b2shape() const { return _pShape; }
	
	/// flips shape upside-down
	void flip();
	
	// name
	
	QString name() const { return _name; }
	void setName( const QString& n ) { _name = n; }
	
	void setDamageManager( DamageManager* pManager );	///< Associates damage manager with shape

protected:

	/// Puts object into stream
	virtual void toStream( QDataStream& stream ) const;
	/// Loads object from stream
	virtual void fromStream( QDataStream& stream );
	
	/// Provides ID which identifies object type
	virtual QString classId() const { return "Shape"; }

private:

	static b2ShapeDef* createDefCopy( b2ShapeDef* );

	b2ShapeDef*	_pDef;		///< B2 shape definition
	QString		_name;		///< Name
	b2Shape*	_pShape;	///< B2D object
};

}

#endif // FLYERSHAPE_H

// EOF


