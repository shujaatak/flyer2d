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

#ifndef FLYERWORLD_H
#define FLYERWORLD_H

#include <QPainter>
#include <QList>
#include <QMap>
#include <QLinkedList>
#include <QLinearGradient>

#include "worldobject.h"
#include "environment.h"
#include "ground.h"

class b2World;
class b2Body;
class b2BroadPhase;

namespace Flyer {

class Plane;
class Ground;
class Machine;
class Pilot;

/**
	Main world object. Holds Box2d world, and controls simulation.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class World
{
public:
	
	/// Object classes
	enum ObjectType {
		// general properties
		ObjectSimulated				= 0x000001,
		ObjectStatic				= 0x000002,
		
		ObjectRenderedMap			= 0x000080,		///< Object is rendered in the minimap
		
		// class
		ObjectPlane					= 0x000080,
		ObjectInstallation			= 0x000100,
		ObjectVehicle				= 0x000200,
		ObjectAirfield				= 0x000400,
		
		// confilict side
		ObjectSide1					= 0x010000,
		ObjectSide2					= 0x020000
		
		// TODO others here
	};
	

	World( const QRectF& boundary );
	virtual ~World();

	// initialization
	
	void initRandomGround( const QList<Ground::Section>& seed );
	void setPlayer( Pilot* pPilot );
	
	// other

	/// Renders world
	void render( QPainter& painter, const QRectF& rect );
	
	/// Renders map of the world
	void renderMap( QPainter& painter, const QRectF& rect );
	
	/// Simlation step
	void simulate( double dt );
	
	/// Returns box2d world
	b2World* b2world() { return _pb2World; }
	
	/// Returns world boundary
	QRectF boundary() const { return _boundary; }
	
	/// Returns player
	Pilot* player() const { return _pPlayer; }
	
	/// Returns player's plane
	Plane* playerPlane() const;
	
	/// Return environment
	const Environment* environment() const { return & _environment; }
	
	/// Returns ground object
	const Ground* ground() const { return _pGround; }
	
	/// Adds object to the world
	void addObject( WorldObject* pObject, int objectClass );
	
	/// Removes object from world
	void removeObject( WorldObject* pObject, bool destroy = true );
	
	/// Returns timestep used in simulation
	double timestep() const;
	
	/// Returns current simulaton time
	double time();
	
	/// Adds object to 1-second timer.
	void addToTimer1( WorldObject* pObject ) { _timer1Objects.append( pObject ); }
	
	// querying
	
	/// Finds machines in specified area
	QList<Machine*> findMachines( const QRectF& area, int types ) const;
	
	
	// decoration
	
	/// Registers object in decoration database, using its bounding rect.
	void addDecoration( WorldObject* pObject );
	
	/// Changes decoration position in brodphase
	void decorationMoved( WorldObject* pObject );
	
	/// Removes object from decorations broadphase
	void removeDecoration( WorldObject* pObject );
	
private:
	
	void initWorld();

	QList<WorldObject*> _allObjects;	///< List of objects
	QList<WorldObject*> _timer1Objects;	///< Objects connected to 1-second timer.
	
	/// Categorized lists of objects
	QMap< int, QList<WorldObject*> >	_objects;
	
	b2World*	_pb2World;			///< Box2d world
	Pilot*	_pPlayer;				///< Player
	Ground*	_pGround;				///< Ground body
	QRectF	_boundary;				///< World boundary
	Environment	_environment;		///< Environment data
	b2BroadPhase*	_pDecorationBroadPhase;	///< Spatal database of non-physical objects
	bool	_decorationsDirty;		///< Flag - decroatrion broadphase was modified  and should be commited.
	
	/// List of objects to be destroyed during next simulation stet
	QLinkedList<WorldObject*> _objectsToDestroy;
	
	void renderAthmosphere( QPainter& painter, const QRectF& rect );
	QLinearGradient	_skyGradient;	///< sky gradient (experimental)
	
	int		_steps;					///< Simulation steps so far
	int		_renders;				///< Renders so far
	double	_timer1Time;			///< Time from lats 1-second timer event
	double	_lastKnownHealth;		///< Last known player's health.
	
};

}

#endif // FLYERWORLD_H

// EOF


