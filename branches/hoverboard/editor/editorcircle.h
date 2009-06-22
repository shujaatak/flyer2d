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

#ifndef EDITORCIRCLE_H
#define EDITORCIRCLE_H

#include <editableitem.h>

/**
 @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class EditorPoint;

class EditorCircle : public EditableItem
{

public:
	EditorCircle();
	virtual ~EditorCircle();

	virtual QRectF boundingRect () const;
	virtual void paint ( QPainter * painter,
		 const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	
	virtual void pointMoved(EditorPoint*);
	
	QPointF center() const;
	double radius() const;
	
	void init( const QPointF& c, double r );

private:

	EditorPoint*	_pCenter;
	EditorPoint*	_pRim;
	QPointF			_rimPos;	///< Relative rim point position
};

#endif // EDITORCIRCLE_H

// EOF


