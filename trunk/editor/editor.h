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
#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QGraphicsScene>

#include "body.h"
#include "editablewrapper.h"

#include "ui_editor.h"

class BodyWrapper;

/**
 @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Editor : public QMainWindow, Ui::Editor
{
	Q_OBJECT
public:
	Editor();
	virtual ~Editor();

public slots:

	// actions
	
	virtual void on_actionNewBody_triggered() { newBody(); }
	virtual void on_actionAddPolyShape_triggered() { addPolygonalShape(); }
	virtual void on_actionAddCircShape_triggered() { addCircularShape(); }
	virtual void on_actionRemoveShape_triggered() { removeShape(); }
	virtual void on_actionShapeAddVertex_triggered() { shapeAddVertex(); }
	virtual void on_actionShapeRemoveVertex_triggered() { shapeRemoveVertex(); }
	virtual void on_actionLoadTexture_triggered() { loadTexture(); }
	virtual void on_actionSaveBodyAs_triggered() { saveBodyAs(); }
	virtual void on_actionSaveBody_triggered() { saveBody(); }
	virtual void on_actionOpenBody_triggered() { openBody(); }
	
	// other
	
	virtual void on_treeBodyHierarchy_itemSelectionChanged();
	void itemSelected( EditableWrapper* pObject );
 	
private:
	
	// operations
	
	void newBody();
	void saveBody();
	void saveBodyAs();
	void openBody();
	void addPolygonalShape();
	void addCircularShape();
	void removeShape();
	void loadTexture();
	void exit();
	
	void shapeAddVertex();
	void shapeRemoveVertex();

	void select( EditableWrapper* );	///< Selects object
	void updateBodyTree();				///< Updates body tree view

	// data

	QGraphicsScene	_scene;
	
	// variables
	
	QString				_path;				///< Cuyrrent path
	BodyWrapper*		_pBodyWrapper;		///< Edited body
	EditableWrapper*	_pSelectedObject;	///< Currently selected wrapper for property editor
	
	/// Reverse mapping between wrappers and tree items. This sucvks, I should be using MVC here
	QMap< EditableWrapper*, QTreeWidgetItem* > _wrapperItems;
};

#endif // EDITOR_H

// EOF


