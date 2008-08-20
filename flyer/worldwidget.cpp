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

#include <QMouseEvent>
#include <QPainter>

#include "world.h"

#include "worldwidget.h"
#include "plane.h"

namespace Flyer
{

// ============================================================================
// Constructor
#ifdef FLYER_NO_OPENGL
WorldWidget::WorldWidget ( QWidget* parent, Qt::WindowFlags f ) : QWidget( parent, f )
#else
WorldWidget::WorldWidget ( QWidget* parent, Qt::WindowFlags f ) : QGLWidget ( parent, NULL, f )
#endif
{
	connect( & _timer, SIGNAL(timeout()), SLOT(onTimer()) );
	
	
	_pWorld = new World();
	
	// init transform
	adjustTransform();
	
	// simulate
	_timer.setInterval( 100 );
	_frames = 0;
}

// ============================================================================
// Destructor
WorldWidget::~WorldWidget()
{
}


// ============================================================================
// Paints event.
/*
void WorldWidget::paintEvent( QPaintEvent* pEvent )
{
	QPainter painter( this );
	
	render( painter );
}
*/
// ============================================================================
// Renders widget
void WorldWidget::render( QPainter& painter )
{
	painter.setRenderHint( QPainter::Antialiasing, true );
	painter.setRenderHint( QPainter::HighQualityAntialiasing, true );
	
	// white background
	painter.fillRect( rect(), Qt::white );
	
	// airpseed
	QString airspeed;
	airspeed.sprintf("airspeed: %.2f km/h", plane()->airspeed() * 3.6 );
	painter.drawText( 10, 10, airspeed );
	
	// altitude
	QString altitude;
	altitude.sprintf("altitude: %.2f m", plane()->pos().y() );
	painter.drawText( 10, 25, altitude );
	
	// x
	QString location;
	location.sprintf("location: %.1f km", plane()->pos().x() / 1000.0 );
	painter.drawText( 10, 40, location );
	
	// autopilot ( it's time for proper rendering)
	if ( plane()->autopilot() )
	{
		painter.drawText( 10, 55, "autopilot" );
	}
	
	
	painter.setTransform( _transform );
	
	_pWorld->render( painter, _transform.inverted().mapRect( rect() ) );
	
	// render map
	QRectF worldRect = _pWorld->boundary();
	
	QTransform mapTransform;
	mapTransform.translate( 10, height() - 10 );
	double mapScale = 40.0 / worldRect.height();
	mapTransform.scale( mapScale, - mapScale );
	mapTransform.translate( -worldRect.left(), -worldRect.top() );
	
	//painter.setTransform( mapTransform, false );
	QRectF mapRect = mapTransform.mapRect( worldRect );
	
	painter.setTransform( mapTransform, false );
	painter.setPen( Qt::black );
	painter.setBrush( Qt::yellow );
	painter.drawRect( worldRect );
	
	_pWorld->renderMap( painter, worldRect );
}

// ============================================================================
// On timer. Simulates word
void WorldWidget::onTimer()
{
	_pWorld->simulate( 0.1 );

	adjustTransform();
	repaint();
	
	_frames ++;
	
	// when autopilot is on - update elevator slider
	if ( plane()->autopilot() )
	{
		emit elevatorChanged( plane()->elevatorAngle() );
	}
}

// ============================================================================
// Start simulation
void WorldWidget::start()
{
	_timer.start();
}

// ============================================================================
// Stop simualtion
void WorldWidget::stop()
{
	_timer.stop();
}

// ============================================================================
// Single simulation step
void WorldWidget::step()
{
	QPixmap pixmap( size() );
	QPainter painter( & pixmap );
	render( painter );
	
	pixmap.save( QString("step%1.png").arg( _frames ) );
	if ( ! _timer.isActive() )
	{
		onTimer();
	}
	
}

// ============================================================================
// updates viewport so plane is visible
/*
void WorldWidget::makeSurePlaneVisible()
{
	QPointF pos = _pPlane->pos();
	
	// margin
	QPointF bottomRight = _transform.inverted().map( rect().bottomRight() );
	QPointF topLeft = _transform.inverted().map( rect().topLeft() );
	
	double w = bottomRight.x() - topLeft.x();
	double h = topLeft.y() - bottomRight.y();
	
	double maxx = topLeft.x() + 0.7*w;
	double minx = topLeft.x() + 0.3*w;
	
	double maxy = bottomRight.y() + 0.7*h;
	double miny = bottomRight.y() + 0.3*h;
	
	// near right margin?
	if ( pos.x() > maxx )
	{
		_transform.translate( maxx - pos.x(), 0 );
	}
	// near left
	else if ( pos.x() < minx )
	{
		_transform.translate( minx - pos.x(), 0 );
	}
	
	// near top?
	if ( pos.y() > maxy )
	{
		_transform.translate( 0, maxy - pos.y() );
	}
	// near bottom?
	else if ( pos.y() < miny )
	{
		_transform.translate( 0, miny - pos.y() );
	}
	
}*/

// ============================================================================
// Adjusts translation to plane position
void WorldWidget::adjustTransform()
{
	QPointF pos = plane()->pos();
	double airspeed = plane()->airspeed();
	
	double zoomMax = 10.0; // pixels per meter
	double zoomMin = 1.0;
	
	double zoom = zoomMax;
	if ( airspeed > 1 )
	{
		zoom = 100.0/airspeed;
		if ( zoom > zoomMax ) zoom = zoomMax;
		if ( zoom < zoomMin ) zoom = zoomMin;
	}
	QTransform t;
	int w = width();
	int h = height();
	
	
	t.scale( zoom, -zoom );
	t.translate( -pos.x() + w/(zoom*2), -pos.y()- h/zoom/2 );
	
	_transform = t;
}

// ============================================================================
// Resize event - handles resizing.
void WorldWidget::resizeEvent( QResizeEvent* )
{
	adjustTransform();
}

// ============================================================================
// Handles mouse movement, Implements mouse steering.
void WorldWidget::mouseMoveEvent( QMouseEvent* pEvent )
{
	if ( ! plane()->autopilot() )
	{
		double e = 2* ( - pEvent->y() / double(height()) + 0.5 );
		plane()->setElevator( e );
		emit elevatorChanged( e );
	}
	setFocus(); // steal focus
}

// ============================================================================
// Handles mouse press.
void WorldWidget::mousePressEvent( QMouseEvent* pEvent )
{
	pEvent->ignore();
}
// ============================================================================
// Handles wheel. controls thrtottle
void WorldWidget::wheelEvent( QWheelEvent* pEvent )
{
	double step = pEvent->delta() / 1200.0; // single click - 120, stepping in 0.1 units
	
	plane()->setThrottle( plane()->throttle() + step );
	emit throttleChanged( plane()->throttle() );
}

// ============================================================================
// Handles key press
void WorldWidget::keyPressEvent( QKeyEvent* pEvent )
{
	switch( pEvent->key() )
	{
	// SPACE
	case Qt::Key_Space:
		plane()->flip();
		break;
		
	// V
	case Qt::Key_V:
		plane()->setFlaps( plane()->flaps() + 0.33 );
		emit flapsChanged( plane()->flaps() );
		break;
	
	// F
	case Qt::Key_F:
		plane()->setFlaps( plane()->flaps() - 0.33 );
		emit flapsChanged( plane()->flaps() );
		break;
		
	// B
	case  Qt::Key_B:
		plane()->applyWheelBrake( true );
		break;
		
	// A
	case Qt::Key_A:
		plane()->setAutopilot( ! plane()->autopilot() );
		break;
	
	default:
		pEvent->ignore();
	}
}

// ============================================================================
// Handles key release
void WorldWidget::keyReleaseEvent( QKeyEvent* pEvent )
{
	if( pEvent->key() == Qt::Key_B )
	{
		plane()->applyWheelBrake( false );
		pEvent->accept();
	}

}

// ============================================================================
// Returns player's plane
Plane* WorldWidget::plane() const
{
	return _pWorld->playerPlane();
}

#ifdef FLYER_NO_OPENGL
// ============================================================================
// Paints content
void WorldWidget::paintEvent( QPaintEvent* )
{
	QPainter painter( this );
	render( painter );
}

#else
// ============================================================================
// Initializes opengl for drawing
void WorldWidget::initializeGL()
{
	// turn on antialiasing, if available
	if ( format().sampleBuffers() )
	{
		glEnable( GL_LINE_SMOOTH );
		glEnable( GL_POINT_SMOOTH );
		glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	}
}

// ============================================================================
// paints GL
void WorldWidget::paintGL()
{
	QPainter painter( this );
	render( painter );
}
#endif

} // ns
// EOF


