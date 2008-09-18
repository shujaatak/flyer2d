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

#ifndef FLYERCONTROLSURFACE_H
#define FLYERCONTROLSURFACE_H

#include "surface.h"

namespace Flyer
{

/**
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class ControlSurface : public Surface
{
public:
	ControlSurface(Machine* pParent, const QString& name = "" );
	virtual ~ControlSurface();
	
	virtual void damage(double force);
	virtual void render(QPainter& painter, const QRectF& rect, const RenderingOptions& options );
	virtual double status() const;
	
	void setValue( double a );
	double angle() const { return _value * _step; }	///< Current angle [radians]
	double value() const { return _value; }
	
	void setStep( double s ) { _step = s; }

protected:

	virtual QPointF aerodynamicForce() const;
	virtual QPointF calculateForce(double velocity, double sinAttack) const;

	// config
	double	_step;	///< Actual max angle [radians]
	
	// variables
	double _value;			///< Current position [-1 - 1 ]
	double _currentMaxValue;
	double _currentMinValue;

};

}

#endif // FLYERCONTROLSURFACE_H

// EOF


