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

#include "antiairbattery.h"
#include "body.h"
#include "gun.h"
#include "world.h"
#include "ground.h"
#include "damagemanager.h"
#include "antiairgunoperator.h"

namespace Flyer
{

static const double WIDTH = 5;
static const double HEIGHT = 5;

// ============================================================================
// Constructor
AntiAirBattery::AntiAirBattery ( World* pWorld, double location, double angle  ) : Machine ( pWorld )
{
	double aboveGround = 1;	/// position above ground
	
	double locationY = world()->ground()->height( location );
	
	// create damage manager
	_dmMain = new DamageManager( 30E3 );
	addDamageManager( _dmMain );
	
	// create body
	b2BodyDef bodyDef;
	bodyDef.massData.mass = 0; // static body
	bodyDef.angle = 0;
	bodyDef.position.Set( location, locationY + aboveGround );
	
	b2PolygonDef shapeDef;
	shapeDef.SetAsBox( WIDTH/2, HEIGHT/ 2 );
	shapeDef.userData = _dmMain;
	
	_bodyMain = new Body();
	_bodyMain->create( bodyDef, pWorld->b2world() );
	_bodyMain->addShape( & shapeDef );
	addBody( _bodyMain, 0 );
	
	// create gun
	_sysGun = new Gun( this, "gun" );
	_sysGun->setBody( _bodyMain );
	_sysGun->setMuzzle( QPointF( 0.0, HEIGHT/2 + 0.1 ) );
	_sysGun->setNormal( QPointF( cos( angle), sin(angle) ) );
	_sysGun->setBulletMass( 7.91E-3 ); 
	_sysGun->setBulletVelocity( 735 );
	_sysGun->setBulletSize( 7.85E-3 ); // NOTE all data taken from kalashnikov ;)
	_sysGun->setFiringInterval( 0.4 ); // 2.5 shots/sec
	_sysGun->setBulletLifespan( 4.0 );
	_sysGun->setDamageCapacity( 100E3 );
	addSystem( _sysGun, SystemSimulated );
	
	// create operator
	_sysOperator = new AntiAirGunOperator( this, "operator" );
	_sysOperator->setGun( _sysGun );
	_sysOperator->setMinAngle( 0.5 );
	_sysOperator->setMaxAngle( M_PI/2 );
	_sysOperator->setDamageCapacity( 100E3 );
	addSystem( _sysOperator, SystemSimulated );
	
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
void AntiAirBattery::render ( QPainter& painter, const QRectF& )
{
	QTransform t = _bodyMain->transform();
	QRectF r( -WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT );
	
	painter.save();
		painter.setPen( Qt::black );
		painter.setBrush( Qt::darkRed );
		
		painter.setTransform( t, true );
		painter.drawRect( r );
		
		// draw gun
		QPointF start( 0.0, HEIGHT/2 );
		QPointF end = start + _sysGun->normal() *HEIGHT ;
		painter.drawLine( start, end );
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

// ============================================================================
// Boundsing rect
QRectF AntiAirBattery::boundingRect() const
{
	b2Vec2 pos = _bodyMain->b2body()->GetPosition();
	
	QRect r( pos.x -WIDTH/2, pos.y - HEIGHT/2, WIDTH ,HEIGHT );
	return r;
}


}
