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

#include "surface.h"
#include "body.h"
#include "b2dqt.h"
#include "plane.h"

namespace Flyer {

// ============================================================================
// Constructor
Surface::Surface(Machine* pParent, const QString& name): System(pParent, name)
{
	// nope
}

// ============================================================================
// Destructor
Surface::~Surface()
{
}

// ============================================================================
// Damages surface
void Surface::damage(double force)
{
}

// ============================================================================
// Renders surface
void Surface::render( QPainter& painter, const QRectF& rect )
{
	painter.save();
		painter.setTransform( body()->transform(), true );
		painter.setPen( Qt::black );
		
		double wx = cos( ( _inclination*parent()->orientation() ) ) * _width/2;
		double wy = sin( ( _inclination*parent()->orientation() ) ) * _width/2;
		
		painter.drawLine( QLineF( -wx, -wy, wx, wy ) );
		
	painter.restore();
	
	// draw force (DEBUG )
	double fs = 50 ; // force scale
	QPointF wf = aerodynamicForce() / fs;
	const b2Vec2& pos = body()->b2body()->GetPosition();
	
	painter.setPen( Qt::blue );
	painter.drawLine( QLineF( pos.x, pos.y, pos.x + wf.x(), pos.y + wf.y() ) );
}

// ============================================================================
// Simulates surface
void Surface::simulate( double dt )
{
	if ( body() && body()->b2body() )
	{
		QPointF force = aerodynamicForce();
		b2Body* pBody = body()->b2body();
		
		b2Vec2 pos = pBody->GetWorldPoint( point2vec( _position ) );
		pBody->ApplyForce( point2vec( force ), pos );
	}
}

// ============================================================================
/// Clauclates aerodynamic forxe, in world coordinates [N]
QPointF Surface::aerodynamicForce() const
{
	QPointF result;
	if ( body() && body()->b2body() )
	{
		b2Body* pBody = body()->b2body();
		double angle = pBody->GetAngle(); // body angle
	
		b2Vec2 velocity = pBody->GetLinearVelocity();
		double v = velocity.Length();
		double velAngle = atan2( velocity.y, velocity.x );
	
		double attack = angle + (_inclination*parent()->orientation()) - velAngle; // angle of attack
		double sina = sin( attack );

		QPointF force = calculateForce( v, sina ); // f0rce in wing coordinates
		
		result = QPointF
			( force.x()*cos(angle) + force.y()*sin(angle)
			, force.x()*sin(angle) - force.y()*cos(angle)
			);
		
	}
	
	return result;
}

// ============================================================================
//  Sets lift coeff
void Surface::setLiftCoeff( double lift )
{
	_liftCoeff = lift;
	_currentLift = lift;
	// NOTE: implicit repair.
}

// ============================================================================
// Sets drag coeff
void Surface::setDragCoeffV( double drag )
{
	_dragCoeffV = drag;
	_currentDragV = drag;
}

// ============================================================================
// Sets drag coeff
void Surface::setDragCoeffH( double drag )
{
	_dragCoeffH = drag;
	_currentDragH = drag;
}

}
