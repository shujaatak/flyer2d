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

#include "joint.h"
#include "body.h"

namespace Flyer
{


// ============================================================================
// Constructor
Joint::Joint()
{
	_pDefinition = NULL;
	_pJoint = NULL;
	_pBody1 = NULL;
	_pBody2 = NULL;
	_pWorld = NULL;
	_broken = false;
}

// ============================================================================
// Destructor
Joint::~Joint()
{
	delete _pDefinition;
	// TODO destroy joint here?
}

// ============================================================================
/// Creates joint
/// If \b fix is true, creates joint even if it was broken before.
void Joint::create( b2JointDef* pDef, b2World* pWorld, bool fix /*= false*/  )
{
	Q_ASSERT( pDef && pWorld );
	
	_pWorld = pWorld;
	pDef->userData = this;
	
	// copy definition
	delete _pDefinition;
	if ( pDef->type == e_revoluteJoint )
	{
		b2RevoluteJointDef* pRevoluteDef = (b2RevoluteJointDef*)pDef;
		_pDefinition = new b2RevoluteJointDef( * pRevoluteDef );
	}
	else
	{
		qFatal("unknown joint type");
	}
	
	if ( fix ) _broken = false;
	
	if ( ! _broken )
	{
		_pJoint = pWorld->CreateJoint( pDef );
	}
	
}

// ============================================================================
/// Info fomr destructio listener: b2 joint was removed.
void Joint::jointDestroyed()
{
	_pJoint = NULL;
}

// ============================================================================
/// Flips body along defined axis.
/// Axis is defined as pair of points.
void Joint::flip( const QPointF& /*p1*/, const QPointF& /*p2*/ )
{
	if ( ! _pBody1 && ! _pBody2 ) return; // do nothing if joint not used
	
	Q_ASSERT( _pWorld );
	
	// create joint
	if ( ! _broken )
	{
		
		if ( _pDefinition->type == e_revoluteJoint )
		{
			b2RevoluteJointDef* pRevoluteDef = (b2RevoluteJointDef*)_pDefinition;
			// flip local anchor uspide-down
			b2Vec2 localAnchor;
			localAnchor.x = pRevoluteDef->localAnchor1.x;
			localAnchor.y = - pRevoluteDef->localAnchor1.y;
			
			pRevoluteDef->Initialize
				( _pBody1->b2body(), _pBody2->b2body()
				, _pBody1->b2body()->GetWorldPoint(localAnchor) );
		}
		else
		{
			qFatal("unsupportet joint type");
		}
	
		_pJoint = _pWorld->CreateJoint( _pDefinition );
	}
}


// ============================================================================
/// Set associated bodies. These bodies are not used immediately, but whren jointis recreated.
void Joint::setBodies( Body* pBody1, Body* pBody2 )
{
	_pBody1 = pBody1;
	_pBody2 = pBody2;
}

// ============================================================================
/// Makes joint broken. Broken joint is destroyed, and can't be recreated.
void Joint::breakJoint()
{
	_broken = true;
	if ( _pJoint && _pWorld )
	{
		// TODO check lock here somehow. Thi can't be called durig worl's simulation
		_pWorld->DestroyJoint( _pJoint );
		_pJoint = NULL;
	}
}

}
