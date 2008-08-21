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

#ifndef FLYERWING_H
#define FLYERWING_H

#include "surface.h"

namespace Flyer
{

/**
	Wing. Aerodynamic surfasce with flaps.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Wing : public Surface
{
public:
	Wing ( Plane* pParent, const QString& name = "" );
	virtual ~Wing();

	virtual void damage ( double force );
	virtual void render( QPainter& painter, const QRectF& rect );
	
	// properties
	void setFlapsDrag( double fg ) { _flapsDrag = fg; }
	void setFlapsLift( double fl ) { _flapsLift = fl; }
	
	void setFlaps( double f );
	double flaps() const { return _flaps; }

protected:
	virtual QPointF calculateForce ( double velocity, double sinAttack ) const;
	
	// config
	double	_flapsDrag;			///< Extra drag when flaps are at max
	double	_flapsLift;			///< Extra lift when flaps are at max
	
	
	// variables
	double _flaps;				///< Flaps position 0-1
	double _currentFlapsMax;
	double _currentFlapsMin;

};

}

#endif // FLYERWING_H

// EOF


