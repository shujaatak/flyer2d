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

#ifndef FLYERMACHINE_H
#define FLYERMACHINE_H

class QPainter;
#include <QMap>
#include <QList>
#include <QPointF>

#include "worldobject.h"

namespace Flyer
{

class System;
class Body;
class Joint;
class DamageManager;

/**
	Base class for machines. Machine is a set of systems, bodies and joints.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Machine : public WorldObject
{
public:
	Machine ( World* pWorld );
	virtual ~Machine();

	virtual void render ( QPainter& painter, const QRectF& rect );
	virtual void simulate ( double dt );
	
	// system classes and manipulation
	enum SystemType {				/// system type
		SystemSimulated	= 1,		///< System is simulated
		SystemRendered1	= 2,		///< System is rendered on layer 1
		SystemRendered2	= 4,		///< System is rendered on layer 1
		SystemRendered3	= 8			///< System is rendered on layer 1
		};

	void addSystem( System* pSystem, int types );
	void removeSystem( System* pSystem );
	
	// bodies classes and manipulation
	enum BodyType {					/// Body type
		BodyRendered1	= 1,		///< Render layer 1
		BodyRendered2	= 2,		///< Render layer 1
		BodyRendered3	= 4			///< Render layer 1
		};
		
	void addBody( Body* pBody, int types );
	void removeBody( Body* pBody );
	
	// joints
	void addJoint( Joint* pJoint );
	void removeJoint( Joint* pJoint );

	// damage managers
	void addDamageManager( DamageManager* pManager );
	void removeDamageManager( DamageManager* pManager );
	
	// geometrical operations
	
	/// Flip machine around axis of symmetry
	void flip( const QPointF& p1, const QPointF& p2 );
	
	/// Returns current orientation (flip) sign
	double orientation() const { return _orientation; }
	

protected:

	// TODO dhouldn't all these share common base class?

	// systems
	QMap<int, QList<System*> >	_systems;
	QList<System*> _allSystems;
	
	// bodies
	QMap<int, QList<Body*> > _bodies;
	QList<Body*>	_allBodies;
	
	// joints
	QList<Joint*>	_allJoints;
	
	// damage managers
	QList<DamageManager*> _allDamageManagers;
	
	// variables
	
	double _orientation;	//!< Orientation - 1 - normal, -1 - mirrored. 
};

}

#endif // FLYERMACHINE_H

// EOF


