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

#ifndef FLYERBULLET_H
#define FLYERBULLET_H

#include "worldobject.h"
#include "body.h"

namespace Flyer
{

/**
	This is a bullet. Small, simple object with short lifespan.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Bullet : public WorldObject
{
public:
	Bullet ( World* pWorld );
	virtual ~Bullet();

	virtual QRectF boundingRect();
	virtual void render ( QPainter& painter, const QRectF& rect );
	virtual void simulate ( double dt );

	// properties
	
	void setMass( double m ) { _mass = m; }
	void setSize( double s ) { _size = s; }
	void setLifespan( double l ) { _lifespan = l; }
	
	// actions
	
	/// Fires bullet from given point with given velocity
	void fire( const QPointF& point, const QPointF& velocity );

public:

	// config
	double _mass;		///< Mass [kg]
	double _size;		///< Size [m]
	double _lifespan;	///< Lifespan [seconds]
	
	// variables
	Body	_body;		///< Bullet's body
	double	_age;		///< Bullet's age
};

}

#endif // FLYERBULLET_H

// EOF


