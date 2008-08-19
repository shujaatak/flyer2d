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

#ifndef FLYERENGINE_H
#define FLYERENGINE_H

#include <QPointF>

#include "system.h"

namespace Flyer
{

/**
	This class impelemnts generic engine behavior.
	
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Engine : public System
{
public:
	Engine ( const QString& name, double thrust, const QPointF& normal = QPointF( 1.0, 0.0 ) );
	virtual ~Engine();

	virtual void damage ( double force );
	virtual void simulate ( double dt );
	virtual void render( QPainter& painter, const QRectF& rect );
	
	// properties
	
	void setThrottle( double throttle ); /// Sets throttle (0-1)
	double throttle() const { return  _throttle; }
	

private:

	QPointF thrustForce();	///< calculates thrust force
	
	// variables
	double _throttle;			///< Current throttle
	double _currentMaxThrust;	///< Current max thrust

	// params
	double	_maxThrust;		///< Max engine thrust
	QPointF	_normal;		///< _normal vector
};

}

#endif // FLYERENGINE_H

// EOF


