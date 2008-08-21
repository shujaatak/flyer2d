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

#include "body.h"
#include "b2dqt.h"
#include "plane.h"
#include "controlsurface.h"

namespace Flyer {

// ============================================================================
// Constructor
ControlSurface::ControlSurface( Plane* pParent, const QString& name ): Surface(pParent, name)
{
	_value = 0;
	_currentMaxValue = 1.0;
	_currentMinValue = -1.0;
	_dragCoeffH = 0.0;
	_dragCoeffV = 0.0;
	_currentDragV = 0.0;
	_currentDragH = 0.0;
	_inclination = 0.0;
}

// ============================================================================
// Destructotr
ControlSurface::~ControlSurface()
{
	// npe
}

// ============================================================================
/// Clauclates aerodynamic forxe, in world coordinates [N]
QPointF ControlSurface::aerodynamicForce() const
{
	QPointF result;
	if ( body() && body()->b2body() )
	{
		b2Body* pBody = body()->b2body();
		double bodyAngle = pBody->GetAngle(); // body angle
	
		b2Vec2 velocity = pBody->GetLinearVelocity();
		double v = velocity.Length();
		double velAngle = atan2( velocity.y, velocity.x );
	
		double angle = ( _value*_step + _inclination ) *parent()->orientation();
		
		double attack = angle + bodyAngle - velAngle; // angle of attack
		double sina = sin( attack );

		QPointF force = calculateForce( v, sina ); // f0rce in wing coordinates
		
		result = QPointF
			( force.x()*cos(bodyAngle) + force.y()*sin(bodyAngle)
			, force.x()*sin(bodyAngle) - force.y()*cos(bodyAngle)
			);
		
	}
	
	return result;
}

// ============================================================================
// Calculates aerodynamic force
QPointF ControlSurface::calculateForce( double velocity, double sinAttack ) const
{
	QPointF pos = vec2point( body()->b2body()->GetPosition() );
	
	//double airDensity = parent()->world()->environment()->relativeDensity( pos );
	double airDensity = 1.0;
	
	// lift
	double lift = -airDensity * velocity*velocity * _currentLift * sinAttack;
	
	// drag (negative, becouse backwards)
	double dragH = _currentDragH;
	double drag = -airDensity * velocity*velocity * (dragH +( _currentDragV-dragH) * sinAttack*sinAttack );
	
	return QPointF( drag, lift );	
}

// ============================================================================
/// Damages control surface.
void ControlSurface::damage(double arg1)
{
	qDebug("TODO: implement elevator damage");
}

// ============================================================================
/// Renders
void ControlSurface::render(QPainter& painter, const QRectF& )
{
	painter.save();
		painter.setTransform( body()->transform(), true );
		painter.setPen( Qt::black );
		
		double angle = ( _value*_step + _inclination ) *parent()->orientation();
		double wx = cos( angle ) * _width/2;
		double wy = sin( angle ) * _width/2;
		
		painter.drawLine( QLineF( _position.x()-wx, _position.y()-wy, _position.x()+wx, _position.y()+wy ) );
		
	painter.restore();
	
	// draw force (DEBUG )
	double fs = 500 ; // force scale
	QPointF wf = aerodynamicForce() / fs;
	const b2Vec2& pos = body()->b2body()->GetWorldPoint( point2vec( _position ) );
	
	painter.setPen( Qt::green );
	painter.drawLine( QLineF( pos.x, pos.y, pos.x + wf.x(), pos.y + wf.y() ) );
}

// ============================================================================
/// Sets current value.
void ControlSurface::setValue( double v )
{
	if ( v > _currentMaxValue )
		_value = _currentMaxValue;
	else if ( v < _currentMinValue )
		_value = _currentMinValue;
	else
		_value = v;
}

}
