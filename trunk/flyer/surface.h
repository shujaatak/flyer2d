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

#ifndef FLYERSURFACE_H
#define FLYERSURFACE_H

#include <QPointF>

#include "system.h"

namespace Flyer {

/**
	Comon base clas for aerodynamic surfaces.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Surface : public System
{
public:
	Surface( Plane* pParent, const QString& name = "" );
	virtual ~Surface();
	
	// system duties
	
	virtual void damage(double arg1);
	virtual void render(QPainter& arg1, const QRectF& arg2);
	virtual void simulate(double arg1);
	
	// properties
	void setLiftCoeff( double lift );
	void setDragCoeffV( double drag );
	void setDragCoeffH( double drag );
	void setLength( double l ) { _length = l; }
	void setWidth( double w ) { _width = w; }
	void setInclination( double i ) { _inclination = i; }
	void setPosition( const QPointF& pos ) { _position = pos; }

protected:

	// methods
	
	/// Master force claculation. Returns force in world coordinates
	virtual QPointF aerodynamicForce() const;
	
	/// Calculates aerodynamic force
	virtual QPointF calculateForce( double velocity, double sinAttack ) const = 0;

	// config
	
	double	_width;			///< width [m]. Used only for rendering
	double	_length;		///< Length [m]. USed only for rendering
	double	_dragCoeffH;	///< Horizontal drag coefficient	[N/m2]
	double	_dragCoeffV;	///< Vertical drag coefficient	[N/m2]
	double	_liftCoeff;		///< Lift coefficient	[N/m2]
	double	_inclination;	///< Inclination angle	[radians]
	QPointF	_position;		///< Position, in body coordinates
	
	// variables
	
	double _currentDragH;
	double _currentDragV;
	double _currentLift;
};

}

#endif // FLYERSURFACE_H

// EOF


