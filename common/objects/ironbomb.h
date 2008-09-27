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

#ifndef FLYERIRONBOMB_H
#define FLYERIRONBOMB_H

#include "machine.h"

namespace Flyer
{

class ContactFuse;
class DamageManager;
class Body;
class Surface;
class PassiveAttachPoint;

/**
	Simple iron bomb wthconfigurable sisze, mass and explosion energy.
 @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class IronBomb : public Machine
{

public:
	IronBomb ( World* pWorld );
	virtual ~IronBomb();
	
	void setLength( double l ) { _length = l; }
	void setDiameter( double d ) { _diameter = d; }
	void setMass( double m ) { _mass = m; }
	void setEnergy( double e ) { _energy = e; }
	
	PassiveAttachPoint* atachPoint() const { return _attachPoint; }
	
	void init( const QPointF& position, double angle );
	
private:

	// config
	double _length;
	double _diameter;
	double _mass;
	double _energy;
	
	// components
	
	ContactFuse*	_sysFuse;
	DamageManager*	_dmFuse;
	Body*			_bodyMain;
	Surface*		_sysStabilizer;
	PassiveAttachPoint*	_attachPoint;

};

}

#endif // FLYERIRONBOMB_H

// EOF


