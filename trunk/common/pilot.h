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

#ifndef FLYERPILOT_H
#define FLYERPILOT_H

#include <system.h>

namespace Flyer
{

/**
Pilot represent actual pilot. Be it player or AI.
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Pilot : public System
{

public:
	Pilot( Machine* pParent, const QString& name );
	virtual ~Pilot();

	virtual double status() const;
	virtual void damage( double force );
	virtual void destroy();
	virtual void repair();
	
private:

	double _health;			///< Pilot's health: 0-1

};

}

#endif // FLYERPILOT_H

// EOF


