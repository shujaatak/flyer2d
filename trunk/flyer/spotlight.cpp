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

#include "spotlight.h"
#include "body.h"

namespace Flyer {

// ============================================================================
// Constructor
Spotlight::Spotlight(Machine* pParent, const QString& name): System(pParent, name)
{
	_on = false;
	_angle = 0.0;
}

// ============================================================================
// Destructor
Spotlight::~Spotlight()
{
	// nope
}

// ============================================================================
// Damages spotlight
void Spotlight::damage( double force )
{
	if ( _currentRange > 0 )
	{
		double rangeRemoved = _range * ( force / damageCapacity() );
	
		//qDebug("spotlight damaged with: %g", force );
		double propOfDestruction = 5 * rangeRemoved / _currentRange ; // prob of total destruction
		if ( ( qrand() % 1000 ) < ( propOfDestruction * 1000 ) )
		{
			//qDebug("spotlight destroyed, propability was: %g", propOfDestruction);
			_currentRange = 0;
		}
		else
		{
			_currentRange = qMax( 0.0, _currentRange - rangeRemoved );
		}
		
	}
}

// ============================================================================
// Renders
void Spotlight::render(QPainter& painter, const QRectF& )
{
	if ( _on && body() )
	{
		QTransform t = body()->transform();
		t.translate( _position.x(), _position.y() );
		t.rotateRadians( _angle );
		
		QPolygonF lightShape;
		lightShape.append( QPointF( 0,0 ) );
		lightShape.append( QPointF( _currentRange, _currentRange*_width/2 ) );
		lightShape.append( QPointF( _currentRange, -_currentRange*_width/2 ) );
		
		QLinearGradient gradient( QPointF(0,0), QPointF( _currentRange, 0 ) );
		
		QColor colorStart = _color;
		colorStart.setAlpha( 255 );
		QColor colorEnd = _color;
		colorEnd.setAlpha( 0 );
		
		gradient.setColorAt( 0, colorStart );
		gradient.setColorAt( 1, colorEnd );
		
		painter.save();
			
			painter.setTransform( t, true );
			
			// draw light
			painter.setPen( Qt::NoPen );
			painter.setBrush( gradient );
			painter.drawPolygon( lightShape );
			
		painter.restore();
		
	}
	
}

// ============================================================================
// Sets light angle
void Spotlight::setRange( double r )
{
	_range = r;
	_currentRange = r;
}

}
