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

#ifndef FLYERANTIAIRBATTERY_H
#define FLYERANTIAIRBATTERY_H

#include "machine.h"

namespace Flyer
{

class Gun;
class Body;
class DamageManager;
class AntiAirGunOperator;

/**
	Simple anti-air gun which fires kalashnikov shells constantly
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class AntiAirBattery : public Machine
{
public:
	AntiAirBattery ( World* pWorld, double location, double angle );
	~AntiAirBattery();

	virtual void render ( QPainter& painter, const QRectF& rect, const RenderingOptions& options );
	virtual void renderOnMap( QPainter& painter, const QRectF& rect );

private:

	// components
	Body*	_bodyMain;
	Body*	_bodyBase;
	Gun*	_sysGun;
	DamageManager* _dmMain;
	AntiAirGunOperator* _sysOperator;
	double _lastDisplayedAngle;
};

}

#endif // FLYERANTIAIRBATTERY_H

// EOF


