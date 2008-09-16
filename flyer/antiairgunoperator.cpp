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

#include <QPointF>
#include "world.h"
#include "antiairgunoperator.h"
#include "b2dqt.h"
#include "gun.h"
#include "body.h"
#include "machine.h"
#include "plane.h"

namespace Flyer
{

static const double DEFAULT_ROTATION_SPEED = 1.0; // resonabledefault

// ============================================================================
// Constructor
AntiAirGunOperator::AntiAirGunOperator( Machine* pParent, const QString& name ) : System( pParent, name )
{
	_pGun = NULL;
	_rotationSpeed = DEFAULT_ROTATION_SPEED;
	_desiredGunAngle = 0.0;
	_currentAngle = 0.0;
	_damageReceived	= 0.0;
	_broken = false;
}

// ============================================================================
// Destructor
AntiAirGunOperator::~AntiAirGunOperator()
{
}

// ============================================================================
/// Returens estimated damafe status
double AntiAirGunOperator::status() const
{
	// TODO implement damage
	return 1.0 - qMax( 1.0, ( _damageReceived / damageCapacity() ) );
}

// ============================================================================
/// Damages operator
void AntiAirGunOperator::damage ( double force )
{
	if ( ! _broken )
	{
		if ( _damageReceived < damageCapacity() )
		{
			_damageReceived += force;
		}
		
		if ( _damageReceived >= damageCapacity() )
		{
			_damageReceived = damageCapacity();
			_broken = true;
			if ( _pGun ) _pGun->setFiring( false );
		}
	}
}

// ============================================================================
// Simulates
void AntiAirGunOperator::simulate ( double dt )
{
	if ( _pGun && ! _broken )
	{
		bool shoot = false;
		QPointF enemy = getEnemyPos();
		if ( ! enemy.isNull() )
		{
			// aim at enemy
			if ( _pGun && _pGun->body() && _pGun->body()->b2body() )
			{
				b2Vec2 gunPos = _pGun->body()->b2body()->GetWorldPoint( point2vec( _pGun->muzzle() ) );
				
				// simplest calculation
				// TODO add aiming before enemy
				
				_desiredGunAngle = atan2( enemy.x() - gunPos.x, enemy.y() - gunPos.y ); //angle from zenith
				
			}
			
			// decide if shoot
			if ( fabs( _desiredGunAngle - _currentAngle ) < 0.2 ) // TODO use constant or what
			{
				// check if in angle
				if ( fabs(_currentAngle) > _minAngle && fabs(_currentAngle ) < _maxAngle )
				{
					shoot = true;
				}
			}
		}
		
		// rotate gun
		double error = _desiredGunAngle - _currentAngle;
		double movement = _rotationSpeed * dt;
		if ( fabs(error) < movement )
		{
			_currentAngle = _desiredGunAngle;
		}
		else if ( error < 0 )
		{
			_currentAngle -= movement;
		}
		else
		{
			_currentAngle += movement;
		}
		// set gun angle
		_pGun->setNormal( QPointF( sin(_currentAngle), cos(_currentAngle) ) );
		_pGun->setFiring( shoot );
	}
}

// ============================================================================
/// Seeks for enemy,. Returns it's position, or null QpointF if not found
QPointF AntiAirGunOperator::getEnemyPos()
{
	const double SIGHT_RANGE = 1000; // 1km
	
	// player pos
	QPointF playerPos = parent()->world()->playerPlane()->position();
	if ( _pGun && _pGun->body() && _pGun->body()->b2body() )
	{
		// own pos
		b2Vec2 gunPos = _pGun->body()->b2body()->GetWorldPoint( point2vec( _pGun->muzzle() ) );
		
		// check range
		if ( ( gunPos - point2vec( playerPos ) ).Length() <= SIGHT_RANGE )
		{
			return playerPos;
		}
	}
	
	return QPointF();
	
	
	
	
}

}
