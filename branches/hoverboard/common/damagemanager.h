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

#ifndef FLYERDAMAGEMANAGER_H
#define FLYERDAMAGEMANAGER_H

#include <QList>
#include <QMap>

namespace Flyer
{

class System;

/**
	Damage manager is object which lives associated to 
	Body objet. It is notified whern body is damaged.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class DamageManager
{
public:
	DamageManager();
	virtual ~DamageManager();
	
	/// Called when body to which the damage is associated receives damage.
	virtual void damage( double force );
	
	/// Called when body to which the damage manager is attached is destroyed.
	virtual void destroy();
	
	/// Adds system to damage manager. 
	void addSystem( System* pSystem, int weight, bool critical = false  );
	
private:

	QList<System*> _systems;
	QMap<System*, int> _criticalSystems;
};

}

#endif // FLYERDAMAGEMANAGER_H

// EOF


