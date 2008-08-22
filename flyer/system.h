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



namespace Flyer
{

class Body;
class Machine;

/**
	Base class for all aircraft (and other) systems.
	Systems is an antity which provides functionality, control,
	instrument, acts with force and/or can be damaged.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class System
{
public:
	System( Machine* pParent, const QString& name = "" );
	virtual ~System();
	
	// properties
	
	QString name() const { return _name; }
	void setName( const QString& name ) { _name = name; }
	
	void setBody( Body* pBody ) { _pBody = pBody; }
	Body* body() const { return _pBody; }
	
	void setDamageCapacity( double dc ) { _damageCapacity = dc; }
	double damageCapacity() const { return _damageCapacity; }
	
	Machine* parent() const { return _pParent; }
	
	// actions
	
	virtual void render( QPainter& /*painter*/, const QRectF& /*rect*/ ){};
	virtual void simulate( double /*dt*/ ){};
	virtual void damage( double /*force*/ ){};
	

protected:

private:

	QString _name;				///< System's name
	Body*	_pBody;				///< _body associated with system
	double	_damageCapacity;	///< Amount of damage system can sustain before being comlletely useless
	Machine*	_pParent;		///< Parent
};

}

#endif // FLYERSYSTEM_H

// EOF


