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

#ifndef FLYEREXPLOSION_H
#define FLYEREXPLOSION_H


#include <QList>
#include <QVector>
#include "worldobject.h"

#include "Box2D.h"

namespace Flyer
{

class DamageManager;

/**
	Simulatyes explosion. Draws itself, and acts with appropriate force on each object within range,
	inflicts damage.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Explosion : public WorldObject
{

public:
	Explosion ( World* pWorld );
	virtual ~Explosion();

	virtual QRectF boundingRect() const;
	virtual void render ( QPainter& painter, const QRectF& rect );
	virtual void simulate ( double dt );
	
	// properties
	void setCenter( const QPointF& c ) { _center = c; }
	void setEnergy( double e );
	
private:

	void actWithForce();	///< Pushes bodies, activates damage managers for current step

	// config
	double _speed;			///< Speed of expansions [m/s], defaults to speed of sound
	double _energy;			///< Explosion energy
	QPointF _center;		///< Explosion center
	double _maxRadius;		///< Max radius
	double _maxFireRadius;	///< max radius of fire
	
	// variables
	double	_radius;		///< Current radius
	int		_currentStep;	///< Current simulation step

};

}

#endif // FLYEREXPLOSION_H

// EOF


