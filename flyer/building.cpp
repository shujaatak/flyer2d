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

#include "building.h"

namespace Flyer
{

// ============================================================================
// Constructor
Building::Building ( World* pWorld ) : WorldObject ( pWorld )
{
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
/// Renders building
void Building::render ( QPainter& painter, const QRectF& /*rect*/, const RenderingOptions& options  )
{
	// TODO both ways here, to compare results
	_pBody->render( painter, options );
	/*
	painter.save();
	
		
		painter.setTransform( _pBody->transform(), true );
		QPen pen;
		pen.setWidth( 1 );
		pen.setCosmetic( true );
		pen.setColor( _background ? Qt::lightGray : Qt::black );
		painter.setPen( pen );
		
		painter.setBrush( _color );
		
		painter.drawPath( _path );
		
	painter.restore();
	*/
}

// ============================================================================
/// Creates random small building
Building* Building::createSmallBuilding( World* pWorld, double location, bool background )
{
	Building* pBuilding = new Building( pWorld );
	
	int housenum = ( qrand() % 3 ) + 1;
	QString fileName = QString("house_small_%1.body").arg( housenum );
	pBuilding->_pBody = BodyProvider::loadBody( fileName );
	
	
	pBuilding->_width = 9; // TODO read from somwhere
	pBuilding->_pBody->def().position.Set( location, pWorld->ground()->height( location ) );
	pBuilding->_pBody->create( pWorld->b2world() );
	
	if ( background ) pBuilding->_pBody->setLayers( World::ObjectRenderedBackground );
	else pBuilding->_pBody->setLayers( World::ObjectRenderedBuildings );
	
	// add object
	int objectClass = World::ObjectStatic;
	if( background ) objectClass |= World::ObjectRenderedBackground;
	else objectClass |= World::ObjectRenderedBuildings;
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
