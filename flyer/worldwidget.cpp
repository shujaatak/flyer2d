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
#include "plane.h"
#include "b2dqt.h"
#include "common.h"

#include "worldwidget.h"

namespace Flyer
{

static const double FPS = 10; //const FPS

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
	_timer.setInterval( 1000/FPS );
	_frames = 0;
	_zoom = ZOOM1;
	_lastRenderTime = 0;
	
#ifndef FLYER_NO_OPENGL
	setAutoBufferSwap( false );
#endif

}

// ============================================================================
// Destructor
WorldWidget::~WorldWidget()
{
}


// ============================================================================
// Renders widget
void WorldWidget::render( QPainter& painter )
{
	// calculate fps
	double now = getms();
	double FPS = _lastRenderTime == 0.0 ? 0.0 : 1000.0 / ( now - _lastRenderTime );
	_lastRenderTime = now;
	
	painter.setRenderHint( QPainter::Antialiasing, true );
	painter.setRenderHint( QPainter::HighQualityAntialiasing, true );
	
	painter.save();
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
	painter.restore();
	
	// airpseed
	QString airspeed;
	airspeed.sprintf("airspeed: %.2f km/h", plane()->airspeed() * 3.6 );
	painter.drawText( 10, 10, airspeed );
	
	// altitude
	QString altitude;
	altitude.sprintf("altitude: %.2f m", plane()->position().y() );
	painter.drawText( 10, 25, altitude );
	
	// x
	QString location;
	location.sprintf("location: %.1f km", plane()->position().x() / 1000.0 );
	painter.drawText( 10, 40, location );
	
	// autopilot ( it's time for proper rendering)
	if ( plane()->autopilot() )
	{
		painter.drawText( 10, 55, "autopilot" );
	}
	
	// fps
	QString fps;
	fps.sprintf("FPS: %.1f", FPS );
	painter.drawText( 10, height() - 15, fps );
	
	// throttle
	QString throttle;
	throttle.sprintf("throttle: %d%%", int( plane()->throttle() * 100 ) );
	painter.drawText( width() - 100, 10, throttle );
	
	// flaps
	QString flaps;
	flaps.sprintf("flaps: %d%%", int( plane()->flaps() * 100 ) );
	painter.drawText( width() - 100, 25, flaps );
	
	// pause
	if ( ! isRunning() )
	{
		QRect r( 0, 10, width(), 500 );
		painter.drawText( r
			, Qt::AlignHCenter, QString("PAUSED\nPress P to resume.") );
	}
	
	// redner HUD
	{
	
		QPen hudPen;
		hudPen.setCosmetic( true );
		hudPen.setWidthF( 1 );
		hudPen.setColor( QColor( 128, 128, 128, 128 ) );
		
		double s = sin( plane()->angle() );
		double c = cos( plane()->angle() );
		double spacing = 40; // [px]
		double shortLine = 100; 
		double longLine = 200;
		double x = _planePos.x();
		double y = _planePos.y();
		double o = plane()->orientation();
		// top line
		painter.drawLine( QPointF( x - spacing*s*o, y - spacing*c*o ), QPointF( x - shortLine*s*o, y - shortLine*c*o));
	
		// forward line
		painter.drawLine( QPointF( x + spacing*c, y - spacing*s ), QPointF( x + longLine*c, y - longLine*s));
		
	}
	
	// render messages
	{
		// acquire
		QStringList messages;
		const int MAX_DISPLAYED_MESSAGES = 5;
		double MAX_MESSAGE_AGE = 5.0; // 5 s
		
		double minTime = world()->time() - MAX_MESSAGE_AGE;
		
		int last = plane()->messages().size() - 1; // index of last element
		while( last >= 0 && messages.size() < MAX_DISPLAYED_MESSAGES )
		{
			if ( plane()->messages()[ last ].time() > minTime )
			{
				messages.prepend( plane()->messages()[ last ].text() );
			}
			else
			{
				break;
			}
			
			last --;
		}
		
		
		// display
		QFont messageFont( "Arial" );
		messageFont.setPixelSize( height()/ 30 );
		messageFont.setWeight( QFont::Bold );
		
		QRectF messagesRect( 0, height() * 0.7, width(), height() * 0.3 );
		
		painter.setFont( messageFont );
		painter.setPen( QColor( 0, 0, 0, 192 ) );
		painter.drawText( messagesRect, Qt::AlignHCenter | Qt::AlignBottom, messages.join("\n") );
	}
	
}

// ============================================================================
// On timer. Simulates word
void WorldWidget::onTimer()
{
	_pWorld->simulate( 1.0/FPS );

	adjustTransform();
	updateGL();
	
	_frames ++;
	
	// when autopilot is on - update elevator slider
	if ( plane()->autopilot() )
	{
		emit elevatorChanged( plane()->elevatorAngle() );
		//if ( plane()->autopilot()->mode() == Autopilot::FollowTrack )
		//{
			emit flapsChanged( plane()->flaps() );
			emit throttleChanged( plane()->throttle() );
		//}
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
	repaint();
	_lastRenderTime = 0;
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
/// Returns \b true if simulation is runing
bool WorldWidget::isRunning() const
{
	return _timer.isActive();
}

// ============================================================================
// Adjusts translation to plane position
void WorldWidget::adjustTransform()
{
	QPointF pos = plane()->position();
	//double airspeed = plane()->airspeed();
	b2Vec2 velocity = plane()->linearVelocity();
	int w = width();
	int h = height();
	
	
	// calculate zoom from airspeed
	/*
	double zoomMax; // pixelss per meter for 1000px screen
	double zoomMin = 0.3;
	double zoomPerVelocity = 50.0;	// zoom per m/s
	double minZoomSpeed = 15.0; // speed below zoom is always min
	switch( _zoom )
	{
		case ZOOM1:
			zoomMax = 2.0;
			zoomPerVelocity = 25.0;
			break;
		case ZOOM2:
		default:
			zoomMax = 10.0;
			zoomPerVelocity = 50.0;
			break;
		case ZOOM3:
			zoomMax = 20.0;
			zoomPerVelocity = 100.0;
			break;
	}
	
	double zoom = zoomMax;
	if ( airspeed > ( minZoomSpeed + 1 ) )
	{
		zoom = zoomPerVelocity/(airspeed - minZoomSpeed);
		if ( zoom > zoomMax ) zoom = zoomMax;
		if ( zoom < zoomMin ) zoom = zoomMin;
	}
	// rescale zoom to normalized window size (1000px )
	zoom *= ( w/1000.0 );
	*/
	
	// simplest form - three fixed zoom factors
	double viewportSize = qMax( w, h );
	double metersVisible; // meters in veiwport
	switch( _zoom )
	{
		case ZOOM1:
			metersVisible = 100;
			break;
		case ZOOM2:
		default:
			metersVisible = 250;
			break;
		case ZOOM3:
			metersVisible = 625;
			break;
	}
	double zoom = viewportSize / metersVisible;
	
	// calculate position from velocity vector
	//double velMax = 50.0;
	//double relPosX = qMax( -1.0, qMin( 1.0, velocity.x / velMax ) );
	//double relPosY = qMax( -0.5, qMin( 1.0, velocity.y / velMax ) );
	
	// desired plane position, in pixels
	/* i'l lthink i'll withdraw all the motion
	double posX = w * ( 0.5 - 0.3*relPosX );
	double posY = h * ( ( fabs( velocity.x ) > 20.0 ) ? ( 0.35 + 0.15*relPosY ) : 0.5 );
	*/
	double posX = w * 0.5;
	double posY = h * ( ( fabs( velocity.x ) > 30.0 ) ? 0.35  : 0.5 );
	
	// modify _planePos to make it closer to 
	double maxSpeedX = 300 * _pWorld->timestep(); /// max speed in pixels/per second * timestep
	double maxSpeedY = 50 * _pWorld->timestep(); /// max speed in pixels/per second * timestep
	double ex = qMax( -maxSpeedX, qMin( maxSpeedX, _planePos.x() - posX ));
	double ey = qMax( -maxSpeedY, qMin( maxSpeedY, _planePos.y() - posY ));
	
	_planePos.rx()  -= ex;
	_planePos.ry()  -= ey;
	//qDebug("ex: %g, ey: %g. desired pos: %g,%g, current pos: %g,%g", ex, ey, posX, posY,  _planePos.x(), _planePos.y() );
	
	
	QTransform t;
	t.scale( zoom , -zoom ); // rescale zoom to normalize 1000px window
	t.translate( -pos.x() + _planePos.x()/zoom, -pos.y()- _planePos.y()/zoom );
	
	_transform = t;
}

// ============================================================================
// Resize event - handles resizing.
void WorldWidget::resizeEvent( QResizeEvent* )
{
	_planePos = QPointF( width()/2.0, height() / 2.0 );
	adjustTransform();
}

// ============================================================================
// Handles mouse movement, Implements mouse steering.
void WorldWidget::mouseMoveEvent( QMouseEvent* pEvent )
{
	if ( ! plane()->autopilot() )
	{
		double y = 2* ( - pEvent->y() / double(height()) + 0.5 );
		//double e = y > 0 ? y*y : -y*y; // TODO expoeriment - use 2nd power as input function (3rd is too big, I've checked)
		double e = y; // neeeeeeeeeey, linear is teh best!
		plane()->setElevator( e );
		emit elevatorChanged( e );
	}
	setFocus(); // steal focus
}

// ============================================================================
// Handles mouse press.
void WorldWidget::mousePressEvent( QMouseEvent* pEvent )
{
	switch ( pEvent->button() )
	{
	
	// Left
	case Qt::LeftButton:
		plane()->setFiring( true );
		break;
		
	// right
	case Qt::RightButton:
		plane()->releaseWeapon();
		break;
		
	
	default:
		pEvent->ignore();
	}
}

// ============================================================================
// Handles mouse release.
void WorldWidget::mouseReleaseEvent( QMouseEvent* pEvent )
{
	switch ( pEvent->button() )
	{
	
	// Left
	case Qt::LeftButton:
		plane()->setFiring( false );
		break;
		
	
	default:
		pEvent->ignore();
	}
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
	if ( pEvent->isAutoRepeat() ) return;
	
	switch( pEvent->key() )
	{
	// SPACE
	case Qt::Key_Space:
		plane()->flipPlane();
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
		
	// pg up - zoom out
	case Qt::Key_PageUp:
		_zoom = Zoom( qMin( _zoom+1, int(ZOOM3) ) );
		break;
	
	// pg dwon - zoom in
	case Qt::Key_PageDown:
		_zoom = Zoom( qMax( _zoom-1, int(ZOOM1) ) );
		break;
	
	// puse
	case Qt::Key_P:
		if ( isRunning() ) stop();
		else start();
		break;
	
	default:
		pEvent->ignore();
	}
}

// ============================================================================
// Handles key release
void WorldWidget::keyReleaseEvent( QKeyEvent* pEvent )
{
	if ( pEvent->isAutoRepeat() ) return;
	
	switch( pEvent->key() )
	{
	// B
	case Qt::Key_B:
		plane()->applyWheelBrake( false );
		break;
	
	default:
		pEvent->ignore();
	};

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
	// TODO, no my computer has it working, but this returns false
	//if ( format().sampleBuffers() )
	//{
		glEnable( GL_LINE_SMOOTH );
	//	glEnable( GL_POINT_SMOOTH );
		//glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	//}
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

}

// ============================================================================
// paints GL
void WorldWidget::paintGL()
{
	QPainter painter( this );
	render( painter );
	swapBuffers();
}
#endif

} // ns
// EOF


