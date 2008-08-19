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

#ifndef FLYERSYSTEM_H
#define FLYERSYSTEM_H

#include <QString>

class QPainter;
class QRectF;

class b2Body;
class b2Shape;


namespace Flyer
{

/**
	Base class for all aircraft (and other) systems.
	Systems is an antity which provides functionality, control,
	instrument, acts with force and/or can be damaged.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class System
{
public:
	System( const QString& name = "" );
	virtual ~System();
	
	// properties
	
	QString name() const { return _name; }
	void setName( const QString& name ) { _name = name; }
	
	void setBody( b2Body* pBody ) { _pBody = pBody; }
	b2Body* body() const { return _pBody; }
	
	void setDamageCapacity( double dc ) { _damageCapacity = dc; }
	double damageCapacity() const { return _damageCapacity; }
	
	// actions
	
	virtual void render( QPainter& /*painter*/, const QRectF& /*rect*/ ){};
	virtual void simulate( double /*dt*/ ){};
	virtual void damage( double /*force*/ ){};
	

protected:

	/// renders system's body
	void renderBody( QPainter& painter );
	
	// Renders shape
	void renderShape( QPainter& painter, b2Shape* pShape );
	
private:

	QString _name;				///< System's name
	b2Body*	_pBody;				///< _body associated with system
	double	_damageCapacity;	///< Amount of damage system can sustain before being comlletely useless
};

}

#endif // FLYERSYSTEM_H

// EOF


