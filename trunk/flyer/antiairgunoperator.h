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

#ifndef FLYERANTIAIRGUNOPERATOR_H
#define FLYERANTIAIRGUNOPERATOR_H

#include "system.h"

namespace Flyer
{
class Gun;

/**
	This is a AA gun operator. It operates AA gun to target neares enemy, and also
	simluates gun rotation
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class AntiAirGunOperator : public System
{
public:
	AntiAirGunOperator( Machine* pParent, const QString& name = "" );
	virtual ~AntiAirGunOperator();

	virtual double status() const;
	virtual void damage ( double force );
	virtual void simulate ( double dt );
	
	// config
	void setGun( Gun* pGun ){ _pGun = pGun; }
	void setMinAngle( double a ) { _minAngle = a; }
	void setMaxAngle( double a ) { _maxAngle = a; }
	
	
private:

	QPointF getEnemyPos(); ///< The Tricky Part(TM) - sekes for enemy

	// variables
	double _currentAngle;		///< Current gun angle
	double _desiredGunAngle;	///< Desired gun angle
	
	double	_damageReceived;	///< damage received
	bool	_broken;			///< broken (or "killed" )

	// config
	double _minAngle;		///< Min angle from zenith
	double _maxAngle;		///< Max angle from zenith
	double _rotationSpeed;	///< Rotation [rad/s]
	
	Gun*	_pGun;			///< gun to operate

};

}

#endif // FLYERANTIAIRGUNOPERATOR_H

// EOF


