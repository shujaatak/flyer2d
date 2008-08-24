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

#ifndef FLYERSHRAPNEL_H
#define FLYERSHRAPNEL_H

#include <worldobject.h>

namespace Flyer
{

class Body;

/**
	Shrapnel is an short-lived objects that is created as a result of bodies being
	detached or destroyen in explosion.
	Shrapnel maintains list of it's bodies.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Shrapnel : public WorldObject
{

public:
	Shrapnel( World* pWorld );
	~Shrapnel();
	
	/// Renders object
	virtual void render( QPainter& painter, const QRectF& rect );
	
	/// Simualtes object
	virtual void simulate( double dt );
	
	// config
	
	void setLifespan( double l ) { _lifespan = l; }
	void addBody( Body* pBody ){ _bodies.append( pBody ); }

private:

	// config
	double _lifespan;	///< Maximum lifespan

	// variables
	QList<Body*> _bodies;
	
	double	_age;

};

}

#endif // FLYERSHRAPNEL_H

// EOF


