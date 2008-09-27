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

#ifndef FLYERACTIVEATTACHPOINT_H
#define FLYERACTIVEATTACHPOINT_H

#include "attachpoint.h"

namespace Flyer
{

class PassiveAttachPoint;

/**
This is active attach point. It is master side of arrach point connection - it 
manages the joint that is used to attaching.
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class ActiveAttachPoint : public AttachPoint
{
public:
	ActiveAttachPoint();
	~ActiveAttachPoint();

	// config
	
	virtual void setParent( Machine* p );
	
	// operations
	
	void attach( PassiveAttachPoint* pPoint );
	void release();
	
	virtual void flip( const QPointF& p1, const QPointF& p2 );
	
private:

	PassiveAttachPoint*	_pAttachedPoint;
	
	Joint*		_pJoint;
	
};

}

#endif // FLYERACTIVEATTACHPOINT_H

// EOF


