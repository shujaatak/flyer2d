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

#include "Box2D.h"

#include "renderingoptions.h"
#include "world.h"
#include "textureprovider.h"

#include "ground.h"

namespace Flyer
{

// ============================================================================
// Constructor
Ground::Ground ( World* pWorld ) : WorldObject ( pWorld )
{
	
	//random();
	
	// init seed
	QList<RGSection> seed;
	RGSection section;
	
	// first section - mountains from -15 to -5 km
	section.x = -15000;
	section.y = 2500;
	section.maxSlope = 1.5;
	section.canBeDividedRight = true;
	section.minSectionSize = 50;	// dense terrain
	section.maxSectionSize = 200;
	section.maxHeight = 2400;
	section.minHeight = 400;
	seed.append( section );
	
	// second section - flat land fro -5 km to -50m
	section.x = -5000;
	section.y = 500;
	section.maxSlope = 0.1;
	section.canBeDividedRight = true;
	section.minSectionSize = 100;	// smooth terrain
	section.maxSectionSize = 400;
	section.maxHeight = 700;
	section.minHeight = 250;
	seed.append( section );
	
	// third section - airfield
	section.x = -100;
	section.y = 300;
	section.canBeDividedRight = false;
	seed.append( section );
	
	// fourth section - smooth flats gently going up to 10km
	section.x = 300; // this gives 400m for runway
	section.y = 300; // flat runway
	section.maxSlope = 0.1;
	section.canBeDividedRight = true;
	section.minSectionSize = 100;
	section.maxSectionSize = 400;
	section.maxHeight = 850;
	section.minHeight = 250;
	seed.append( section );
	
	// fifth section - next airfield
	section.x = 10000; // at 10th km
	section.y = 800;
	section.canBeDividedRight = false;
	seed.append( section );
	
	// sixth section - mountins to the end
	section.x = 10350; // 250m runway
	section.y = 800;
	section.maxSlope = 1.5;
	section.canBeDividedRight = true;
	section.minSectionSize = 70;
	section.maxSectionSize = 200;
	section.maxHeight = 2400;
	section.minHeight = 700;
	seed.append( section );
	
	// very last section
	section.x = 15000;
	section.y = 2500;
	section.canBeDividedRight = false;
	seed.append( section );
	
	_heightmap = generate( seed );
	
	// create ground
	b2BodyDef groundBodyDef;
	groundBodyDef.position.SetZero();
	
	_pGround = new Body("Ground");
	_pGround->create( groundBodyDef, pWorld->b2world() );
	_pGround->setLayers( 0xffff ); //alll!
	
	QList<b2PolygonDef*> shapes = createShape();
	foreach( b2PolygonDef* pShape, shapes )
	{
		/*
		qDebug("Creating triangle: %.2f,%.2f %.2f,%.2f %.2f,%.2f"
			, pShape->vertices[0].x, pShape->vertices[0].y
			, pShape->vertices[1].x, pShape->vertices[1].y
			, pShape->vertices[2].x, pShape->vertices[2].y
			);
		*/
		_pGround->addShape( pShape );
		delete pShape;
	}
	
	prepareTextures();
	
}

// ============================================================================
// Destructor
Ground::~Ground()
{
}


// ================================= set heightmap =====================
/// Sets height map
void Ground::setHeightmap( const QPolygonF& heightMap )
{
	_heightmap = heightMap;
}

// ================================== height ========================
/// Calculates y for specified x.
double Ground::height( double x ) const
{
	// find line containing this coordinate
	for( int i = 0; i < _heightmap.size() - 1; i++ )
	{
		if ( _heightmap[i].x() <= x && _heightmap[i+1].x() >= x )
		{
			double x1 = _heightmap[i].x();
			double y1 = _heightmap[i].y();
			double x2 = _heightmap[i+1].x();
			double y2 = _heightmap[i+1].y();
			
			return y1 + ( y2 - y1 ) * ( x - x1) / ( x2 - x1 );
		}
	}
	return 0;
}

// ============================================================================
// Generates random ground
void Ground::random()
{
	QRectF boundary = world()->boundary();
	const double WALL_WIDTH = 200;
	double worldHeight = boundary.bottom();
	double worldWidth = boundary.width() - 2* WALL_WIDTH;
	
	double maxSlope = 0.2;
	
	// params / constants
	double MAX_HEIGHT = worldHeight * 0.3; // fit between 10% and 90%
	double MIN_HEIGHT = 0;
	
	const double MAX_SLICE_WIDTH = 500.0;
	const double MIN_SLICE_WIDTH = 100.0;
	
	double MAX_SLOPE = maxSlope; // difficulty level. max slope is MAX_SLOPE * (1+SLOPE_VARIATION)
	const double SLOPE_GAIN = 0.02;	// controlelr gain: slope/meter difference
	double SLOPE_VARIATION = maxSlope * 0.1;
	
	// first - get random target heights at constant intervals
	const int SECTIONS = 2; // 10 slices
	double targets[ SECTIONS ];
	
	for ( int i = 0; i < SECTIONS; i++ )
	{
		targets[ i ] = MIN_HEIGHT + ( MAX_HEIGHT - MIN_HEIGHT ) * double(qrand()) / RAND_MAX;
	}
	
	// now - randomize slices
	QPolygonF points;
	
	// intial height
	double height = MIN_HEIGHT + ( MAX_HEIGHT - MIN_HEIGHT ) * double(qrand()) / RAND_MAX;
	
	// first point - big mountan t ther begginging
	points.append( QPointF( 0, worldHeight ) );
	points.append( QPointF( WALL_WIDTH, height ) );
	
	// now - randomize points
	double x = 0;
	while ( x < ( worldWidth - MAX_SLICE_WIDTH ) )
	{
		double sliceWidth = MIN_SLICE_WIDTH + ( MAX_SLICE_WIDTH - MIN_SLICE_WIDTH ) * double(qrand()) / RAND_MAX;
		
		// find out which section is it
		double sectionReal = x / worldWidth / SECTIONS ;
		int closestSection = qRound( sectionReal );
		int nextSection = ceil( sectionReal );
		
		// find target
		
		// calculate slope
		double calculatedSlope = SLOPE_GAIN * ( targets[closestSection] - height );
		
		// apply sign based on next section pos
		if ( ( ( targets[nextSection] - height ) * calculatedSlope ) < 0.0 )
		{
			calculatedSlope *= -1.0;
		}
		
		// apply limit
		if ( calculatedSlope > MAX_SLOPE ) calculatedSlope = MAX_SLOPE;
		if ( calculatedSlope < -MAX_SLOPE ) calculatedSlope = -MAX_SLOPE;
		
		// add randomization
		double slope = calculatedSlope - SLOPE_VARIATION + 2*SLOPE_VARIATION*( double(qrand()) / RAND_MAX );
		
		// calculate new height
		height = height + slope * sliceWidth;
		x += sliceWidth;
		
		points.append( QPointF( x + WALL_WIDTH, height ) );
		
	}
	
	// close
	points.append( QPointF( worldWidth + WALL_WIDTH*2, worldHeight ) );
	
	points.translate( world()->boundary().left(), 0 );
	
	setHeightmap( points );	
	prepareTextures();
}

// ============================================================================
// Recursively generates random terrain using provided seed
QPolygonF Ground::generate( QList<RGSection> seed )
{
	
	QPolygonF result;
 	// prepare seed
	int seedSize = seed.size();
	for( int i =0; i< seedSize; i++ )
	{
		// common
		seed[i].canBeDividedLeft = false; // we divining only to the right
		
		if ( seed[i].canBeDividedRight && i < seedSize-1 )
		{
			seed[i].nextX = seed[i+1].x;
			seed[i].nextY = seed[i+1].y;
			
			divideRight( seed[i] );
		}
			
		traverseSection( seed[i], result );
		
	}
	
	return result;
}

// ============================================================================
// Traverses section in-order and appends result to polygon
void Ground::traverseSection( RGSection& section, QPolygonF& points )
{
	if ( section.pLeft )
	{
		traverseSection( * section.pLeft, points );
	}
	points.append( QPointF( section.x, section.y ) );
	if ( section.pRight )
	{
		traverseSection( * section.pRight, points );
	}
	
}

// ============================================================================
// Divides scetion to the right
void Ground::divideRight( RGSection& section )
{
	double width = section.nextX - section.x;
	
	if ( width < section.minSectionSize * 2 )
	{
		return; // STOP
	}
	
	// ok, find x of new point
	double newX = rand( section.x +  section.minSectionSize, section.nextX - section.minSectionSize );
	
	double widthLeft = newX - section.x;
	double widthRight = section.nextX - newX;
	
	// now find y
	double maxY = qMin( section.y + widthLeft * section.maxSlope,
		section.nextY + widthRight * section.maxSlope );
	double minY = qMax( section.y - widthLeft * section.maxSlope,
		section.nextY - widthRight * section.maxSlope );
	
	
	double newY = rand( qMax( section.minHeight, minY), qMin( section.maxHeight, maxY ) );
	
	//qDebug("divide right between %.0f,%0.f and %.0f,%.0f: new point: %.0f,%.0f",
	//	section.x, section.y, section.nextX, section.nextY, newX, newY );
	
	RGSection* newSection = new RGSection;
	
	newSection->minSectionSize = section.minSectionSize;
	newSection->maxSectionSize = section.maxSectionSize;
	newSection->maxSlope = section.maxSlope;
	newSection->maxHeight = section.maxHeight;
	newSection->minHeight = section.minHeight;
	
	newSection->x = newX;
	newSection->y = newY;
	
	Q_ASSERT( world()->boundary().contains( newX, newY ) );
	 
	newSection->prevX = section.x;
	newSection->prevY = section.y;
	newSection->nextX = section.nextX;
	newSection->nextY = section.nextY;
	
	section.pRight = newSection;
	
	divideRight( *newSection );
	divideLeft( *newSection );
}

// ============================================================================
// Divides scetion to the left
void Ground::divideLeft( RGSection& section )
{
	double width = section.x - section.prevX;
	
	if ( width < section.minSectionSize * 2 )
	{
		return; // STOP
	}
	
	// ok, find x of new point
	double newX = rand( section.prevX +  section.minSectionSize, section.x - section.minSectionSize );
	double widthLeft = newX - section.prevX;
	double widthRight = section.x - newX;
	// now find y
	double maxY = qMin( section.y + widthRight * section.maxSlope,
		section.prevY + widthLeft * section.maxSlope );
	double minY = qMax( section.y - widthRight * section.maxSlope,
		section.prevY - widthLeft * section.maxSlope );
	
	double newY = rand( qMax( section.minHeight, minY), qMin( section.maxHeight, maxY ) );
	
	//qDebug("divide left between %.0f,%0.f and %.0f,%.0f: new point: %.0f,%.0f",
	//	section.prevX, section.prevY, section.x, section.y, newX, newY );
	
	RGSection* newSection = new RGSection;
	
	newSection->minSectionSize = section.minSectionSize;
	newSection->maxSectionSize = section.maxSectionSize;
	newSection->maxSlope = section.maxSlope;
	newSection->maxHeight = section.maxHeight;
	newSection->minHeight = section.minHeight;
	
	newSection->x = newX;
	newSection->y = newY;
	
	Q_ASSERT( world()->boundary().contains( newX, newY ) );
	
	newSection->prevX = section.prevX;
	newSection->prevY = section.prevY;
	newSection->nextX = section.x;
	newSection->nextY = section.y;
	
	section.pLeft = newSection;
	
	divideRight( *newSection );
	divideLeft( *newSection );
}

// ============================================================================
// Generates random number from range from start to end inclusive
double Ground::rand( double start, double end )
{
	return start + (end-start) * 0.001*(qrand() % 1000);
}

// =========================== create shape ============================
QList<b2PolygonDef*> Ground::createShape()
{
	QList<b2PolygonDef*> list;
	
	// create two triangles for each point (trapezoid) in height map
	int num = _heightmap.size();
	QPointF prevPoint = _heightmap.first();
	for ( int i = 1; i < num; i++ )
	{
		QPointF point = _heightmap[ i ];
		
		double bottom = qMax( qMin( point.y(), prevPoint.y() ) - 100, world()->boundary().top() );
		
		// calculate point at bottom
		QPointF bottomPoint( ( prevPoint.x() + point.x() ) / 2, bottom );
		
		list.append( createTriangleB2Shape( bottomPoint, point, prevPoint ) );
		
		prevPoint = point;
	}
	
	return list;
}
// ========================== create triangle ================
b2PolygonDef* Ground::createTriangleB2Shape( const QPointF& a, const QPointF& b, const QPointF& c )
{
	b2PolygonDef* pTriangle = new b2PolygonDef();
	
	pTriangle->vertexCount = 3;
	pTriangle->vertices[0].Set( a.x(), a.y() );
	pTriangle->vertices[1].Set( b.x(), b.y() );
	pTriangle->vertices[2].Set( c.x(), c.y() );
	
	pTriangle->restitution = 0.01;
	
	return pTriangle;
}

// ============================================================================
// Renders ground
void Ground::render ( QPainter& painter, const QRectF& rect, const RenderingOptions& /*options*/ )
{
	painter.setPen( Qt::NoPen );
	painter.setBrush( QColor("#8F6A32") );

	// draw filling
	
	// create cached polygin, if none
	if ( _painterPolygon.empty() )
	{
		_painterPolygon = _heightmap;
		// add corners
		_painterPolygon.append( world()->boundary().topRight() );
		_painterPolygon.prepend( world()->boundary().topLeft() );
	}
	painter.drawPolygon( _painterPolygon );
	
	// draw textures
	for( int i = 0; i < _heightmap.size() - 1; i++ )
	{
		const QPointF& p1 = _heightmap[i];
		const QPointF& p2 = _heightmap[i+1];
		
		double low = qMin( p1.y(), p2.y() );
		double hi = qMax( p1.y(), p2.y() );
		double vmargin = 10; // [m]
		
		QRectF segmentRect( QPointF( p1.x(), low-vmargin), QPointF( p2.x(), hi+vmargin ) );
		
		// does the section fits into screen?
		if ( ! segmentRect.intersects( rect ) )
		{
			continue;
		}
		
		// ok, these points define line that should be textured along
		
		// determine translation
		double scale = 0.05; // std 5cm / pixel
		//double scale = 1.0;
		double shear = ( p2.y() - p1.y() ) / ( p2.x() - p1.x() );
		QTransform t;
		t.translate(  _heightmap[i].x(), _heightmap[i].y() );
		t.scale( scale, - scale );
		t.shear( 0, -shear );
		
		// draw pixmaps
		painter.save();
			painter.setClipRect( segmentRect );
			painter.setTransform( t, true );
			double x = 0;
			foreach( int index, _textureIndices[i] )
			{
				const QImage& image = _textures[ index ];
				painter.drawImage( x, -image.height(), image );
				//painter.setPen( Qt::red );
				//painter.drawRect( x, -1, 4, 1 );
				x += image.width();
			}
		painter.restore();
	}
	
}

// ============================================================================
// Renders grpound on map
void Ground::renderOnMap( QPainter& painter, const QRectF& /*rect*/ )
{
	painter.setPen( Qt::black );
	painter.setBrush( Qt::green );

	// create cached polygin, if none
	if ( _painterPolygon.empty() )
	{
		_painterPolygon = _heightmap;
		// add corners
		_painterPolygon.append( world()->boundary().topRight() );
		_painterPolygon.prepend( world()->boundary().topLeft() );
	}
	
	
	painter.drawPolygon( _painterPolygon );
}

// ============================================================================
/// This method generates toxtrues which will be used to render ground.
/// It is assumed that heightmap is already generated.
void Ground::prepareTextures()
{
	// first - get list of source images
	_textures.clear();
	for( int i = 0; i < 7; i++ ) // TODO umebr of pixmaps shoudl be read by scaning proper directory
	{
		QString fileName = QString("ground/grass_low%1.png").arg(i+1);
		Texture t =TextureProvider::loadTexture( fileName );
		_textures.append( t.image( Texture::Normal ) );
	}
	
	// ok, now generate randomsequences for each ground segment
	_textureIndices.clear();
	for( int i = 0; i < _heightmap.size()-1; i++ )
	{
		_textureIndices.append( QList<int>() );
		// TODO this assumes that each pixmap is 4m long
		double segmentLength = _heightmap[i+1].x() - _heightmap[i].x();
		int imageCount = int( ceil( segmentLength / 4.0 ) );
		for( int j = 0; j < imageCount; j++ )
		{
			_textureIndices.last().append( qrand() % _textures.size() );
		}
	}
}

}
