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

#include <QList>
#include <QPainter>

namespace Flyer {

class World;
class RenderingOptions;

/**
	Common base class for objects managed by world.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class WorldObject
{
public:
	WorldObject( WorldObject* pParent );
	WorldObject( World* pWorld );
	virtual ~WorldObject();

	/// Renders object
	virtual void render( QPainter& /*painter*/, const QRectF& /*rect*/, const RenderingOptions& /*options*/ ){};
	
	/// Renders object on map
	virtual void renderOnMap( QPainter& /*painter*/, const QRectF& /*rect*/ ){};
	
	/// Returns object's bounding rect
	virtual QRectF boundingRect() const { return QRectF(); }
	
	/// Simualtes object
	virtual void simulate( double /*dt*/ ){}
	
	/// Returns world
	World* world() const { return _pWorld; }
	
	/// Returns Z-coordinate
	int renderLayer() const { return _renderLayer; }
	
	/// Sets render layere
	void setRenderLayer( int z ) { _renderLayer = z; }
	
	
	// chilsren / parent relationship
	
	/// Returns parent object
	WorldObject* parent() const { return _pParent; }
	
	QList<WorldObject*> children() const { return _children; }
	
private:

	World* _pWorld;					///< World
	WorldObject*	_pParent;		///< Parent world object [optional, NULL for top-level objects]
	QList<WorldObject*>	_children;	///< Children objects
	
	int _renderLayer;							///< Z-coordinate, used for ordering painting
};

}

#endif // WORLDOBJECT_H

// EOF


