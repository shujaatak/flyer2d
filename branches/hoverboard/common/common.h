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

///\file common.h Common definitions used across the project.

#ifndef FLYER_COMMON_H
#define FLYER_COMMON_H

class QPainter;

#include <QPolygonF>
class QLineF;
class QPainterPath;
class b2PolygonDef;

namespace Flyer
{

/// Predefined render layers, used to set Z-coorindate on world items
enum RenderLayer {
	// render layers
	LayerSky			= 0,		///< Sky - the ultimate background
	LayerBackground		= 1,		///< Background - first physical layer
	LayerBuildings		= 2,		///< Buildings
	LayerVehicles		= 3,		///< Ground vehicles and planes
	LayerForeground		= 4,		///< Foreground ground and non-physical objects
	LayerHud			= 5,		///< Absolute foregorund (painted from inside on your monitor)
};

/// Physical layers
enum PhysicalLayers {
	PhysLayerBackground	= 0x0001,
	PhysLayerBuildings	= 0x0002,
	PhysLayerVehicles	= 0x0004,
};

// Common utilities
///////////////////

/// Benchmarkng utility. Returns current imte as milliseconds.
double getms(); 

/// Checks if painter paints on OpenGL surface.
bool isOpenGL( QPainter* p );

// Random numbers
/////////////////


/// Returns random from [0, 1)
double random01();

// Geometrical utilities
////////////////////////

/// Finds approximated outline of the path.
QPolygonF findApproximateOutline( const QPainterPath& path );

/// Splits polygon randomly into two, similary-sized polygons
QList<QPolygonF> splitPolygonRandomly( const QPolygonF& polygon, QLineF* pdebugout = NULL );

/// Splits polygon into two, cutting it with the supplied line
QList<QPolygonF> splitPolygon( const QPolygonF& polygon, const QLineF& line );

/// Converts polygon into set of triangles
QList<QPolygonF> triangulatePolygon( const QPolygonF& polygon );

/// Checks if polygon is convex
bool isPolygonConvex( const QPolygonF& p );

/// Calculates polygon's area
double convexPolygonArea( const QPolygonF& p );

/// Checks polygon direction
bool convexPolygonDirection( const QPolygonF& p );

/// Cecks polgon validity for Box2D
int CheckPolyShape(const b2PolygonDef* poly);

}

#endif // FLYER_COMMON_H

// EOF
