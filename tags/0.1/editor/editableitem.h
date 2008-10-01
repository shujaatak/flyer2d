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

#ifndef EDITABLEITEM_H
#define EDITABLEITEM_H

#include <QGraphicsItem>

/**
	Thin layer of interface abiuve QGraphicsEvent. Adds 'pointMOved' class, called by popints when dragged.
 @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class EditorPoint;
class EditableWrapper;

class EditableItem : public QGraphicsItem
{

public:
	EditableItem();
	~EditableItem();
	
	virtual void pointMoved( EditorPoint* );
	virtual void pointClicked( EditorPoint* );
	
	void setWrapper( EditableWrapper* pW ){ _pWrapper = pW; }
	
protected:

	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );

	EditableWrapper* _pWrapper;
};

#endif // EDITABLEITEM_H

// EOF


