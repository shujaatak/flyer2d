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

#ifndef FLYERATTACHPOINT_H
#define FLYERATTACHPOINT_H

#include <QString>

#include "Box2D.h"

#include "joint.h"

namespace Flyer
{

class Machine;

/**
Attach Point describes point to which one machine can attach another machines.
Attached machines are always in master-slave relationshsip, so there are two
kinds of attach points - passive and active
@see ActiveAttachPoint
@see PassiveAttachPoint
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class AttachPoint
{

public:
	AttachPoint();
	virtual ~AttachPoint();
	
	// config 
	
	/// Sets point position, in body coordinates
	void setPosition( const b2Vec2& pos ) { _position = pos; }
	/// Sets point angle, relative to body
	void setAngle( double a ) { _angle = a; }
	
	b2Vec2 position() const { return _position; }
	double angle() const { return _angle; }
	
	virtual void setParent( Machine* p );
	Machine* parent() const { return _pParentMachine; }
	
	void setParentBody( Body* p ){ _pParentBody = p; }
	Body* parentBody() const { return _pParentBody; }
	
	void setName( const QString& n ) { _name = n; }
	
	virtual void flip( const QPointF& p1, const QPointF& p2 ) = 0;
	virtual bool attached() const = 0;
	
protected:

	b2Vec2		_position;
	double		_angle;
	
	Machine*	_pParentMachine;	///< Parent machine
	Body*		_pParentBody;		///< Parent;'s body (optional, will use main body as default)
	
	QString		_name;

};

}

#endif // FLYERATTACHPOINT_H

// EOF


