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

#include "passiveattachpoint.h"
#include "machine.h"
#include "body.h"

#include "activeattachpoint.h"

namespace Flyer
{

// ============================================================================
// Constructor
ActiveAttachPoint::ActiveAttachPoint()
		: AttachPoint()
{
	_pJoint = NULL;
	_pAttachedPoint = NULL;
}

// ============================================================================
// Destructor
ActiveAttachPoint::~ActiveAttachPoint()
{
	// NOTE: don't delete the joint. It will be deleted by parent body
}

// ============================================================================
/// Sets parent machine, adds own joint to it's joint list
void ActiveAttachPoint::setParent( Machine* p )
{
	AttachPoint::setParent( p );
	
	_pJoint = new Joint();
	
	//p->addJoint( _pJoint ); TODO experimental
}

// ============================================================================
// Attaches another body via attach point
void ActiveAttachPoint::attach( PassiveAttachPoint* pPoint )
{
	Q_ASSERT( pPoint );
	Q_ASSERT( pPoint->parent() );
	Q_ASSERT( _pParentMachine );
	
	// OK, let's do the hard stuff
	
	// let passive point know that it's connected
	pPoint->setAttachedPoint( this );
	
	// get bodies
	Body* pMasterBody	= parentBody() ? parentBody() : _pParentMachine->mainBody();
	Body* pSlaveBody	= pPoint->parentBody() ? pPoint->parentBody() : pPoint->parent()->mainBody();
	
	// bodies have to be created TODO: I dont like this requirement
	Q_ASSERT( pMasterBody->b2body() );
	Q_ASSERT( pSlaveBody->b2body() );
	
	// sync orientation
	if ( pMasterBody->orientation() * pSlaveBody->orientation() < 0 )
	{
		PhysicalObject* pSlaveMachine = pSlaveBody->parent();
		if ( pSlaveMachine )
		{
			double x = pSlaveMachine->position().x();
			pSlaveMachine->flip( QPointF( x, 1 ), QPointF( x, -1 ) );
		}
	}
	
	// find connection's world point and angle
	b2Vec2 position( _position.x, _position.y * pMasterBody->orientation() );
	b2Vec2 worldPoint = pMasterBody->b2body()->GetWorldPoint( position );
	double worldAngle = pMasterBody->b2body()->GetAngle() + _angle * pMasterBody->orientation();
	
	// move slave body so it's passive point coords overlap with world coords.
	double targetAngle	= worldAngle - pPoint->angle() * pSlaveBody->orientation();
	b2Vec2 passivePos	= b2Vec2( pPoint->position().x, pPoint->position().y * pSlaveBody->orientation() );
	b2Vec2 tagertPos	= worldPoint - b2Mul( b2Mat22( targetAngle ), passivePos );
	
	pSlaveBody->b2body()->SetXForm( tagertPos,  targetAngle );
	
	// now, create a joint
	b2RevoluteJointDef def;
	def.Initialize( pMasterBody->b2body(), pSlaveBody->b2body(), worldPoint );
	def.enableLimit = true;
	def.upperAngle = 0.0;
	def.lowerAngle = 0.0;
	
	_pJoint->create( & def, pMasterBody->b2body()->GetWorld(), true );
	_pJoint->setBodies( pMasterBody, pSlaveBody );
	
	// and finally - take business card from this guy
	_pAttachedPoint = pPoint;
	
}

// ============================================================================
// Detaches attached body.
void ActiveAttachPoint::release()
{
	if ( _pJoint && _pJoint->b2joint() )
	{
		_pJoint->breakJoint();
		_pJoint->setBodies( NULL, NULL );
		
		_pAttachedPoint = NULL;
	}
}

// ============================================================================
// Flips attached machine around specified axis.
void ActiveAttachPoint::flip( const QPointF& p1, const QPointF& p2 )
{
	if ( _pAttachedPoint ) _pAttachedPoint->flip( p1, p2 );
	if ( _pJoint ) _pJoint->flip( p1, p2 );
}

}
