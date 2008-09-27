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

#ifndef FLYERENVIRONMENT_H
#define FLYERENVIRONMENT_H

#include <QPointF>

namespace Flyer
{

/**
	This class provides environment data - pressure, air density, temperature etc.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Environment
{
public:
	Environment();
	virtual ~Environment();
	
	double pressure( const QPointF& location ) const;				///< Pressure [Pa]
	double relativeDensity( const QPointF& location ) const;		///< Desnity [relative units, 1->0]
	double temperature( const QPointF& location ) const;			///< Temperature [k]
};

}

#endif // FLYERENVIRONMENT_H

// EOF


