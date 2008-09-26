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

#include "body.h"
#include "gun.h"
#include "world.h"
#include "ground.h"
#include "damagemanager.h"
#include "antiairgunoperator.h"
#include "bodyprovider.h"
#include "textureprovider.h"

#include "antiairbattery.h"

namespace Flyer
{

static const double WIDTH = 5;
static const double HEIGHT = 5;

// ============================================================================
// Constructor
AntiAirBattery::AntiAirBattery ( World* pWorld, double location, double angle  ) : Machine ( pWorld )
{
	setLayers( World::ObjectRenderedBuildings );

	double locationY = world()->ground()->height( location );
	b2Vec2 basePos( location, locationY );
	b2Vec2 bodyPos( location, locationY + 1.5 );
	
	// create damage manager
	_dmMain = new DamageManager( 30E3 );
	addDamageManager( _dmMain );
	
	_bodyMain = BodyProvider::loadBody( "installations/flak1-body.body" );
	_bodyMain->setPosition( bodyPos );
	_bodyMain->create( pWorld->b2world() );
	addBody( _bodyMain, BodyRendered1 );
	
	_bodyBase = BodyProvider::loadBody( "installations/flak1-base.body" );
	_bodyBase->setPosition( basePos );
	_bodyBase->create( pWorld->b2world() );
	addBody( _bodyBase, BodyRendered1 );
	
	_bodyMain->shapeByName( "main" )->setDamageManager( _dmMain );
	
	// create gun
	_sysGun = Gun::berezin( this, "Gun" );
	_sysGun->setBody( _bodyMain );
	_sysGun->setMuzzle( QPointF( 0.0, 0.0 ) );
	_sysGun->setMuzzleShift( 4.0 );
	_sysGun->setNormal( QPointF( cos( angle), sin(angle) ) );
	addSystem( _sysGun, SystemSimulated );
	
	// create operator
	_sysOperator = new AntiAirGunOperator( this, "operator" );
	_sysOperator->setGun( _sysGun );
	_sysOperator->setMinAngle( 0.5 );
	_sysOperator->setMaxAngle( M_PI/2 );
	_sysOperator->setDamageCapacity( 100E3 );
	addSystem( _sysOperator, SystemSimulated );
	_lastDisplayedAngle = 0.5; // min angle from zenith;
	
	// add systems to damage manager
	_dmMain->addSystem( _sysGun, 1 );
	_dmMain->addSystem( _sysOperator, 1 );
	_dmMain->addSystem( NULL, 2 );
	
}

// ============================================================================
// Destructor
AntiAirBattery::~AntiAirBattery()
{
}

// ============================================================================
// Renders AA battery
void AntiAirBattery::render ( QPainter& painter, const QRectF& rect, const RenderingOptions& options )
{
	Machine::render( painter, rect, options );
	
	QTransform t = _bodyMain->transform();
	
	// get angle
	double currentAngle = _sysOperator->currentAngle();
	double lastSign = _lastDisplayedAngle > 0 ? 1 : -1;
	
	// limit angle to min/max
	if ( fabs( currentAngle ) < _sysOperator->minAngle() )
	{
		currentAngle = _sysOperator->minAngle() * lastSign; // don't change sign while outside range
	}
	if (  fabs( currentAngle ) > _sysOperator->maxAngle() )
	{
		currentAngle = _sysOperator->maxAngle() * lastSign;
	}
	
	// check if sign changed
	if ( ( lastSign * currentAngle)  < 0 )
	{
		double x = _bodyMain->position().x;
		_bodyMain->flip( QPointF( x, 1 ), QPointF( x, -1 ) ); // flip around vertical axis
	}
	_lastDisplayedAngle = currentAngle;
	double sign = currentAngle > 0 ? 1 : -1;
	
	// calculate screen angle
	double screenAngle =  M_PI/2 - currentAngle;
	
	painter.save();
		QImage barrel = TextureProvider::loadTexture("installations/flak1-barrel.png").image(Texture::Normal);
		QPointF textureAxis = QPointF( 13, 8 ); // axis in texture coords
		
		double scale = 0.05; // TODO this has to go with texture somehow
		t.rotateRadians( screenAngle );
		t.scale( scale * sign, -scale * sign );
		t.translate( -textureAxis.x(), -textureAxis.y() );
		painter.setTransform( t, true );
		painter.drawImage( 0, 0, barrel );
	painter.restore();
}

// ============================================================================
// Renders on map
void  AntiAirBattery::renderOnMap( QPainter& painter, const QRectF&  )
{
	double size = 200;
	b2Vec2 pos = _bodyMain->b2body()->GetPosition();
	
	QRect r( pos.x -size/2, pos.y - size/2, size ,size );
	painter.setPen( Qt::darkRed );
	painter.drawRect( r );
}


}
