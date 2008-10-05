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

#ifndef FLYERAIRFIELD_H
#define FLYERAIRFIELD_H

#include "worldobject.h"

namespace Flyer {

/**
	This class represents airfield's surface.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Airfield : public WorldObject
{
public:
	Airfield( World* pWorld, double x1, double x2 );
	
	virtual ~Airfield();
	
	virtual QRectF boundingRect() const;
	virtual void renderOnMap(QPainter& painter, const QRectF& rect );
	virtual void render(QPainter& painter, const QRectF& rect, const RenderingOptions& options );

private:

	double _x1, _x2, _y;	// physical coordnates
};

}

#endif // FLYERAIRFIELD_H

// EOF


