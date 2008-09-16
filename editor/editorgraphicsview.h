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

#ifndef EDITORGRAPHICSVIEW_H
#define EDITORGRAPHICSVIEW_H

#include <QGraphicsView>

/**
	Edityor's version of QGraphicsView. Draws grid in the background.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class EditorGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	EditorGraphicsView( QWidget* parent );
	virtual ~EditorGraphicsView();

	virtual void drawBackground( QPainter* painter, const QRectF& rect );

};

#endif // EDITORGRAPHICSVIEW_H

// EOF


