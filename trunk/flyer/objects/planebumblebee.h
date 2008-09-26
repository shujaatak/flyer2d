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

#ifndef FLYERPLANEBUMBLEBEE_H
#define FLYERPLANEBUMBLEBEE_H


#include "Box2D.h"

#include "Box2D.h"

#include "plane.h"

namespace Flyer
{

class Mounting;
class IronBomb;

/**
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class PlaneBumblebee : public Plane
{
public:
	PlaneBumblebee( World* pWorld, const QPointF& pos, double angle );
	~PlaneBumblebee();

	virtual void render( QPainter& painter, const QRectF& rect, const RenderingOptions& options );

private:

	void createSystems();
	void createBodies( const QPointF& pos, double angle );
	void createDamageManagers();

	// type -specific data
	
	// bodies 
	Body* _bodyHull;
	Body* _bodyEngine;
	Body* _bodyLeg;
	Body* _bodyWheel;
	Body* _bodyTail;
	
	// joints
	Joint* _jointWheel;
	Joint* _jointEngine;
	Joint* _jointLeg;
	Joint* _jointTail;
	
	// systems
	Engine*			_sysEngine;
	Mounting*		_sysWheelMounting;
	Wing*			_sysWing;
	ControlSurface*	_sysElevator;
	WheelBrake*		_sysBrake;
	Autopilot*		_sysAutopilot;
	Gun*			_sysGun;
	
	// damage managers
	DamageManager*	_dmEngine;
	DamageManager*	_dmWheel;
	DamageManager*	_dmLeg;
	DamageManager*	_dmHull;
	DamageManager*	_dmTail;
	
	// attached machines
	ActiveAttachPoint* _pointBomb;
	
};

}

#endif // FLYERPLANEBUMBLEBEE_H

// EOF


