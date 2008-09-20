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

#include <QFileDialog>
#include <QMessageBox>

#include "shape.h"
#include "gexception.h"
#include "editorpolygon.h"
#include "editorcircle.h"
#include "bodywrapper.h"
#include "shapewrapper.h"
#include "textureprovider.h"
#include "bodyprovider.h"

#include "editor.h"

// ============================================================================
//. Tree widget item with asociated QObject object
class ObjectTreeItem : public QTreeWidgetItem
{
public:
	ObjectTreeItem(QTreeWidget * parent) : QTreeWidgetItem(parent, UserType) { _pObject = NULL; }
	ObjectTreeItem(QTreeWidgetItem * parent) : QTreeWidgetItem(parent, UserType) { _pObject = NULL; }
	virtual ~ObjectTreeItem() {}
	
	void setObject( EditableWrapper* pO ) { _pObject = pO ;}
	EditableWrapper* object() const { return _pObject; }
	
private:

	EditableWrapper* _pObject;
	
};

// ============================================================================
// Constructor
Editor::Editor(): QMainWindow()
{
	setupUi( this );
	view->setScene( & _scene );
	QTransform t;
	t.scale( 50, -50 ); // 50 px per meter
	view->setTransform( t );
	_pBodyWrapper = NULL;
	_pSelectedObject = NULL;
	
	newBody();
	
	// debug code
	/*
	EditorPoint* ep = new EditorPoint();
	_scene.addItem( ep );
	view->ensureVisible( ep, 50, 50 );
	EditorPolygon* pPolygon = new EditorPolygon();
	pPolygon->init( 5 );
	_scene.addItem( pPolygon );
	view->ensureVisible( pPolygon, 50, 50 );
	
	EditorCircle* pCircle = new EditorCircle();
	pCircle->init( QPointF( 2,2), 3 );
	_scene.addItem( pCircle );
	
	Flyer::Body* pBody = new Flyer::Body( "jajo" );
	BodyWrapper* pWrapper = new BodyWrapper( pBody, this );
	
	propertyEditor->addObject( pWrapper );
	*/
	
}

// ============================================================================
// Destructor
Editor::~Editor()
{
	// delete body wrapper here, before scene goes doen with t;s items
	if ( _pBodyWrapper ) delete _pBodyWrapper;
}

// ============================================================================
// Creates new body
void Editor::newBody()
{
	select( NULL );
	if ( _pBodyWrapper )
	{
		delete _pBodyWrapper;
		_pBodyWrapper = NULL;
	}
	
	Flyer::Body* pBody = new Flyer::Body();
	_pBodyWrapper = new BodyWrapper( pBody, & _scene );
	connect( _pBodyWrapper, SIGNAL(selected(EditableWrapper*)), SLOT(itemSelected( EditableWrapper*)));
	updateBodyTree();
	_path = QString();
	
}

// ============================================================================
/// Saves body to currently set path.
void Editor::saveBody()
{
	if ( _path.isNull() )
	{
		saveBodyAs();
	}
	else if ( _pBodyWrapper )
	{
		if ( _pBodyWrapper->isValid() )
		{
			try
			{
				_pBodyWrapper->sync();
				_pBodyWrapper->body()->toFile( _path );
			}
			catch ( const GException& e )
			{
				QMessageBox::warning( this, tr("Saving body"),
					QString(tr("Error saving body: %1")).arg( e.getMessage() ) );
			}
		}
		else
		{
			QMessageBox::warning( this, tr("Saving body"), tr("Body is invalid and can't be saved") );
		}
	}
}

// ============================================================================
/// Asks for path, and saves body
void Editor::saveBodyAs()
{
	QFileDialog dialog;
	QString library = Flyer::BodyProvider::libraryPath();
	
	dialog.setDefaultSuffix( "body" );
	dialog.setFilter( "Body (*.body)" );
	dialog.setWindowTitle( tr("Save body") );
	dialog.setFileMode( QFileDialog::AnyFile );
	dialog.setAcceptMode( QFileDialog::AcceptSave );
	dialog.setDirectory( library );
	
	if ( dialog.exec() )
	{
		QString path = dialog.selectedFiles().first();
		if (  _pBodyWrapper )
		{
			if ( _pBodyWrapper->isValid() )
			{
				try
				{
					_pBodyWrapper->sync();
					_pBodyWrapper->body()->toFile( path );
					_path = path;
				}
				catch ( const GException& e )
				{
					QMessageBox::warning( this, tr("Saving body"),
						QString(tr("Error saving body: %1")).arg( e.getMessage() ) );
				}
			}
			else
			{
				QMessageBox::warning( this, tr("Saving body"), tr("Body is invalid and can't be saved") );
			}
		}
	}
}

// ============================================================================
// Zooms in
void Editor::zoomIn()
{
	QTransform zoom;
	zoom.scale( 1.5, 1.5 );
	view->setTransform( zoom, true );
}

// ============================================================================
// Zooms out
void Editor::zoomOut()
{
	QTransform zoom;
	zoom.scale( 0.666, 0.666 );
	view->setTransform( zoom, true );
}

// ============================================================================
// Fits everythinf into screen
void Editor::zoomAll()
{
	view->fitInView( _scene.itemsBoundingRect(), Qt::KeepAspectRatio );
}

// ============================================================================
/// Adds new shape to current body.
void Editor::addPolygonalShape()
{
	if ( _pBodyWrapper )
	{
		b2PolygonDef def;
		def.SetAsBox( 5, 5 );
		
		_pBodyWrapper->addShape( & def );
		
		updateBodyTree();
	}
	// show all items
	zoomAll();
}

// ============================================================================
/// Opens body file
void Editor::openBody()
{
	QString library = Flyer::BodyProvider::libraryPath();
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open body"), library, "Body (*.body)" );
	if ( ! fileName.isNull() )
	{
		if ( _pBodyWrapper )
		{
			select( NULL );
			delete _pBodyWrapper;
			_pBodyWrapper = NULL;
		}
		
		try
		{
			Flyer::Body* pBody = new Flyer::Body();
			pBody->fromFile( fileName );
			_pBodyWrapper = new BodyWrapper( pBody, & _scene, this );
			connect( _pBodyWrapper, SIGNAL(selected(EditableWrapper*)), SLOT(itemSelected( EditableWrapper*)));
			updateBodyTree();
			select( _pBodyWrapper );
			_path = fileName;
			
			// show all items
			zoomAll();
		}
		catch( const GException& e )
		{
			QMessageBox::critical( this, tr("Loading body"), e.getMessage() );
		}
	}
	
	
}

// ============================================================================
/// Adds new shape to current body.
void Editor::addCircularShape()
{
	if ( _pBodyWrapper )
	{
		b2CircleDef def;
		def.radius = 5;
		
		_pBodyWrapper->addShape( & def );

		updateBodyTree();
	}
	// show all items
	zoomAll();
}

// ============================================================================
/// Removes selected shape
void Editor::removeShape()
{
	ShapeWrapper* pShape = qobject_cast< ShapeWrapper* >( _pSelectedObject );
	if ( pShape )
	{
		select( NULL );
		delete pShape;
		updateBodyTree();
	}
}

// ============================================================================
// Loads texture for current body
void Editor::loadTexture()
{
	if ( _pBodyWrapper )
	{
		QString library = Flyer::TextureProvider::libraryPath();
		QString fileName = QFileDialog::getOpenFileName( this, tr("Select texture"), library, "PNG (*.png)" );
		if ( ! fileName.isNull() )
		{
			// strip library path from file name
			if ( fileName.startsWith( library ) )
			{
				fileName.remove( 0, library.length() );
				// remove all leading slashes/backslahses
				while( fileName.startsWith( QDir::separator() ) ) fileName.remove( 0, 1 );
				 
				_pBodyWrapper->loadTexture( fileName );
				zoomAll();
			}
			else
			{
				QMessageBox::critical( this, tr("Loading texture"), 
					QString(tr("Selected texture must be from inside of texture library, which is located in: %1"))
						.arg( library )
					);
			}
		}
	}
}

// ============================================================================
// Exits application
void Editor::exit()
{
}

// ============================================================================
// Adds vertex to polygonal shape
void Editor::shapeAddVertex()
{
	ShapeWrapper* pShape = qobject_cast<ShapeWrapper*>( _pSelectedObject );
	if ( pShape )
	{
		pShape->addVertex();
	}
}

// ============================================================================
// Removes vertex from polygonal shape
void Editor::shapeRemoveVertex()
{
	ShapeWrapper* pShape = qobject_cast<ShapeWrapper*>( _pSelectedObject );
	if ( pShape )
	{
		pShape->removeVertex();
	}
}

// ============================================================================
/// Selects objects which has to be displayed  in property editor.
void Editor::select( EditableWrapper* pObject )
{
	if ( _pSelectedObject )
	{
		_pSelectedObject->setSelected( false );
		disconnect( _pSelectedObject, NULL, statusBar(), NULL );
		statusBar()->setToolTip( "" );
	}
	propertyEditor->setObject( pObject );
	_pSelectedObject = pObject;
	if ( _pSelectedObject )
	{
		_pSelectedObject->setSelected( true );
		connect( _pSelectedObject, SIGNAL(showHint(const QString&)), statusBar(), SLOT(showMessage( const QString& )));
	}
	
}

// ============================================================================
/// Updates body tree view
void Editor::updateBodyTree()
{
	// delete current content
	treeBodyHierarchy->clear();
	_wrapperItems.clear();
	
	// create top-level item for body
	if ( _pBodyWrapper )
	{
		ObjectTreeItem* pBodyItem = new ObjectTreeItem( treeBodyHierarchy );
		pBodyItem->setObject( _pBodyWrapper );
		pBodyItem->setText( 0, "Body" );
		_wrapperItems[ _pBodyWrapper ] = pBodyItem;
	
		// add shapes
		QList< ShapeWrapper* > shapes = _pBodyWrapper->findChildren< ShapeWrapper* >();
		foreach( ShapeWrapper* pShapeWrapper, shapes )
		{
			ObjectTreeItem* pShapeItem = new ObjectTreeItem( pBodyItem );
			pShapeItem->setObject( pShapeWrapper );
			pShapeItem->setText( 0, "Shape" );
			_wrapperItems[ pShapeWrapper ] = pShapeItem;
		}
		
		pBodyItem->setExpanded( true );
	}
}

// ============================================================================
// Handles selection change
void Editor::on_treeBodyHierarchy_itemSelectionChanged()
{
	QList<QTreeWidgetItem *> items = treeBodyHierarchy->selectedItems();
	
	if ( items.size() == 0 )
	{
		select(NULL);
	}
	else
	{
		ObjectTreeItem* pItem = (ObjectTreeItem*)items.first();
		select( pItem->object() );
	}
}

// ============================================================================
// Selectes item
void Editor::itemSelected( EditableWrapper* pObject )
{
	select( pObject );
	// update tree selection
	if ( _wrapperItems.contains( pObject ) )
	{
		treeBodyHierarchy->clearSelection();
		_wrapperItems[ pObject ]->setSelected( true );
	}
}


// EOF


