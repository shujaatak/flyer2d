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

#include <QGLWidget>

#include "worldscene.h"

#include "worldwidget.h"

namespace Flyer
{

// ============================================================================
// Constructor
WorldWidget::WorldWidget ( QWidget* parent ) : QGraphicsView ( parent )
{
	_pScene = new WorldScene( this );
	setScene( _pScene );
	
#ifndef FLYER_NO_OPENGL
	QGLWidget* pViewport = new QGLWidget();
	//pViewport->setAutoBufferSwap( false );
	setViewport( pViewport );
#endif

}

// ============================================================================
// Destructor
WorldWidget::~WorldWidget()
{
}

// ============================================================================
/// Returns player's plane
Plane* WorldWidget::plane() const
{
	return _pScene->plane();
}

// ============================================================================
/// Returns game world
World* WorldWidget::world() const
{
	return _pScene->world();
}

// ============================================================================
// Resize event - handles resizing.
void WorldWidget::resizeEvent( QResizeEvent* )
{
	_pScene->setSceneRect( viewport()->rect() );
	_pScene->resized();
}

} // ns
// EOF


