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
#include "damagemanager.h"
#include "controlsurface.h"
#include "b2dqt.h"
#include "contactfuse.h"
#include "bodyprovider.h"
#include "passiveattachpoint.h"
#include "world.h"
#include "common.h"

#include "ironbomb.h"

namespace Flyer
{

// ============================================================================
/// Constructor
IronBomb::IronBomb ( World* pWorld ) : Machine ( pWorld )
{
	_attachPoint = NULL;
	setLayers( PhysLayerVehicles | PhysLayerBuildings );
	setRenderLayer( LayerVehicles );
	setName( "Bomb" );
}

// ============================================================================
/// Destructor
IronBomb::~IronBomb()
{
}

// ============================================================================
/// Configures, ale sets 125 kg bomb defaults
void IronBomb::init( const QPointF& position, double angle )
{
	// create main body
	b2BodyDef bodyDef;
	bodyDef.position = point2vec( position );
	bodyDef.angle = angle;
	bodyDef.isBullet = true;
	
	_bodyMain = BodyProvider::loadBody( "weapons/gpb125.body" );
	_bodyMain->setPosition( point2vec( position ) );
	_bodyMain->setAngle( angle );
	addBody( _bodyMain, BodyRendered1 );
	_bodyMain->create( world()->b2world() );
	setMainBody( _bodyMain );
	
	// create dm
	_dmFuse = new DamageManager( 10E3 ); // small value
	_bodyMain->shapeByName("main")->setDamageManager( _dmFuse );
	addDamageManager( _dmFuse );
	
	
	// create surface
	_length = 1.6; // TODO get this from somwhere, read form body.
	_sysStabilizer = new ControlSurface( this, "Stabilizer" );
	_sysStabilizer->setBody( _bodyMain );
	_sysStabilizer->setLength( _diameter );
	_sysStabilizer->setPosition( QPointF( -_length/2, 0.0 ) );
	_sysStabilizer->setWidth( _diameter );
	_sysStabilizer->setDragCoeffH( 0.1 );
	_sysStabilizer->setDragCoeffV( 2 );
	_sysStabilizer->setLiftCoeff( 6 );
	addSystem( _sysStabilizer, SystemSimulated );
	
	// create fuse
	_energy = 500E3; // explosion energy. 
	_sysFuse = new ContactFuse( this, "Fuse" );
	_sysFuse->setEnergy( _energy );
	_sysFuse->setDamageCapacity( 30E3 ); // small enough
	addSystem( _sysFuse, 0 );
	_dmFuse->addSystem( _sysFuse, 1 );
	
	// create attach point
	_attachPoint = new PassiveAttachPoint();
	_attachPoint->setParent( this );
	_attachPoint->setPosition( b2Vec2( 0, 0.3 ) );
	addPassiveAttachPoint( _attachPoint );
	
	
}


}
