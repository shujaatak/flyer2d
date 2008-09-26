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

#ifndef FLYERPASSIVEATTACHPOINT_H
#define FLYERPASSIVEATTACHPOINT_H

#include "attachpoint.h"

namespace Flyer
{

class ActiveAttachPoint;

/**
This is passive attach point. It's basicaly does nothing, only keeps references
to it's body and attached active joint.
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class PassiveAttachPoint : public AttachPoint
{
public:
	PassiveAttachPoint();
	~PassiveAttachPoint();

	// control mnessages from active point
	
	void setAttachedPoint( ActiveAttachPoint* p ) { _pAttachedPoint = p; }
	virtual void flip( const QPointF& p1, const QPointF& p2 );
	
private:

	ActiveAttachPoint* _pAttachedPoint;
};

}

#endif // FLYERPASSIVEATTACHPOINT_H

// EOF


