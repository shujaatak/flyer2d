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

#ifndef FLYERBUILDING_H
#define FLYERBUILDING_H

#include <QPainterPath>

#include "body.h"
#include "worldobject.h"

namespace Flyer
{

/**
Building is a static object, standing on the groud. Building cam be in foreground (where it collides with panes,
or background.
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Building : public WorldObject
{

public:
	Building ( World* pWorld );
	~Building();
	
	virtual QRectF boundingRect() const;
	virtual void render ( QPainter& painter, const QRectF& rect );
	
	double width() const { return _width; }
	double location() const { return _position.x; }
	
	static Building* createSmallBuilding( World* pWorld, double location, bool background );
	static Building* createLargeBuilding( World* pWorld, double location, bool background );

private:

	// appearance. replace with texture someday
	QPainterPath	_path;		///< Visual path. It should be texture in the future
	QColor			_color;		///< Color
	
	Body*			_pBody;			///< Building's body
	QRectF			_boundingRect;	///< Bounding rect
	
	b2Vec2			_position;		///< Buildong's position
	double			_width;
	bool			_background;
};

}

#endif // FLYERBUILDING_H

// EOF


