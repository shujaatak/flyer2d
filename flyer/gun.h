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

#ifndef FLYERGUN_H
#define FLYERGUN_H

#include <QPointF>
#include "system.h"

namespace Flyer
{

/**
	Gun is a device that fires bullets in defined direction and speed
	from point defined as 'muzzle' define in attached body coordinates.
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Gun : public System
{
public:
	Gun ( Machine* pParent, const QString& name = "" );
	virtual ~Gun();

	virtual void damage ( double force );
	virtual void simulate( double dt );
	virtual double status() const;
	
	// config
	void setBulletMass( double m ) { _mass = m; }
	void setBulletSize( double s ) { _size = s; }
	void setBulletLifespan( double l ) { _lifespan = l; }
	
	void setMuzzle( const QPointF& m ) { _muzzle = m; }
	QPointF muzzle() const { return _muzzle; }
	void setNormal( const QPointF& n ) { _normal = n; }
	QPointF normal() const { return _normal; }
	void setBulletVelocity( double v ) { _velocity = v; _currentVelocity = v;}
	void setFiringInterval( double i ) { _interval = i; _currentnInterval = i;}
	
	// actions
	void setFiring( bool firing ) { _firing = firing; }
	bool firing() const { return _firing; }
	
	// predefined guns
	
	static Gun* kalashikov( Machine* pParent, const QString& name = "Kalashinkov" );
	static Gun* berezin( Machine* pParent, const QString& name = "Berezin" );
	
private:

	// config
	double _mass;		///< Bullet mass
	double _size;		///< Bullet size
	double _lifespan;	///< Bullet lifespan
	double _velocity;	///< Bullet;s initial velocity
	double _interval;	///< shooting interval [seconds]
	
	QPointF _muzzle;	///< Muzzle location
	QPointF _normal;	///< Muzzle direction
	
	// variables
	bool	_firing;	///< Is currently firing
	double	_timeFromLastFiring;
	double	_currentVelocity;	///< Current velocity
	double	_currentnInterval;	///< Current interval
	bool	_broken;			///< If is totally broken
};

}

#endif // FLYERGUN_H

// EOF


