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
	double res =  double(qrand() % 0xffff) / (double)0xffff;
	//qDebug("random01: %g", res);
	return res;
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
QList<QPolygonF> splitPolygonRandomly( const QPolygonF& polygon, QLineF* pdebugout )
{
	QRectF br = polygon.boundingRect();
	
	// get random angle on cutting
	double angle = random01()* 6.28;
	
	// cut throught the center
	QPointF center = br.center();
	double upper = br.width() + br.height(); // upper bound for any of the polygon's dimensions
	
	QPointF p1( center.x() - upper*sin(angle), center.y() - upper*cos(angle) );
	QPointF p2( center.x() + upper*sin(angle), center.y() + upper*cos(angle) );
	
	// debug
	if ( pdebugout ) *pdebugout = QLineF( p1, p2 );
	
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
	for( int i = ( interIndex1 + 1 ) % polygon.size();
		 i != ( interIndex2 + 1 ) % polygon.size();
		i = ( i+1) % polygon.size() )
	{
		res1.append( polygon[i] );
	}
	res1.append( intersection2 );
	
	res2.append( intersection2 );
	for( int i = ( interIndex2 + 1 ) % polygon.size();
		i != ( interIndex1 + 1 ) % polygon.size();
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
	//qDebug("Triangulating polygon with %d vertices", polygon.size() );
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
	
	//qDebug("%d triangles created", triangles.size() );
	
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

		total += fabs( D * 0.5 ); // NOTE fabs here makes it direction-resitant, but vulnelabre for concave shapes
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

// ============================================================================
/// Checks shape validity. Returns 0 if valid.
///\author Erin Catto, Daid
///\note Taken from Boxcuter, licensed n Box2D license.
int CheckPolyShape(const b2PolygonDef* poly)
{
	if (!(3 <= poly->vertexCount && poly->vertexCount <= b2_maxPolygonVertices))
		return -1;

	b2Vec2 m_normals[poly->vertexCount];

	// Compute normals. Ensure the edges have non-zero length.
	for (int32 i = 0; i < poly->vertexCount; ++i)
	{
		int32 i1 = i;
		int32 i2 = i + 1 < poly->vertexCount ? i + 1 : 0;
		b2Vec2 edge = poly->vertices[i2] - poly->vertices[i1];
		if (!(edge.LengthSquared() > B2_FLT_EPSILON * B2_FLT_EPSILON))
			return -1;
		m_normals[i] = b2Cross(edge, 1.0f);
		m_normals[i].Normalize();
	}

	// Ensure the polygon is convex.
	for (int32 i = 0; i < poly->vertexCount; ++i)
	{
		for (int32 j = 0; j < poly->vertexCount; ++j)
		{
			// Don't check vertices on the current edge.
			if (j == i || j == (i + 1) % poly->vertexCount)
			{
				continue;
			}
			
			// Your polygon is non-convex (it has an indentation).
			// Or your polygon is too skinny.
			float32 s = b2Dot(m_normals[i], poly->vertices[j] - poly->vertices[i]);
			if (!(s < -b2_linearSlop))
				return -1;
		}
	}

	// Ensure the polygon is counter-clockwise.
	for (int32 i = 1; i < poly->vertexCount; ++i)
	{
		float32 cross = b2Cross(m_normals[i-1], m_normals[i]);

		// Keep asinf happy.
		cross = b2Clamp(cross, -1.0f, 1.0f);

		// You have consecutive edges that are almost parallel on your polygon.
		float32 angle = asinf(cross);
		if (!(angle > b2_angularSlop))
			return -1;
	}

	// Compute the polygon centroid.
	b2Vec2 m_centroid; m_centroid.Set(0.0f, 0.0f);
	float32 area = 0.0f;

	// pRef is the reference point for forming triangles.
	// It's location doesn't change the result (except for rounding error).
	b2Vec2 pRef(0.0f, 0.0f);

	const float32 inv3 = 1.0f / 3.0f;

	for (int32 i = 0; i < poly->vertexCount; ++i)
	{
		// Triangle vertices.
		b2Vec2 p1 = pRef;
		b2Vec2 p2 = poly->vertices[i];
		b2Vec2 p3 = i + 1 < poly->vertexCount ? poly->vertices[i+1] : poly->vertices[0];

		b2Vec2 e1 = p2 - p1;
		b2Vec2 e2 = p3 - p1;

		float32 D = b2Cross(e1, e2);

		float32 triangleArea = 0.5f * D;
		area += triangleArea;

		// Area weighted centroid
		m_centroid += triangleArea * inv3 * (p1 + p2 + p3);
	}

	// Centroid
	if (!(area > B2_FLT_EPSILON))
		return -1;
	m_centroid *= 1.0f / area;

	// Compute the oriented bounding box.
	//ComputeOBB(&m_obb, m_vertices, m_vertexCount);

	// Create core polygon shape by shifting edges inward.
	// Also compute the min/max radius for CCD.
	for (int32 i = 0; i < poly->vertexCount; ++i)
	{
		int32 i1 = i - 1 >= 0 ? i - 1 : poly->vertexCount - 1;
		int32 i2 = i;

		b2Vec2 n1 = m_normals[i1];
		b2Vec2 n2 = m_normals[i2];
		b2Vec2 v = poly->vertices[i] - m_centroid;

		b2Vec2 d;
		d.x = b2Dot(n1, v) - b2_toiSlop;
		d.y = b2Dot(n2, v) - b2_toiSlop;

		// Shifting the edge inward by b2_toiSlop should
		// not cause the plane to pass the centroid.

		// Your shape has a radius/extent less than b2_toiSlop.
		if (!(d.x >= 0.0f))
			return -1;
		if (!(d.y >= 0.0f))
			return -1;
	}
	
	return 0;
}

}

// EOF
