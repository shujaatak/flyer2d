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

#ifndef FLYERMOUNTING_H
#define FLYERMOUNTING_H

#include <system.h>

namespace Flyer
{

class Joint;

/**
	It's a pseudo system, which - when attached to joint - breaks it when too big force
	acts upon for too long time;
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Mounting : public System
{
public:
	Mounting ( const QString& name = "", Joint* pJoint = NULL, double tolerance = 0.0, double capacity = 0.0 );
	virtual ~Mounting();

	virtual void damage ( double force );
	virtual void simulate ( double dt );
	
	// properties
	void setJoint( Joint* pJoint ) { _pJoint = pJoint; }
	
	void setTolerance( double t ) { _tolerance = t; }

private:

	double	_tolerance;		///< Damage tolerance threshold
	Joint*	_pJoint;		///< Attached joint
	double _damageReceived;	///< Damage received so far
};

}

#endif // FLYERMOUNTING_H

// EOF

