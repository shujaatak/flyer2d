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

#ifndef EDITABLEWRAPPER_H
#define EDITABLEWRAPPER_H

#include <QObject>
class QGraphicsScene;
class EditableItem;

/**
	QObject - derrived wrapper for Flyer ebjects, exposing their properties to Qt meta-class system.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class EditableWrapper : public QObject
{
	Q_OBJECT

public:
	EditableWrapper( QObject* parent, QGraphicsScene* pScene );
	~EditableWrapper();
	
	void setSelected( bool s );
	bool selected() const { return _selected; }
	
	QGraphicsScene* scene() const { return _pScene; }
	
	// messages from item
	void itemSelected();
	void itemDeleted();
	
	/// checks wheter object is valid and can be saved to file for future generations.
	virtual bool isValid() const = 0;
	/// Syncs wrapped object with editors
	virtual void sync() {} 
	
	/// Message from iterm - geometrychanged	
	virtual void itemChanged(){}

signals:
	
	void selected( EditableWrapper* pObject );	///< Selected using mouse
	void showHint( const QString& str );			///< Hint that should be displayed on status bar
	
protected:

	bool _selected;
	QGraphicsScene* _pScene;
	EditableItem*	_pItem;	///< Visualisation
};

#endif // EDITABLEWRAPPER_H

// EOF


