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

#include "building.h"

namespace Flyer
{

/// Small building colors
static const char* SMALL_COLORS[] = { "#9F1215", "#685D9F", "#30809F", "#319F59",
	 "#799F2C", "#9D9F1F", "#9F4823", "#E0E0E0" };
	 
static const char* CITY_COLORS[] = { "#6ADCDC", "#A7C9DC", "#BDBDBD", "#BDBC6F", "#A6B9BD", "#6C9DBD" };

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
void Building::render ( QPainter& painter, const QRectF& rect )
{
	// TODO both ways here, to compare results
	_pBody->render( painter );
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
	// single shape for now
	QPainterPath path;
	
	path.moveTo( 4, 0 );
	path.lineTo( 4, 4 );
	path.lineTo( 4.5, 4 );
	path.lineTo( 0, 9 ); // top of the roof
	path.lineTo( -4.5, 4 );
	path.lineTo( -4, 4 );
	path.lineTo( -4, 0 );
	path.closeSubpath();
	// TODO add windows and other features here
	
	Building* pBuilding = new Building( pWorld );
	
	pBuilding->_path = path;
	pBuilding->_width = 9; 
	
	// position
	double y = qMin( pWorld->ground()->height( location - 4 ), pWorld->ground()->height( location + 4 ) );
	pBuilding->_position.Set( location, y );
	
	// color
	int numColors = sizeof(SMALL_COLORS) / sizeof(SMALL_COLORS[0]);
	int selectedColor = qrand() % numColors;
	
	QColor color = QColor( SMALL_COLORS[ selectedColor ] );
	
	// for background building - blend the color a bit
	if ( background )
	{
		int h, s, v;
		color.getHsv( &h, &s, &v );
		s /= 4;
		color.setHsv( h, s, v );
	}
	pBuilding->_color = color;
	pBuilding->_background = background;
	
	// create body n' shape
	QPolygonF shape;
	shape.append( QPointF( 4, 0 ) );
	shape.append( QPointF( 4, 4.5 ) );
	shape.append( QPointF( 0, 9 ) );
	shape.append( QPointF( -4, 4.5 ) );
	shape.append( QPointF( -4, 0 ) );
	b2PolygonDef polygonDef = shapeToDef( shape, false );
	polygonDef.friction = 0.9;
	polygonDef.restitution = 0.5;
	
	b2BodyDef bodyDef;
	bodyDef.massData.mass = 0; // static body
	bodyDef.position = pBuilding->_position;
	
	pBuilding->_pBody = new Body( "Small building" );
	pBuilding->_pBody->setTexture( "smallhouse.png" );
	pBuilding->_pBody->setTexturePosition( QPointF( -5, -9 ) );
	
	if ( background ) pBuilding->_pBody->setLayers( World::ObjectRenderedBackground );
	else pBuilding->_pBody->setLayers( World::ObjectRenderedBuildings );
	
	pBuilding->_pBody->addShape( & polygonDef );
	pBuilding->_pBody->create( bodyDef, pWorld->b2world() );
	
	// bounding rect
	pBuilding->_boundingRect = shape.boundingRect();
	pBuilding->_boundingRect.translate( pBuilding->_position.x, pBuilding->_position.y );
	
	// TODO set layer here
	
	// add object
	int objectClass = World::ObjectStatic;
	if( background ) objectClass |= World::ObjectRenderedBackground;
	else objectClass |= World::ObjectRenderedBuildings;
	pWorld->addObject( pBuilding, objectClass );
	
	return pBuilding;
	
}

// ============================================================================
/// Creates random city building
Building* Building::createLargeBuilding( World* pWorld, double location, bool background )
{
	// TODO
}


}
