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
#include "activeattachpoint.h"
#include "passiveattachpoint.h"

namespace Flyer
{

// ============================================================================
// Constructor
Machine::Machine ( World* pWorld ) : WorldObject ( pWorld )
{
	_orientation = 1.0;
	_layers = 0;
	_size = 10; // some default value
	_pMainBody = NULL;
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
	
	foreach( ActiveAttachPoint* pPoint, _activeAttachPoints )
	{
		delete pPoint;
	}
	
	foreach( PassiveAttachPoint* pPoint, _passiveAttachPoints )
	{
		delete pPoint;
	}
}

// ============================================================================
// Renders machine
void Machine::render ( QPainter& painter, const QRectF& rect, const RenderingOptions& options )
{
	// draw bodies 
	
	foreach( Body* pBody, _bodies[ BodyRendered3 ] )
	{
		pBody->render( painter, options );
	}
	foreach( Body* pBody, _bodies[ BodyRendered2 ] )
	{
		pBody->render( painter, options );
	}
	foreach( Body* pBody, _bodies[ BodyRendered1 ] )
	{
		pBody->render( painter, options );
	}
	
	// draw systems 
	
	foreach( System* pSystem, _systems[ SystemRendered3 ] )
	{
		pSystem->render( painter, rect, options );
	}
	foreach( System* pSystem, _systems[ SystemRendered2 ] )
	{
		pSystem->render( painter, rect, options );
	}
	foreach( System* pSystem, _systems[ SystemRendered1 ] )
	{
		pSystem->render( painter, rect, options );
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
	pBody->setLayers( _layers );
	
	updateSize();
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
	updateSize();
}


// ============================================================================
/// Updates size
void Machine::updateSize()
{
	QRectF boundingRect;
	foreach( Body* pBody, _allBodies )
	{
		if ( boundingRect.isNull() )
		{
			boundingRect = pBody->shape().boundingRect();
		}
		else
		{
			boundingRect |= pBody->shape().boundingRect();
		}
	}
	
	_size = qMax( boundingRect.width(), boundingRect.height() );
	//qDebug("%s: size updated to: %g", qPrintable( name() ),_size );
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
	
	// flip attached machines
	foreach( ActiveAttachPoint* pPoint, _activeAttachPoints )
	{
		pPoint->flip( p1, p2 );
	}
	
	// flip joints
	foreach( Joint* pJoint, _allJoints )
	{
		pJoint->flip( p1, p2 );
	}
}

// ============================================================================
/// Returns machine position in world coorindates, using main body position. Returns null pointif ther's no main body.
QPointF Machine::position() const
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
		detachBody( pBody1 );
	}
	else if ( pBody2 && ! pBody2->isConnectedTo( mainBody() ) )
	{
		detachBody( pBody2 );
	}
	else
	{
		qFatal("Machine bodies not connected to main body");
	}
}

// ============================================================================
/// Detaches body from machine, turning it into shrapnell
void Machine::detachBody( Body* pBody )
{
	if ( pBody->b2body() )
	{
		Body*		pCopy = pBody->createCopy();
		Shrapnel*	pShrapnel = new Shrapnel( world() );
		pShrapnel->addBody( pCopy );
		world()->addObject( pShrapnel, World::ObjectSimulated | World::ObjectRendered );
		
		QList<Body*> connectedBodies;
		b2JointEdge* pJoint = pBody->b2body()->GetJointList();
		while( pJoint )
		{
			Body* pOther =  static_cast<Body*>( pJoint->other->GetUserData() );
			if ( pOther ) connectedBodies.append( pOther );
			pJoint = pJoint->next;
		}
		
		pBody->destroy();
		qDebug("Body %s detached", qPrintable( pBody->name() ) );
		
		// detach connected bodies
		foreach( Body* pConnected, connectedBodies )
		{
			detachBody( pConnected );
		}
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

// ============================================================================
/// Returns mahcine orenitation.
double Machine::angle() const
{
	if ( _pMainBody && _pMainBody->b2body() )
	{
		return _pMainBody->b2body()->GetAngle();
	}
	
	return 0.0;
}

// ============================================================================
/// Sets layer this machine lives in.
void Machine::setLayers( int layers )
{
	_layers = layers;
	foreach( Body* pBody, _allBodies )
	{
		pBody->setLayers( layers );
	}	
}

// ============================================================================
/// Adds attach point
void Machine::addActiveAttachPoint( ActiveAttachPoint* pPoint )
{
	if ( ! pPoint->parent() )
	{
		pPoint->setParent( this );
	}
	_activeAttachPoints.append( pPoint );
}

// ============================================================================
/// Adds attach point
void Machine::addPassiveAttachPoint( PassiveAttachPoint* pPoint )
{
	if ( ! pPoint->parent() )
	{
		pPoint->setParent( this );
	}
	_passiveAttachPoints.append( pPoint );
}

// ============================================================================
/// Calculates rough bounding rectangle of the machine.
QRectF Machine::boundingRect() const
{
	QPointF pos = position();
	return QRectF( pos.x() - _size, pos.y() - _size, _size*2, _size*2 );
}

}
