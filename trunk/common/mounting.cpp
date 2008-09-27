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

#include "mounting.h"
#include "joint.h"
#include "machine.h"

namespace Flyer
{

// ============================================================================
// Constructor
Mounting::Mounting ( Machine* pParent, const QString& name, Joint* pJoint /*= NULL*/, double tolerance /*= 0.0*/, double capacity /*= 0.0*/ )
	: System ( pParent, name )
{
	_tolerance = tolerance;
	_pJoint = pJoint;
	setDamageCapacity( capacity );
	
	_damageReceived = 0;
	_broken = false;
	
}

// ============================================================================
// Destructor
Mounting::~Mounting()
{
}

// ============================================================================
/// Handles damage
void Mounting::damage ( double damage )
{
	_damageReceived += damage;
	//qDebug("damage left: %g", damageCapacity() - _damageReceived );
	if ( ! _broken &&  _damageReceived > damageCapacity() && _pJoint && parent() )
	{
		parent()->breakJoint( _pJoint );
		_broken = true;
		
	}
}

// ============================================================================
/// Retuens mounting status
double Mounting::status() const
{
	return qMax( 0.0, 1.0 - _damageReceived/ damageCapacity() );
}

// ============================================================================
/// Handles simulation step
void Mounting::simulate ( double dt )
{
	if ( _pJoint && _pJoint->b2joint() )
	{
		double forceTime = dt * _pJoint->b2joint()->GetReactionForce().Length();
		if ( forceTime > _tolerance*dt )
		{
			damage( forceTime );
		}
	}
}

}
