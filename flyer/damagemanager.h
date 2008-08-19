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

namespace Flyer
{

class System;

/**
	Damage manager is object which lives associated to 
	b2d shapes as user data. IT receives information about contacts.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class DamageManager
{
public:
	DamageManager( double tolerance = 0.0 );
	virtual ~DamageManager();
	
	/// Called when sape to which it's associated recives impuls due to contact
	virtual void contact( double force );
	
	/// Adds system to damage manager. 
	void addSystem( System* pSystem, int weight );
	
	// properties
	void setTolerance( double tol ) { _tolerance = tol; }
	double tolerance() const { return _tolerance; }

private:

	QList<System*> _systems;
	
	double _tolerance;	///< Tolerated force. all force above is propagated as damage to associated systems
};

}

#endif // FLYERDAMAGEMANAGER_H

// EOF


