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

#ifndef FLYERWORLDSCENE_H
#define FLYERWORLDSCENE_H

#include <QGraphicsScene>
#include <QTimer>

namespace Flyer
{

class Plane;
class World;
class Game;
class GameUI;

/**
Graphics Scene used to render the game world.
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class WorldScene : public QGraphicsScene
{
	Q_OBJECT
public:
	WorldScene( QWidget* pView );
	virtual ~WorldScene();
	
	
	// world an it's elements
	
	Plane* plane() const;
	World* world() const { return _pWorld; }
	Game* game() const { return _pGame; }
	
	// UI
	void addWindow( QWidget*, double z = 0.0 );	///< Adds window to sidplay


	
	// simulation control
	
	void start();
	void stop();
	bool isRunning() const;
	void step();
	
	void restart();
	void respawn();
	
	// zoom levels
	enum Zoom
	{
		ZOOM1,	///< Level 1 - closeup
		ZOOM2,	///< Level 2 - as usual
		ZOOM3	///< Level 3 - far view
	};
	
	
	virtual void resized();
	
public slots:
	void onTimer();

protected:
	virtual void drawBackground ( QPainter * painter, const QRectF & rect );
	
	
	virtual void mouseMoveEvent( QGraphicsSceneMouseEvent* pEvent );
	virtual void mousePressEvent( QGraphicsSceneMouseEvent* pEvent );
	virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent* pEvent );
	virtual void wheelEvent( QGraphicsSceneWheelEvent* pEvent );
	virtual void keyPressEvent( QKeyEvent* pEvent );
	virtual void keyReleaseEvent( QKeyEvent* pEvent );

private:
	void render( QPainter& painter );	///< Renders
	
	void renderMessages( QPainter& painter ); ///< Renders messages
	void adjustTransform();		///< Adjusts zoom and position
	void prepareStillImage();	///< Prepares still imae of the world
	void updateFrame();			///< Repaintrs world
	
	World* _pWorld;
	Game*	_pGame;
	QTimer _timer;
	
	QTransform _transform;
	QPointF	_planePos;			///< Current displayed plane pos in pixels
	
	int _frames;				///< Frame counter
	Zoom _zoom;					///< Current zoom level
	double _lastRenderTime;		///< Last time when rendering started. Helps in FPS calculation
	
	QWidget* _pView;
	GameUI* _pUI;				///< UI manager
	
	QImage	_still;				///< Still image displayed during pause
};

}

#endif // FLYERWORLDSCENE_H

// EOF


