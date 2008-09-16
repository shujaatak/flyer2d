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

#ifndef FLYERGROUND_H
#define FLYERGROUND_H

#include "worldobject.h"
#include "body.h"


namespace Flyer
{

/**
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Ground : public WorldObject
{
public:
	Ground ( World* pWorld );
	virtual ~Ground();

	virtual void render ( QPainter& painter, const QRectF& rect  );
	virtual void renderOnMap( QPainter& painter, const QRectF& rect );
	double height( double x ) const;					///< Calculates ground height at specified x
	
	void setHeightmap( const QPolygonF& heightMap );	///< Sets heightmap

private:

	// random generation
	struct RGSection	///< Ground section
	{
		double x;			///< x-coord of left point
		double y;			///< y -ccord of left point
		double nextX;		///< x of next point
		double nextY;		///< y of next point
		double prevX;		///< x of prev point
		double prevY;		///< y of prev point
		bool canBeDividedLeft;	///< if section can be divided at left
		bool canBeDividedRight;	///< if section can be divided at right
		
		// settings
		int minSectionSize;	///< minimal section size
		int maxSectionSize;
		double maxHeight;
		double minHeight;
		
		double maxSlope;	///< max slope in this section
		
		// kids
		RGSection* pLeft, *pRight;
		
		RGSection() { pLeft = NULL; pRight = NULL; }
		~RGSection() { delete pLeft; delete pRight; }
	};
	
	void divideRight( RGSection& section ); // divides section on right
	void divideLeft( RGSection& section );	// divides section on left
	static double rand( double start, double end );
	void traverseSection( RGSection& section, QPolygonF& points );
	
	QPolygonF generate( QList<RGSection> seed );

	void random();					///< Generates random ground

	Body* _pGround;					///< Ground body
	QPolygonF	_heightmap;					///< Heightmap - poins of ground surface

	/// Genrates shapes from heightmap
	QList<b2PolygonDef*> createShape();
	b2PolygonDef* createTriangleB2Shape( const QPointF& a, const QPointF& b, const QPointF& c );
	
	QPolygonF	_painterPolygon;		///< Cached outline
};

}

#endif // FLYERGROUND_H

// EOF


