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

#ifndef FLYERSPOTLIGHT_H
#define FLYERSPOTLIGHT_H

#include "system.h"

namespace Flyer {

/**
	Spotlight is a light, which emits cone of coloured light from attached body.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Spotlight : public System
{
public:
	Spotlight( Machine* pParent, const QString& name = "" );
	
	virtual ~Spotlight();
	
	virtual void damage(double force);
	virtual void render(QPainter& painter, const QRectF& rect, const RenderingOptions& options );

	// properties
	
	void setRange( double r );
	void setWidth( double w ) { _width = w; }
	void setColor( const QColor& color ) { _color = color; }
	void setPosition( const QPointF& pos ) { _position = pos; }
	void setAngle( double a ) { _angle = a; }
	
	double angle() const { return _angle; }
	QPointF position() const { return _position; }
	
	// operations
	void setOn( bool on ) { _on = on; }
	bool on() const { return _on; }
	
	double currentRange() const { return _currentRange; }

private:

	// config
	double	_range;
	double	_width;		///< cone with, in radians
	QColor	_color;		///< Light color
	QPointF	_position;	///< Position within attached body
	double	_angle;		///< Angle relative to body's angle [radians]

	// variables
	double	_currentRange;
	bool	_on;
};

}

#endif // FLYERSPOTLIGHT_H

// EOF


