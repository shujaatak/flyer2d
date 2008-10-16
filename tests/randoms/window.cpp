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

#include <math.h>

#include <QApplication>
#include <QPainter>

#include "common.h"

#include "window.h"

Window::Window( QWidget *parent )
		: QWidget( parent )
{
	setupUi( this );
}


Window::~Window()
{
}

// ============================================================================
// PErforms 0-1 test
void Window::on_button01_clicked()
{
	_expectedResults.clear();
	_expectedResults.append( QPointF( 0, 1.0 ) );
	_expectedResults.append( QPointF( 100, 1.0 ) );
	
	_measuredResults = QVector<double>( 100, 0.0 );
	
	resize( 420, 400 );
	update();
	
	for( int i =0 ;i < 10000; i++ )
	{
		Flyer::random01(); // TODO separate test for this!
		double r = Flyer::random01();
		if ( r < 0.0 || r >= 1.0 )
		{
			qDebug("result out of limit: %g", r );
		}
		else
		{
			int x = r * 100;
			_measuredResults[x] += 0.01;
		}
		
		// update each 100 steps
		if ( i % 100 == 0 )
		{
			update();
			QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
	}
	
}


// ============================================================================
// Draws results
void Window::paintEvent( QPaintEvent*  )
{
	if ( _measuredResults.size() == 0 ) return;
	
	double pxPerSample = floor(  double( width() ) / _measuredResults.size() );
	double maxValue = 0.0;
	foreach( QPointF point, _expectedResults )
	{
		maxValue = qMax( point.y(), maxValue );
	}
	
	double pxPerValue = height()*0.5 / maxValue;
	int dataWidth = pxPerSample * _measuredResults.size();
	int x = ( width() - dataWidth ) / 2;
	
	// draw
	QPainter p(this);
	for( int i = 0; i < _measuredResults.size(); i++ )
	{
		QRect r( x, height(), pxPerSample, -_measuredResults[i]*pxPerValue );
		p.fillRect( r, i%2 ? Qt::blue: Qt::black );
		
		x += pxPerSample;
		
	}
	
	// draw results
	p.setPen( Qt::red );
	x = ( width() - dataWidth ) / 2;
	QPolygon expected;
	foreach( QPointF point, _expectedResults )
	{
		int x = ( width() - dataWidth ) / 2 + pxPerSample*point.x();
		int y = height() - point.y()*pxPerValue;
		expected.append( QPoint( x, y ) );
	}
	
	p.setBrush( Qt::NoBrush );
	p.drawPolygon( expected );
	
}

// EOF



