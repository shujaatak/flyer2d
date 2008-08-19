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

#ifndef FLYERWORLD_H
#define FLYERWORLD_H

#include <QPainter>
#include <QList>

#include "worldobject.h"

class b2World;
class b2Body;

namespace Flyer {

class Plane;
class Ground;

/**
	Main world object. Holds Box2d world, and controls simulation.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class World
{
public:
	World();
	virtual ~World();
	
	/// Renders world
	void render( QPainter& painter, const QRectF& rect );
	
	/// Renders map of the world
	void renderMap( QPainter& painter, const QRectF& rect );
	
	/// Simlation step
	void simulate( double dt );
	
	/// Returns box2d world
	b2World* b2world() { return _pb2World; }
	
	/// Returns world boundary
	QRectF boundary() const { return _boundary; }
	
	/// Returns player's plane
	Plane* playerPlane() const { return _pPlayerPlane; }
	
	
private:
	
	void initWorld();

	QList<WorldObject*> _objects;	///< List of objects
	
	b2World*	_pb2World;			///< Box2d world
	Plane*	_pPlayerPlane;			///< Player's plane
	Ground*	_pGround;				///< Ground body
	QRectF	_boundary;				///< World boundary
};

}

#endif // FLYERWORLD_H

// EOF


