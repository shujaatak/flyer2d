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

#include "pilot.h"

namespace Flyer
{

// ============================================================================
// Constructor
Pilot::Pilot( Machine* pParent, const QString& name ): System( pParent, name )
{
	_health = 1.0;
	setDamageCapacity( 200E3 );
}

// ============================================================================
// Destructor
Pilot::~Pilot()
{
}

// ============================================================================
// Pilot's health
double Pilot::status() const
{
	return _health;
}

// ============================================================================
// Damages piloy
void Pilot::damage( double force )
{
	_health = qMax( 0.0, _health - force/damageCapacity() );
}

// ============================================================================
/// Kills pilot
void Pilot::destroy()
{
	_health = 0.0;
}

// ============================================================================
// Heals pilot
void Pilot::repair()
{
	_health = 1.0;
}

}
