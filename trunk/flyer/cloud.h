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

#ifndef FLYERCLOUD_H
#define FLYERCLOUD_H

#include "worldobject.h"
#include "Box2D.h"

namespace Flyer
{

/**
	"Cloud" is a short lived, decorational obkject w/o physical properties. Its just semi-transparent
	'cloud' od different colour, dissapearing after while.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Cloud : public WorldObject
{

public:
	Cloud ( World* pWorld );
	~Cloud();

	/// Renders cloud
	virtual void render ( QPainter& painter, const QRectF& rect, const RenderingOptions& options );
	/// Returns object's bounding rect
	virtual QRectF boundingRect() const;

	// properties
	
	void setColor( const QColor& c ) { _color = c; }
	void setRadius( double r ) { _radius = r; }
	void setPosition( const b2Vec2& p ) { _position = p; }
	void setVelocity( const b2Vec2& v ) { _velocity = v; }
	void setExpansion( double e ) { _expansionSpeed = e; }
	void setLifespan( double s ) { _lifespan = s; }

	// predifined cloud types
	
	static Cloud* createSmoke( World* pWorld, const b2Vec2& pos );
	
private:

	// config
	
	QColor	_color;				///< Cloud color
	double	_expansionSpeed;	///< cloud expansion = m/s
	double	_lifespan;			///< Cloud lifetime
	
	b2Vec2	_position;
	b2Vec2	_velocity;
	double	_radius;			///< Initial radius
	
	// cached values
	
	mutable QRectF	_boundingRect;
	
	// variables
	
	double	_birdthDate;	///< Object's birdth date
};

}

#endif // FLYERCLOUD_H

// EOF


