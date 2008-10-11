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

#include <sys/time.h>
#include <math.h>

#include <QPainter>
#include <QPaintEngine>
#include <QPainterPath>
#include <QLineF>

#include "b2dqt.h"

extern "C" {
#include "gpc.h"
}


#include "common.h"

namespace Flyer
{

// ============================================================================
/// Returns current time in ms.
double getms()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	
	return (tv.tv_sec & 0xff ) * 1e3 + tv.tv_usec * 1e-3;
}

// ============================================================================
/// Checks if painter paints on OpenGL surface.
bool isOpenGL( QPainter* p )
{
	return p->paintEngine()->type() == QPaintEngine::OpenGL;
}

// ============================================================================
/// Returns random value from 0 inclusive to 1 exclusive
double random01()
{
	return (qrand() % 0xffff) / (double)0xffff;
}

// ============================================================================
/// finfds aproximates outline of path.
QPolygonF findApproximateOutline( const QPainterPath& path )
{
	//TODO better implementation here
	return path.toFillPolygon( QTransform() );
}

// ============================================================================
/// Splits polygon in two
QList<QPolygonF> splitPolygonRandomly( const QPolygonF& polygon )
{
	QRectF br = polygon.boundingRect();
	
	// get random angle on cutting
	double angle = random01()* 6.0;
	
	// cut throught the center
	QPointF center = br.center();
	double upper = br.width() + br.height(); // upper bound for any of the polygon's dimensions
	
	QPointF p1( center.x() - upper*sin(angle), center.y() - cos(angle) );
	QPointF p2( center.x() + upper*sin(angle), center.y() + cos(angle) );
	
	return splitPolygon( polygon, QLineF( p1, p2 ) );
}

// ============================================================================
/// Splits polygon into two polygons alogn the line. If polygon is concave, results are undefined.
QList<QPolygonF> splitPolygon( const QPolygonF& polygon, const QLineF& line )
{
	QList<QPolygonF> res;
	
	// find intersections
	
	// indices of intersected lines
	int interIndex1 = -1;
	int interIndex2 = -1;
	QPointF intersection1;
	QPointF intersection2;
	
	for( int i = 0; i < polygon.size(); i ++ )
	{
		QLineF l( polygon[i], polygon[ (i+1) % polygon.size() ] );
		QPointF intersection;
		
		if ( l.intersect( line, &intersection ) == QLineF::BoundedIntersection )
		{
			if ( interIndex1 < 0 )
			{
				interIndex1 = i;
				intersection1 = intersection;
			}
			else
			{
				interIndex2 = i;
				intersection2 = intersection;
				break; // search no more
			}
		}
	}
	
	// checf if two intersections were found
	if ( interIndex2 < 0 )
	{
		qDebug("splitPolygon: Polygon is not intersected by line");
		res.append( polygon );
		return res;
	}

	// construct two polygons using found intersections
	QPolygonF res1, res2;
	
	res1.append( intersection1 );
	for( int i = ( interIndex1 + 1 ) % polygon.size(); i != interIndex2;
		i = ( i+1) % polygon.size() )
	{
		res1.append( polygon[i] );
	}
	res1.append( intersection2 );
	
	res2.append( intersection2 );
	for( int i = ( interIndex2 + 1 ) % polygon.size(); i != interIndex1;
		i = ( i+1) % polygon.size() )
	{
		res2.append( polygon[i] );
	}
	res2.append( intersection1 );
	
	res.append( res1 );
	res.append( res2 );
	
	return res;
}

// ============================================================================
/// Converts polygon into set of triangles.
QList<QPolygonF> triangulatePolygon( const QPolygonF& polygon )
{
	QList<QPolygonF> triangles;
	if ( polygon.size() < 3 )
	{
		qDebug("Can't create shape from polygon with less than 3 vertices!");
		return triangles;
	}
	if ( polygon.size() == 3 )
	{
		triangles.append( polygon );
		return triangles;
	}
	// triangulate here
	// create GPC polygon from QPolygonF
	gpc_vertex_list	vertexList;
	gpc_polygon		gpcpolygon;
	vertexList.num_vertices	= polygon.size();
	vertexList.vertex		= new gpc_vertex[ polygon.size() ];
	
	for( int i = 0; i < vertexList.num_vertices; i++ )
	{
		vertexList.vertex[i].x = polygon[i].x();
		vertexList.vertex[i].y = polygon[i].y();
	}
	
	gpcpolygon.num_contours		= 1;
	gpcpolygon.hole				= NULL;
	gpcpolygon.contour			 = &vertexList;
	
	// request triangles
	gpc_tristrip tristrip;
	
	gpc_polygon_to_tristrip( &gpcpolygon, &tristrip );
	
	// create triangles from tristrups
	for( int s = 0; s < tristrip.num_strips; s++ )
	{
		gpc_vertex_list& strip	= tristrip.strip[s];
		int numTriangles		= strip.num_vertices - 2;
		for ( int t = 0; t < numTriangles; t++ )
		{
			QPolygonF triangle;
			triangle.append( QPointF( strip.vertex[t].x,	strip.vertex[t].y ) );
			triangle.append( QPointF( strip.vertex[t+1].x,	strip.vertex[t+1].y ) );
			triangle.append( QPointF( strip.vertex[t+2].x,	strip.vertex[t+2].y ) );
			
			triangles.append( triangle );
		}
	}
	
	// release polygon
	delete[] vertexList.vertex;
	gpc_free_tristrip( &tristrip );
	
	return triangles;
}

// ============================================================================
/// Checks if polygon is convex.
bool isPolygonConvex( const QPolygonF& p )
{
	int points = p.size();
	if ( points < 3 )
	{
		return true; // short circuit edge case
	}
	
	int negatives = 0;
	int positives = 0;
	
	for( int i = 0; i < points; i++ )
	{
		QPointF p1 = p[i];
		QPointF p2 = p[(i+1)%points];
		QPointF p3 = p[(i+2)%points];
		
		// TODO use b2Cross here
		
		// converts these points to 2 3d vectors
		double a1 = p2.x() - p1.x();
		double a2 = p2.y() - p1.y();
		// a3 = 0
		
		double b1 = p3.x() - p2.x();
		double b2 = p3.y() - p2.y();
		// b3 = 0;
		
		// now find cross product of these vectors
		// c = a x b
		// c1 = 0, c2 = 0
		double c3 = a1*b2 - a2*b1;
		
		
		if ( c3 < 0 ) negatives ++;
		else positives++;
	}
	
	return ( negatives == 0 ) || ( positives == 0 );
}

// ============================================================================
/// Calculatyes polygon area. Polygon must be convex.
double convexPolygonArea( const QPolygonF& p )
{
	double total = 0; // sum of products of all sub - triangles
	
	// pRef is the reference point for forming triangles.
	// It's location doesn't change the result (except for rounding error).
	b2Vec2 p1(0.0f, 0.0f);
	
	for( int i = 0; i < p.size(); i++ )
	{
		b2Vec2 p2  = Flyer::point2vec( p[i] );
		b2Vec2 p3  = Flyer::point2vec( p[ (i+1) % p.size() ] );
		
		b2Vec2 e1 = p2 - p1;
		b2Vec2 e2 = p3 - p1;

		double D = b2Cross(e1, e2);

		total += D * 0.5;
	}
	
	return total;
}

// ============================================================================
/// Checks polygon direction. Polygon must be convex.
/// Returns \b true if polygon is clockwise, and thus acceptable as Box2D shape.
/// Returns \b false if polygon needs to be reversed.
bool convexPolygonDirection( const QPolygonF& p )
{
	Q_ASSERT( p.size() > 2 );
	
	b2Vec2 p01 = point2vec( p[1] ) - point2vec( p[0] );
	b2Vec2 p12 = point2vec( p[2] ) - point2vec( p[1] );
	
	return b2Cross( p01, p12 ) < 0;
}

}

// EOF
