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

#ifndef FLYERPLANE_H
#define FLYERPLANE_H


#include "Box2D.h"

#include "machine.h"
namespace Flyer
{

class System;
class Engine;
class DamageManager;
class Wing;
class WheelBrake;
class Autopilot;
class ControlSurface;
class Gun;

/**
	Plane is a special machine, that has fed designated, aircraft-specific systems,
	available by dedicated interface, like: winfs, engine, elevator, brake.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Plane : public Machine
{
public:
	Plane( World* pWorld, const QPointF& pos, double angle );
	virtual ~Plane();
	
	virtual void renderOnMap( QPainter& painter, const QRectF& rect );
	
	void setThrottle( double t );
	double throttle() const;
	
	void setElevator( double e );
	double elevatorAngle() const;	///< Returns current elevator angle
	
	double airspeed();		///< Claculates airspeed
	
	void applyWheelBrake( bool on );
	bool wheelBrake() const;
	
	double flaps() const;
	void setFlaps( double f );
	
	void flipPlane();		///< Flips plane to the other side or turns around
	
	void setAutopilot( bool on );
	bool autopilot() const;
	
	void setFiring( bool on );
	
	// temporry bomb interface
	virtual void releaseBomb(){}
	
	// systems
	void setAutopilot( Autopilot* pAutopilot ){ _pAutopilot = pAutopilot; }
	void setEngine( Engine* pEngine ) { _pEngine = pEngine; }
	void setWheelBrake( WheelBrake* pBrake ) { _pBrake = pBrake; }
	void setWing( Wing* pWing ) { _pWing = pWing; }
	void setElevator( ControlSurface* pElevator ) { _pElevator = pElevator; }
	void setGun( Gun* pGun ) { _pGun = pGun; }
	
private:

	// special designated systems
	Engine* 		_pEngine;
	Wing*			_pWing;
	WheelBrake*		_pBrake;
	Autopilot*		_pAutopilot;
	ControlSurface*	_pElevator;
	Gun*			_pGun;
	
};

}

#endif // FLYERPLANE_H

// EOF


