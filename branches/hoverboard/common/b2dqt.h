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

///\file b2dqt.h - gate between similar Box2D and Qt types
#ifndef FLYERB2DQT_H
#define FLYERB2DQT_H

#include <QPolygonF>
#include "Box2D.h"

namespace Flyer {

/// Converts QPointF to bVec2d
inline b2Vec2 point2vec( const QPointF& p ) { return b2Vec2( p.x(), p.y() ); }

/// Converts b2Vec2 to QPointF
inline QPointF vec2point( const b2Vec2& v ) { return QPointF( v.x, v.y ); }

inline b2AABB rect2aabb( const QRectF& r )
{
	b2AABB ab;
	ab.lowerBound = point2vec( r.topLeft() );
	ab.upperBound = point2vec( r.bottomRight() );
	return ab;
}

/// Creates polygon shape definition useing shape described by QPolygonF
b2PolygonDef shapeToDef( const QPolygonF& shape );

}
#endif // FLYERB2DQT_H
// EOF

