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

#ifndef FLYERJOINT_H
#define FLYERJOINT_H

#include "Box2D.h"
#include <QPointF>

namespace Flyer
{

class Body;

/**
	Wrap around joint object.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Joint
{
public:
	Joint();
	virtual ~Joint();
	
	/// Creates joint
	void create( b2JointDef* pDef, b2World* pWorld, bool fix = false );
	
	/// Returns associated box2d object.
	b2Joint* b2joint() const { return _pJoint; }
	
	/// Set associated bodies
	void setBodies( Body* pBody1, Body* pBody2 );
	
	/// Flips joint along defined axis
	void flip( const QPointF& p1, const QPointF& p2 );
	
	/// Makes joint broken
	void breakJoint();
	
	/// Retuns associated body
	Body* body1() const { return _pBody1; }
	
	/// Retuns associated body
	Body* body2() const { return _pBody2; }
	
	/// Info fomr destruction listener
	void jointDestroyed();
	
	const b2JointDef* definition() const { return _pDefinition; }

private:

	b2JointDef* _pDefinition;
	b2Joint*	_pJoint;	///< b2d Joint object
	b2World*	_pWorld;	///< Cached world
	
	Body*		_pBody1;	///< Connected body
	Body*		_pBody2;	///< Connected body
	
	bool		_broken;	///< Flag: joint is broken
};

}

#endif // FLYERJOINT_H

// EOF


