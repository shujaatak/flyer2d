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

#include <QPainter>

#include "Box2D.h"

#include "engine.h"

namespace Flyer
{

// ============================================================================
// Constructor
Engine::Engine ( const QString& name, double thrust, const QPointF& normal /* = QPointF( 1.0, 0.0 )*/ )
	 : System ( name )
{
	_maxThrust	= thrust;
	_currentMaxThrust = thrust;
	_normal		= normal;
}

// ============================================================================
// Destructor
Engine::~Engine()
{
}

// ============================================================================
// Sets throttle
void Engine::setThrottle( double t )
{
	if ( t < 0 )
		_throttle = 0.0;
	else if ( t > 1.0 )
		_throttle = 1.0;
	else 
		_throttle = t;
}


// ============================================================================
// Applies damage
void Engine::damage ( double force )
{
	// currently only damag is reducing max thrust
	_currentMaxThrust -=  _maxThrust * (force / damageCapacity() );
	
	if( _currentMaxThrust < 0.0 )
	{
		_currentMaxThrust = 0.0;
	}
	

}

// ============================================================================
// Simulates engine operation.
void Engine::simulate ( double dt )
{
	Q_ASSERT( body() );
	
	const b2Vec2& pos = body()->GetPosition();
	
	QPointF thrust	= thrustForce();
	body()->ApplyForce
		( 10.0 * b2Vec2( thrust.x(), thrust.y() ) // newtons per kg
		, pos );
}

// ============================================================================
// Thrust force (in kg)
QPointF Engine::thrustForce()
{
	double angle = body()->GetAngle();
	
	double thrust = _throttle * _currentMaxThrust;
	
	// thrust along the normal vector
	QTransform t;
	t.rotateRadians( angle ); 
	
	return t.map( _normal*thrust );
}

// ============================================================================
// Renders
void Engine::render( QPainter& painter, const QRectF& rect )
{
	Q_ASSERT( body() );
	
	// render body
	renderBody( painter );

	// render force (debug )
	double fs = 50 ; // force scale
	QPointF tf = thrustForce() / fs;
	const b2Vec2& pos = body()->GetPosition();

	painter.setPen( Qt::red );
	painter.drawLine( QLineF( pos.x, pos.y, pos.x + tf.x(), pos.y + tf.y() ) );
	
}


}
