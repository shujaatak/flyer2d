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

#ifndef FLYERWHEELBRAKE_H
#define FLYERWHEELBRAKE_H

#include "system.h"

namespace Flyer
{

class Joint;

/**
	Wheel brake. Applies braking torque to revolute joint.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class WheelBrake : public System
{
public:
	WheelBrake ( Plane* pParent, const QString& name = "" );

	virtual ~WheelBrake();

	virtual void damage ( double force );
	virtual void simulate ( double dt );
	
	void setOn( bool on ) { _on = on; }
	bool on() const { return _on; }
	
	void setBrakingTorque( double t ) { _brakingTorque = t; }
	void setJoint( Joint* pJoint ) { _pJoint = pJoint; }

private:

	// variables
	
	bool	_on;			///< Is brake on
	
	// config
	
	double	_brakingTorque;	///< Braking torque
	
	Joint*	_pJoint;		///< Revolute joint to brake
};

}

#endif // FLYERWHEELBRAKE_H

// EOF


