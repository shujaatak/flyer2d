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

#include "ironbomb.h"
#include "body.h"
#include "damagemanager.h"
#include "controlsurface.h"
#include "b2dqt.h"
#include "world.h"
#include "contactfuse.h"

namespace Flyer
{

// ============================================================================
/// Constructor
IronBomb::IronBomb ( World* pWorld ) : Machine ( pWorld )
{
}

// ============================================================================
/// Destructor
IronBomb::~IronBomb()
{
}

// ============================================================================
/// Configures
void IronBomb::init( const QPointF& position, double angle )
{
	// create main body
	b2BodyDef bodyDef;
	bodyDef.position = point2vec( position );
	bodyDef.angle = angle;
	bodyDef.isBullet = true;
	
	_bodyMain = new Body( "Bomb" );
	_bodyMain->create( bodyDef, world()->b2world() );
	addBody( _bodyMain, BodyRendered1 );
	setMainBody( _bodyMain );
	
	// create dm
	_dmFuse = new DamageManager( 5E3 ); // small value
	addDamageManager( _dmFuse );
	
	// create shape
	b2PolygonDef shapeDef;
	shapeDef.vertexCount = 6;
	shapeDef.vertices[0].Set( _length/2, 0 );
	shapeDef.vertices[1].Set( _length/2 - _diameter/2, _diameter/2 );
	shapeDef.vertices[2].Set( -_length/2 + _diameter/2, _diameter/2 );
	shapeDef.vertices[3].Set( -_length/ 2, 0 );
	shapeDef.vertices[4].Set( -_length/2 + _diameter/2, -_diameter/2 );
	shapeDef.vertices[5].Set( _length/2 - _diameter/2, -_diameter/2 );
	shapeDef.friction = 0.6;
	shapeDef.restitution = 0.1;
	shapeDef.density = _mass / (_length*_diameter );
	shapeDef.userData = _dmFuse;
	_bodyMain->addShape( & shapeDef );
	
	// create surface
	_sysStabilizer = new ControlSurface( this, "Stabilizer" );
	_sysStabilizer->setBody( _bodyMain );
	_sysStabilizer->setLength( _diameter );
	_sysStabilizer->setPosition( QPointF( -_length/2, 0.0 ) );
	_sysStabilizer->setWidth( _diameter );
	_sysStabilizer->setDragCoeffH( 0.1 );
	_sysStabilizer->setDragCoeffV( 2 );
	_sysStabilizer->setLiftCoeff( 6 );
	addSystem( _sysStabilizer, SystemSimulated | SystemRendered2 );
	
	// create fuse
	_sysFuse = new ContactFuse( this, "Fuse" );
	_sysFuse->setEnergy( _energy );
	_sysFuse->setDamageCapacity( 20E3 );
	addSystem( _sysFuse, 0 );
	_dmFuse->addSystem( _sysFuse, 1 );
}


}
