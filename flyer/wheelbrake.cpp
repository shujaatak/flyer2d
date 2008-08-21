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

#include "wheelbrake.h"
#include "joint.h"

namespace Flyer
{

// ============================================================================
// Constructor
WheelBrake::WheelBrake ( Plane* pParent, const QString& name ) : System ( pParent, name )
{
	_pJoint = NULL;
	_on = false;
}

// ============================================================================
// estructor
WheelBrake::~WheelBrake()
{
}

// ============================================================================
// Damages brake
void WheelBrake::damage ( double firce )
{
	qDebug("TODO implement wheel brake damage");
}

// ============================================================================
// Simulates brake
void WheelBrake::simulate ( double dt )
{
	if ( _pJoint && _pJoint->b2joint() && _pJoint->b2joint()->GetType() == e_revoluteJoint )
	{
		b2RevoluteJoint* pRevoluteJoint = static_cast<b2RevoluteJoint*>( _pJoint->b2joint() );
		if ( _on )
		{
			pRevoluteJoint->SetMaxMotorTorque( _brakingTorque );
			pRevoluteJoint->SetMotorSpeed( 0.0 );
		}
		else
		{
			pRevoluteJoint->SetMaxMotorTorque( 0.0 );
		}
	}
}

}
