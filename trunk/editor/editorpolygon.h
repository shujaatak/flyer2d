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

#ifndef EDITORPOLYGON_H
#define EDITORPOLYGON_H

#include "editableitem.h"

/**
 @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class EditorPoint;

class EditorPolygon : public EditableItem
{

public:
	EditorPolygon();
	~EditorPolygon();

	virtual QRectF boundingRect () const;
	virtual void paint ( QPainter * painter,
		 const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

	QPolygonF polygon() const;
	void setPolygon( const QPolygonF& p );
	
	void init( int points );	///< inits polygon with specified number of points

	virtual void pointMoved(EditorPoint*);

	static bool isConvex( const QPolygonF& p );

private:

	QList<EditorPoint*> _points;
	
	
};

#endif // EDITORPOLYGON_H

// EOF


