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
#include <QPainter>

#include "body.h"
#include "bodyprovider.h"
#include "gexception.h"
#include "common.h"

#include "window.h"

// ============================================================================
// Constructor
Window::Window( QWidget *parent )
		: QWidget( parent )
{
	setupUi( this );
}

// ============================================================================
// Destructor
Window::~Window()
{
}

// ============================================================================
// paint event
void Window::paintEvent( QPaintEvent* e )
{
	// first things first - paint widgets
	QWidget::paintEvent( e );
	
	// now - paint all polygons
	QList<QColor> colors;
	colors << QColor( Qt::red ) << QColor( Qt::blue ) << QColor( Qt::green );
	
	QRectF bb;
	
	foreach( const QPolygonF& polygon, _polygons )
	{
		if ( bb.isNull() )
		{
			bb = polygon.boundingRect();
		}
		else
		{
			bb |=  polygon.boundingRect();
		}
	}
	
	double scale =  qMin( 0.8 *width() / bb.width(), 0.8 *height()/ bb.height() );
	QPainter painter( this );
	painter.setRenderHint( QPainter::Antialiasing, true );
	
	// bkgnd
	QRectF wa( width()*0.05, height()*0.05, width()*0.9, height()*0.9 );
	painter.fillRect( wa, Qt::white );
	
	// transform
	painter.translate( (width() /*- bb.width()*scale*/) / 2 , ( height()/*-bb.height()/scale*/ )/2 );
	painter.scale( scale, -scale );
	painter.translate( - bb.center() );
	
	// polygons
	painter.setBrush( Qt::NoBrush );
	
	for( int i =0; i < 	_polygons.size(); i++ )
	{
		painter.setPen( colors[i%colors.size()] );
		painter.drawPolygon( _polygons[i] );
		//qDebug("Drawing %d-gon", _polygons[i].size() ); // TODO debug
	}
	
	// lines
	
	painter.setPen( Qt::DashDotLine );
	for ( int j = 0; j < _lines.size(); j++ )
	{
		painter.setPen( colors[j%colors.size()] );
		painter.drawLine( _lines[j] );
	}
}

// ============================================================================
// Loads body
void Window::on_buttonLoad_clicked()
{
	QString library = Flyer::BodyProvider::libraryPath();
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open body"), library, "Body (*.body)" );
	if ( ! fileName.isNull() )
	{
		try
		{
			Flyer::Body body;
			body.fromFile( fileName );
			
			// loaded, extract outline from body
			_polygons.clear();
			_lines.clear();
			_polygons.append( body.outline() );
			
			update();
			
		}
		catch( const GException& e )
		{
			QMessageBox::critical( this, tr("Loading body"), e.getMessage() );
		}
	}
}

// ============================================================================
// Splits each body randomly
void Window::on_buttonSplit_clicked()
{
	QList<QPolygonF> splited;
	
	foreach( const QPolygonF& polygon, _polygons )
	{
		QLineF line;
		QList<QPolygonF> result = Flyer::splitPolygonRandomly( polygon, &line );
		splited += result;
		if ( result.size() == 1 )
		{
			_lines += line;
		}
	}
	
	_polygons = splited;
	update();
}

// ============================================================================
// Triangulates each polygon.
void Window::on_buttonTriangulate_clicked()
{
	QList<QPolygonF> trinagulated;
	
	foreach( const QPolygonF& polygon, _polygons )
	{
		QList<QPolygonF> result = Flyer::triangulatePolygon( polygon );
		trinagulated += result;
	}
	
	_polygons = trinagulated;
	update();
}

// EOF

