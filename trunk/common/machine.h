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


#include "Box2D.h"

#include "message.h"

#include "physicalobject.h"

namespace Flyer
{

class System;
class DamageManager;
class ActiveAttachPoint;
class PassiveAttachPoint;

/**
	Base class for machines. Machine is a set of systems, bodies and joints.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Machine : public PhysicalObject
{
public:
	Machine ( World* pWorld );
	virtual ~Machine();

	virtual void render ( QPainter& painter, const QRectF& rect, const RenderingOptions& options );
	virtual void simulate ( double dt );
	virtual void flip( const QPointF& p1, const QPointF& p2 );
	
	// system classes and manipulation
	enum SystemType {				/// system type
		SystemSimulated	= 1,		///< System is simulated
		SystemRendered1	= 2,		///< System is rendered on layer 1
		SystemRendered2	= 4,		///< System is rendered on layer 1
		SystemRendered3	= 8			///< System is rendered on layer 1
		};

	void addSystem( System* pSystem, int types );
	void removeSystem( System* pSystem );
	const QList<System*>& systems() const { return _allSystems; }
	
	// damage managers
	void addDamageManager( DamageManager* pManager );
	void removeDamageManager( DamageManager* pManager );
	
	// attach points 
	
	/// Adds attach point to machine
	void addActiveAttachPoint( ActiveAttachPoint* pPoint );
	
	/// Adds attach point to machine
	void addPassiveAttachPoint( PassiveAttachPoint* pPoint );
	
	/// Returns machine's attach points
	QList<ActiveAttachPoint*>& activeAttachPoints() { return _activeAttachPoints; }
	
	// messages
	void addSystemMessage( const QString& );
	
	const QList<Message>& messages() const { return _messages; }

protected:

	
	// special designated items
	
	// systems
	QMap<int, QList<System*> >	_systems;
	QList<System*> _allSystems;
	
	
	// damage managers
	QList<DamageManager*> _allDamageManagers;
	
	// attach points
	QList<ActiveAttachPoint*>		_activeAttachPoints;
	QList<PassiveAttachPoint*>		_passiveAttachPoints;
	
	QList<Message> _messages;
};

}

#endif // FLYERMACHINE_H

// EOF


