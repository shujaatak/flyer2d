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

#include "machine.h"
#include "system.h"
#include "body.h"
#include "joint.h"
#include "damagemanager.h"
#include "b2dqt.h"
#include "shrapnel.h"
#include "world.h"

namespace Flyer
{

// ============================================================================
// Constructor
Machine::Machine ( World* pWorld ) : WorldObject ( pWorld )
{
	_orientation = 1.0;
}

// ============================================================================
// Destructor
Machine::~Machine()
{
	// delete all
	foreach( DamageManager* pDM, _allDamageManagers )
	{
		//qDebug("deleting dm: %p", pDM );
		delete pDM;
	}
	
	foreach( System* pSystem, _allSystems )
	{
		//qDebug("deleting system: %p (%s)", pSystem, qPrintable( pSystem->name() ) );
		delete pSystem;
	}
	
	foreach ( Body* pBody, _allBodies )
	{
		delete pBody;
	}
}

// ============================================================================
// Renders machine
void Machine::render ( QPainter& painter, const QRectF& rect )
{
	// draw bodies 
	
	foreach( Body* pBody, _bodies[ BodyRendered3 ] )
	{
		pBody->render( painter );
	}
	foreach( Body* pBody, _bodies[ BodyRendered2 ] )
	{
		pBody->render( painter );
	}
	foreach( Body* pBody, _bodies[ BodyRendered1 ] )
	{
		pBody->render( painter );
	}
	
	// draw systems 
	
	foreach( System* pSystem, _systems[ SystemRendered3 ] )
	{
		pSystem->render( painter, rect );
	}
	foreach( System* pSystem, _systems[ SystemRendered2 ] )
	{
		pSystem->render( painter, rect );
	}
	foreach( System* pSystem, _systems[ SystemRendered1 ] )
	{
		pSystem->render( painter, rect );
	}
	
}

// ============================================================================
// Simulates machine
void Machine::simulate ( double dt )
{
	// proceed with scheduled joint breakages
	while ( ! _jointsToBreak.isEmpty() )
	{
		doBreakJoint( _jointsToBreak.takeFirst() );
	}
	
	// simulate systems
	foreach ( System* pSystem, _systems[ SystemSimulated ] )
	{
		pSystem->simulate( dt );
	}
}

// ============================================================================
//. Adds system to machine. types is  bitmask with system types.
void Machine::addSystem( System* pSystem, int types )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		int bit = 1<<i;
		if ( types & bit )
		{
			_systems[ bit ].append( pSystem );
		}
	}
	
	_allSystems.append( pSystem );
}

// ============================================================================
// Removes system from all lists
void Machine::removeSystem( System* pSystem )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		_systems[ 1<<i ].removeAll( pSystem );
	}
	
	_allSystems.removeAll( pSystem );
}

// ============================================================================
// Adds body to allappropriate lists
void Machine::addBody( Body* pBody, int types )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		int bit = 1<<i;
		if ( types & bit )
		{
			_bodies[ bit ].append( pBody );
		}
	}
	
	_allBodies.append( pBody );
}

// ============================================================================
// Removes body from all lists
void Machine::removeBody( Body* pBody )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		_bodies[ 1<<i ].removeAll( pBody );
	}
	
	_allBodies.removeAll( pBody );
}

// ============================================================================
// Adds joint to list
void Machine::addJoint( Joint* pJoint )
{
	_allJoints.append( pJoint );
}

// ============================================================================
// Removes joint from list
void Machine::removeJoint( Joint* pJoint )
{
	_allJoints.removeAll( pJoint );
}

// ============================================================================
// Adds damage manager
void Machine::addDamageManager( DamageManager* pManager )
{
	_allDamageManagers.append( pManager );
}

// ============================================================================
// Removes damage mamnager
void Machine::removeDamageManager( DamageManager* pManager )
{
	_allDamageManagers.removeAll( pManager );
}

// ============================================================================
/// Flips machine aronud axis defined by p1 and p2.
/// Each flip changes the _orientation sign.
void Machine::flip( const QPointF& p1, const QPointF& p2 )
{
	_orientation = _orientation * -1;
	
	// flip bodies
	foreach( Body* pBody, _allBodies )
	{
		pBody->flip( p1, p2 );
	}
	
	// flip joints
	foreach( Joint* pJoint, _allJoints )
	{
		pJoint->flip( p1, p2 );
	}
}

// ============================================================================
/// Returns machine position in world coorindates, using main body position. Returns null pointif ther's no main body.
QPointF Machine::pos() const
{
	if ( _pMainBody && _pMainBody->b2body() )
	{
		return vec2point( _pMainBody->b2body()->GetPosition() );
	}
	
	return QPointF();
}

// ============================================================================
/// Breaks specified joint. Detaches part of machine if needed.
void Machine::breakJoint( Joint* pJoint )
{
	_jointsToBreak.append( pJoint );
	qDebug("Joint broken");
}

// ============================================================================
/// Actually breaks joint. Detaches parts of machine if neccesary.
void Machine::doBreakJoint( Joint* pJoint )
{
	// get connected bodies
	Body* pBody1 = pJoint->body1();
	Body* pBody2 = pJoint->body2();
	
	// break joint itself
	pJoint->breakJoint();
	
	// check whcih bodies are detached
	if ( pBody1 && ! pBody1->isConnectedTo( mainBody() ) )
	{
		Body*		pCopy = pBody1->createCopy();
		Shrapnel*	pShrapnel = new Shrapnel( world() );
		pShrapnel->addBody( pCopy );
		world()->addObject( pShrapnel, World::ObjectSimulated | World::ObjectRenderedForeground );
		pBody1->destroy();
		qDebug("Body %s detached", qPrintable( pBody1->name() ) );
	}
	else if ( pBody2 && ! pBody2->isConnectedTo( mainBody() ) )
	{
		Body*		pCopy = pBody2->createCopy();
		Shrapnel*	pShrapnel = new Shrapnel( world() );
		pShrapnel->addBody( pCopy );
		world()->addObject( pShrapnel, World::ObjectSimulated | World::ObjectRenderedForeground );
		pBody2->destroy();
		qDebug("Body %s detached", qPrintable( pBody1->name() ) );
	}
	else
	{
		qFatal("Machine bodies not connected to main body");
	}
}

// ============================================================================
/// Returns machine's linear velocity, or 0 if none
b2Vec2 Machine::linearVelocity() const
{
	if ( _pMainBody && _pMainBody->b2body() )
	{
		return _pMainBody->b2body()->GetLinearVelocity();
	}
	
	return b2Vec2(0,0);
}

}
