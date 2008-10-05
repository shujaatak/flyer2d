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

#include "b2dqt.h"
#include "world.h"
#include "ground.h"
#include "bodyprovider.h"
#include "common.h"

#include "building.h"

namespace Flyer
{

// ============================================================================
// Constructor
Building::Building ( World* pWorld ) : PhysicalObject( pWorld )
{
	_pBody = NULL;
}

// ============================================================================
// Destructor
Building::~Building()
{
}

// ============================================================================
/// Returns building's bounding rect
QRectF Building::boundingRect() const
{
	return _boundingRect;
}

// ============================================================================
/// Initializes small building.
void Building::initSmallBuilding( double location, bool background )
{
	int housenum = ( qrand() % 4 ) + 1;
	QString fileName = QString("house_small_%1.body").arg( housenum );
	_pBody = BodyProvider::loadBody( fileName );
	
	// set physical layer
	if ( background ) setLayers( PhysLayerBackground );
	else setLayers( PhysLayerBuildings );
	
	// set rendering layer
	if ( background ) setRenderLayer( LayerBackground );
	else setRenderLayer( LayerBuildings );
	
	b2Vec2 pos = b2Vec2(location, world()->ground()->height( location ) );
	_width = 9;
	_pBody->setPosition( pos );
	_pBody->create( world()->b2world() );
	setName( "Small building" );
	_boundingRect = _pBody->shape().boundingRect().translated( vec2point( pos ));
	addBody( _pBody, BodyRendered1 );
	setMainBody( _pBody );
	
}

// ============================================================================
/// Creates random small building
Building* Building::createSmallBuilding( World* pWorld, double location, bool background )
{
	Building* pBuilding = new Building( pWorld );
	pBuilding->initSmallBuilding( location, background );
	
	// add object
	int objectClass = World::ObjectStatic | World::ObjectRendered;
	pWorld->addObject( pBuilding, objectClass );
	
	return pBuilding;
	
}

// ============================================================================
/// Creates random city building
Building* Building::createLargeBuilding( World* /*pWorld*/, double /*location*/, bool /*background*/ )
{
	// TODO
	return NULL;
}


}
