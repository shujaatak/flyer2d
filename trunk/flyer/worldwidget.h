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

#ifndef WORLDWIDGET_H
#define WORLDWIDGET_H

#include <QGLWidget>
#include <QTransform>
#include <QTimer>


namespace Flyer
{

class Plane;
class World;

/**
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
#ifdef FLYER_NO_OPENGL
class WorldWidget : public QWidget
#else
class WorldWidget : public QGLWidget
#endif
{
	Q_OBJECT
public:
	WorldWidget ( QWidget* parent = 0, Qt::WindowFlags f = 0);

	virtual ~WorldWidget();
	
	// world an it's elements
	
	Plane* plane() const;
	World* world() const { return _pWorld; }
	
	// simulation control
	
	void start();
	void stop();
	bool isRunning() const;
	void step();
	
	// zoom levels
	enum Zoom
	{
		ZOOM1,	///< Level 1 - closeup
		ZOOM2,	///< Level 2 - as usual
		ZOOM3	///< Level 3 - far view
	};
	
	
signals:

	void elevatorChanged( double elevator );
	void throttleChanged( double throttle );
	void flapsChanged( double flaps );
	
protected:

#ifdef FLYER_NO_OPENGL
	virtual void paintEvent( QPaintEvent* pEvent );
#else
	virtual void initializeGL();
	virtual void paintGL();
#endif
	virtual void resizeEvent( QResizeEvent* );
	
	virtual void mouseMoveEvent( QMouseEvent* pEvent );
	virtual void mousePressEvent( QMouseEvent* pEvent );
	virtual void mouseReleaseEvent( QMouseEvent* pEvent );
	virtual void wheelEvent( QWheelEvent* pEvent );
	virtual void keyPressEvent( QKeyEvent* pEvent );
	virtual void keyReleaseEvent( QKeyEvent* pEvent );
	
private slots:

	void onTimer();

private:
	
	void render( QPainter& painter );	///< Renders
	
	void adjustTransform();		///< Adjusts zoom and position
	
	
	World* _pWorld;
	
	QTransform _transform;
	QTimer _timer;
	QPointF	_planePos;			///< Current displayed plane pos in pixels
	
	int _frames;				///< Frame counter
	Zoom _zoom;					///< Current zoom level
	double _lastRenderTime;		///< Lat time when rendering started. Helps in FPS calculation
	
};

}

#endif // WORLDWIDGET_H

// EOF


