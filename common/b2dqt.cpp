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

namespace Flyer {

// ============================================================================
// QPolygonF - > b2PolygonDef
b2PolygonDef shapeToDef( const QPolygonF& shape, bool reversed )
{
	b2PolygonDef def;
	def.vertexCount = 0;
	
	if ( shape.size() > 8 )
	{
		qWarning("Shape has too many vertices");
		return def;
	}
	
	def.vertexCount = shape.size();
	for( int i = 0; i < def.vertexCount; i++ )
	{
		int vertexIndex = reversed ? (def.vertexCount - i - 1) : i;
		def.vertices[vertexIndex].Set( shape[i].x(), shape[i].y() );
	}
	
	return def;
}

}

// EOF

