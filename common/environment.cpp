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

#include "environment.h"

namespace Flyer
{

// ============================================================================
// Constructor
Environment::Environment()
{
	// nope
}

// ============================================================================
// Destructor
Environment::~Environment()
{
	// nope
}

// ============================================================================
/// Returns athmospheric pressure in Pa at secified location.
double Environment::pressure( const QPointF& location ) const
{
	return 101.3E3 - 7.18E3 * ( location.y() / 1000.0 ); // assumin 101.3 kPa at 0, and loss of 7.18 kPa per km
}

// ============================================================================
/// Returns relative density. 1.0 at sea level, decreasing with height. Used to calculating drag, lift and thrust.
double Environment::relativeDensity( const QPointF& location ) const
{
	return 1.0 - 0.08 * ( location.y() / 1000.0 );
}

// ============================================================================
/// Returns temperature in K at specified world point.
double Environment::temperature( const QPointF& location ) const
{
	return 300 - 10 * ( location.y() / 1000.0 ); // assuming 300 at 0 and 10 less per kilometer
}

}

// EOF

