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

class QPainter;
#include <QMap>
#include <QList>

#include "Box2D.h"

#include "worldobject.h"


namespace Flyer
{

class System;
class Engine;
class DamageManager;

/**
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Plane : public WorldObject
{
public:
	Plane( World* pWorld, const QPointF& pos, double angle );
	virtual ~Plane();
	
	void render( QPainter& painter, const QRectF& rect );
	void renderOnMap( QPainter& painter, const QRectF& rect );
	void simulate( double dt );
	
	void setThrottle( double t );
	double throttle() const;
	
	void setElevator( double e );
	double elevatorAngle() const;	///< Returns current elevator angle
	
	double airspeed();		///< Claculates airspeed
	
	QPointF pos() const; ///< Plane position
	
	void applyWheelBrake( bool on );
	bool wheelBrake() const { return _wheelBrake; }
	
	double flaps() const { return _flaps; }
	void setFlaps( double f );
	
	void flip();		///< Flips plane to the other side
	
	void setAutopilot( bool on );
	bool autopilot() const { return _autopilot; }
	
	// system manipulation
	enum SystemType {				/// system type
		SystemSimulated	= 1,		///< System is simulated
		SystemRendered1	= 2,		///< System is rendered on layer 1
		SystemRendered2	= 4,		///< System is rendered on layer 1
		SystemRendered3	= 8			///< System is rendered on layer 1
		};

	void addSystem( System* pSystem, int types );
	void removeSystem( System* pSystem );

private:

	void createBodies( const QPointF& pos, double angle );
	void createShapes( const QPointF& pos, double angle );

	static b2PolygonDef shapeToDef( const QPolygonF& shape, bool reversed );
	
	// simulation calculations
	
	QPointF wingsForce();	///< Calculates wings force
	QPointF elevatorForce();	///< Calculates elevator force
	QPointF elevatorPos();		///< Calculates elevator position
	
	void simulateAutopilot( double dt );

	// physical objects
	b2Body*	_pMainBody;
	b2Body*	_pMainWheel;
	b2Body* _pMainLeg;
	b2Body*	_pEngineCase;
	
	b2Shape* _pMainBodyShape;
	b2Shape* _pSkidShape;
	b2Shape* _pLegShape;
	b2Shape* _pWheelShape;
	b2Shape* _pEngineShape;
	
	b2RevoluteJoint* _pMainWheelJoint;
	b2RevoluteJoint* _pLegJoin;
	
	// shapes
	QPolygonF	_shapeBody;	// main hull body
	QPolygonF	_shapeSkid;	// skid and rudder
	QPolygonF	_shapeLeg;	// leg
	QPolygonF	_shapeEngine;	// engine
	
	// systems
	Engine*		_pEngine;
	
	// damage managers
	DamageManager* _pEngineDamageManager;
	
	// settings
	double _elevator;	///< Elevator angle (0-1)
	
	bool _wheelBrake;	//!< Flag - is whele brake on
	
	double _orientation;	//!< Orientation - 1 - normal, -1 - mirrored. 
	
	double _flaps;		///< Flaps: 0 - 1
	bool	_autopilot;	///< FLag - is autopilot on
	
	// autopilot params
	double _apPreviousError;
	double _apErrorIntegral;
	
	// systems
	QMap<int, QList<System*> >	_systems;
};

}

#endif // FLYERPLANE_H

// EOF


