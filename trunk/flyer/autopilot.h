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

#ifndef FLYERAUTOPILOT_H
#define FLYERAUTOPILOT_H

#include <system.h>

namespace Flyer
{

class ControlSurface;

/**
	Simple autopilot. Steers elevator using simple PID controller
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Autopilot : public System
{
public:
	Autopilot ( Machine* pParent, const QString& name = "" );
	virtual ~Autopilot();

	virtual void damage ( double force );
	virtual void simulate ( double dt );
	
	void setElevator( ControlSurface* pElevator ) { _pElevator = pElevator; }
	void setPID( double p, double i, double d ) { _P = p; _I = i; _D = d; }
	
	void setOn( bool on ) { _on = on; }
	bool on() const { return _on; }
	
private:

	// variables
	
	double _previousError;
	double _errorIntegral;
	bool	_on;			///< Flag - is turned on?
	
	// config
	
	ControlSurface* _pElevator;		///< Controlled elevator
	double _P, _I, _D;
};

}

#endif // FLYERAUTOPILOT_H

// EOF


