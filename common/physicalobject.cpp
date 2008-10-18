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

#include "body.h"
#include "joint.h"
#include "b2dqt.h"
#include "shrapnel.h"
#include "world.h"
#include "common.h"

#include "physicalobject.h"

namespace Flyer
{

// ============================================================================
/// Constructor
PhysicalObject::PhysicalObject( World* pWorld ) : WorldObject( pWorld )
{
	_orientation = 1.0;
	_layers = 0;
	_size = 10; // some default value
	_pMainBody = NULL;
}

// ============================================================================
/// Destructor
PhysicalObject::~PhysicalObject()
{
	qDeleteAll( _allBodies );
}

// ============================================================================
/// Renders object
void PhysicalObject::render ( QPainter& painter, const QRectF& /*rect*/, const RenderingOptions& options )
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
}

// ============================================================================
/// Simulates physical object
void PhysicalObject::simulate ( double dt )
{
	// proceed with scheduled joint breakages
	while ( ! _jointsToBreak.isEmpty() )
	{
		doBreakJoint( _jointsToBreak.takeFirst() );
	}
	
	// proceed with scheduled body breakages
	while ( ! _bodiesToBreak.isEmpty() )
	{
		QPair<Body*, CrashEffect > pair = _bodiesToBreak.takeFirst();
		doBreakBody( pair.first, pair.second );
	}
	
	// simulate bodies
	foreach( Body* pBody, _simulatedBodies )
	{
		pBody->simulate( dt );
	}
}

// ============================================================================
// Adds body to all appropriate lists
void PhysicalObject::addBody( Body* pBody, int types )
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
	pBody->setParent( this );
	
	updateSize();
}

// ============================================================================
// Removes body from all lists
void PhysicalObject::removeBody( Body* pBody )
{
	for ( int i = 0; i < 8; i++ ) // scaning only first 8 bits
	{
		_bodies[ 1<<i ].removeAll( pBody );
	}
	
	_allBodies.removeAll( pBody );
	updateSize();
	pBody->setParent( NULL );
}


// ============================================================================
/// Updates size
void PhysicalObject::updateSize()
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
void PhysicalObject::addJoint( Joint* pJoint )
{
	_allJoints.append( pJoint );
}

// ============================================================================
// Removes joint from list
void PhysicalObject::removeJoint( Joint* pJoint )
{
	_allJoints.removeAll( pJoint );
}

// ============================================================================
/// Flips machine aronud axis defined by p1 and p2.
/// Each flip changes the _orientation sign.
void PhysicalObject::flip( const QPointF& p1, const QPointF& p2 )
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
QPointF PhysicalObject::position() const
{
	if ( _pMainBody && _pMainBody->b2body() )
	{
		return vec2point( _pMainBody->b2body()->GetPosition() );
	}
	
	return QPointF();
}

// ============================================================================
/// Breaks specified joint. Detaches part of machine if needed.
void PhysicalObject::breakJoint( Joint* pJoint )
{
	_jointsToBreak.append( pJoint );
	//qDebug("Joint broken");
}

// ============================================================================
/// Actually breaks joint. Detaches parts of machine if neccesary.
void PhysicalObject::doBreakJoint( Joint* pJoint )
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
/// Body crashes. It is detached.
void PhysicalObject::breakBody( Body* pBody, CrashEffect effect )
{
	_bodiesToBreak.append( QPair<Body*,CrashEffect>( pBody, effect ) );
}

// ============================================================================
/// Body crashes. It is detached.
void PhysicalObject::doBreakBody( Body* pBody, CrashEffect effect )
{
	detachBody( pBody, effect );
	if ( pBody == mainBody() )
	{
		// destroy self!
		world()->removeObject( this, true );
	}
}

// ============================================================================
/// Detaches body from machine, turning it into shrapnell
void PhysicalObject::detachBody( Body* pBody,  CrashEffect effect )
{
	if ( pBody->b2body() )
	{
		// TODO use 'effect' to decide how to replace the body
		switch( effect )
		{
			case NoEffect:
				createShrapnel( pBody );
				break;
				
			case FragmentationEffect:
				createShrapnels( pBody );
				break;
		}
		
		QList<Body*> connectedBodies;
		b2JointEdge* pJoint = pBody->b2body()->GetJointList();
		while( pJoint )
		{
			Body* pOther =  static_cast<Body*>( pJoint->other->GetUserData() );
			if ( pOther ) connectedBodies.append( pOther );
			pJoint = pJoint->next;
		}
		
		pBody->destroy();
		//qDebug("Body %s detached", qPrintable( pBody->name() ) );
		
		// detach connected bodies
		foreach( Body* pConnected, connectedBodies )
		{
			if ( ! pConnected->isConnectedTo( mainBody() ) )
			{
				detachBody( pConnected, NoEffect );
			}
		}
	}
}

// ============================================================================
/// Creates shrapnell that is almost exact copy of the body.
void PhysicalObject::createShrapnel( Body* pBody )
{
	Body*		pCopy = pBody->createCopy();
	Shrapnel*	pShrapnel = new Shrapnel( world() );
	pShrapnel->setLayers( layers() );
	pShrapnel->addBody( pCopy );
	world()->addObject( pShrapnel, World::ObjectSimulated );
}

// ============================================================================
/// Splits body into smaller shrapells.
void PhysicalObject::createShrapnels( Body* pBody )
{
	//qDebug("Creating shrapnels for body %s", qPrintable( pBody->name() ) );
	QPolygonF bodyShape = pBody->outline();
	
	QList<QPolygonF> shrapnelShapes = splitPolygonRandomly( bodyShape );
	
	if ( shrapnelShapes.size() < 2 )
	{
		//qDebug("Don't know how to split %s into shrapnels", qPrintable(pBody->name()) );
		createShrapnel(pBody);
		return;
	}
	// get basic infromation form source body
	float friction		= pBody->shapes().first().def()->friction;
	float restitution	= pBody->shapes().first().def()->restitution;
	float density		= pBody->shapes().first().def()->density;
	
	if ( density == 0 )
	{
		density = 100; // TODO dome stupid value, calculate it better.
	}
	
	// ok, create bodies now
	foreach( const QPolygonF& shape, shrapnelShapes )
	{
		double area = convexPolygonArea( shape );
		if ( area < 1E-4 )
		{
			//qDebug("Shrapnell rejected: to small (area: %g)", area );
		}
		else
		{
			//qDebug("Shrapnell area accepted (%g), proceeding with creation.", area );
			
			QPointF center = shape.boundingRect().center();
			Shrapnel*	pShrapnel = new Shrapnel( world() );
			Body* pSrapnellBody = new Body("shrapnell");
			
			pSrapnellBody->setTexture( pBody->texture() );
			pSrapnellBody->setLimitTextureToShape( true );
			pSrapnellBody->setPosition( pBody->position() ); // TODO translate to shape center, also: translate shape
			pSrapnellBody->setAngle( pBody->angle() );
			pSrapnellBody->setLayers( pBody->layers() );
			pSrapnellBody->setShape( shape, friction, restitution, density );
			pSrapnellBody->create( world() );
			// TODO it'd time to add velocities to body interface
			pSrapnellBody->b2body()->SetLinearVelocity( pBody->velocity() );
			pSrapnellBody->b2body()->SetAngularVelocity( pBody->angularVelocity() );
			
			pShrapnel->addBody( pSrapnellBody );
			world()->addObject( pShrapnel, World::ObjectSimulated );
		}
	}
}


// ============================================================================
/// Returns machine's linear velocity, or 0 if none
b2Vec2 PhysicalObject::linearVelocity() const
{
	if ( _pMainBody && _pMainBody->b2body() )
	{
		return _pMainBody->b2body()->GetLinearVelocity();
	}
	
	return b2Vec2(0,0);
}

// ============================================================================
/// Returns mahcine orenitation.
double PhysicalObject::angle() const
{
	if ( _pMainBody && _pMainBody->b2body() )
	{
		return _pMainBody->b2body()->GetAngle();
	}
	
	return 0.0;
}

// ============================================================================
/// Sets layer this machine lives in.
void PhysicalObject::setLayers( int layers )
{
	_layers = layers;
	foreach( Body* pBody, _allBodies )
	{
		pBody->setLayers( layers );
	}	
}

// ============================================================================
/// Calculates rough bounding rectangle of the machine.
QRectF PhysicalObject::boundingRect() const
{
	QPointF pos = position();
	return QRectF( pos.x() - _size, pos.y() - _size, _size*2, _size*2 );
}

// ============================================================================
/// Message from body - body wishes to receive events
void PhysicalObject::bodyWakesUp( Body* pBody )
{
	if ( ! _simulatedBodies.contains( pBody ) )
	{
		//qDebug( "Body %s wakes up", qPrintable( pBody->name() ) );
		_simulatedBodies.append( pBody );
	}
	wakeUp();
}

// ============================================================================
/// Message from body - body need no more events
void PhysicalObject::bodySleeps( Body* pBody )
{
	//qDebug( "Body %s goes to sleep", qPrintable( pBody->name() ) );
	_simulatedBodies.removeAll( pBody );
	if ( _simulatedBodies.empty() )
	{
		sleep();
	}
}

}

// EOF

