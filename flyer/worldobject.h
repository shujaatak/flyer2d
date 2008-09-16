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

#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include <QPainter>

namespace Flyer {

class World;

/**
	Common base class for objects managed by world.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class WorldObject
{
public:
	WorldObject( World* pWorld );
	virtual ~WorldObject();

	/// Renders object
	virtual void render( QPainter& /*painter*/, const QRectF& /*rect*/ ){};
	
	/// Renders object on map
	virtual void renderOnMap( QPainter& /*painter*/, const QRectF& /*rect*/ ){};
	
	/// Returns object's bounding rect
	virtual QRectF boundingRect() const { return QRectF(); }
	
	/// Simualtes object
	virtual void simulate( double /*dt*/ ){}
	
	/// Returns parent world
	World* world() const { return _pWorld; }
	
private:

	World* _pWorld;			///< Parent world
};

}

#endif // WORLDOBJECT_H

// EOF


