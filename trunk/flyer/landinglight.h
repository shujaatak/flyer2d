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

#ifndef FLYERLANDINGLIGHT_H
#define FLYERLANDINGLIGHT_H

#include "worldobject.h"

class b2Body;

namespace Flyer {

/**
	Landing light which aids pilot in approaching airport.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class LandingLight : public WorldObject
{
public:
	LandingLight( World* pWorld, double x, double y, double angle );
	virtual ~LandingLight();
	
	virtual QRectF boundingRect();
	virtual void render(QPainter& painter, const QRectF& rect);

private:

	b2Body* _pBody;

	double _x, _y, _angle; ///< Coordinates and angle [radians]
	double _range;	///< Light range
	double _width;	///< Light stream width (radians)
};

}

#endif // FLYERLANDINGLIGHT_H

// EOF


