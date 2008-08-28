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

#ifndef FLYERAUTOPILOT_H
#define FLYERAUTOPILOT_H

#include <QLinkedList>
#include <QSet>

#include "Box2D.h"

#include "system.h"

namespace Flyer
{

class ControlSurface;
class Engine;
class Wing;

/**
	Simple autopilot. Steers elevator using simple PID controller
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Autopilot : public System
{
public:

	/// Aotupilot operation modes
	enum Mode
	{
		KeepLevel,			///< Keeps leveled flight, simplest mode
		FollowTrack			///< Folows defined track
	};

	/// Special orders attached to segment
	enum SegmentOrders 
	{
		Land,				///< Land during this segment
		Flip,				///< Flip during this segment
	};

	/// Defines segment of track to follow
	struct TrackSegment
	{
		b2Vec2 start;			///< Segment start
		b2Vec2 end;				///< Segment end
		double	airspeed;		///< Desired airspeed on this segment. Negative means 'don't care'
		QSet< SegmentOrders >	orders;
	};
	
	/// Autopilot settings
	struct Settings
	{
		double	flapsOpenSpeed;	///< Desired airspeed below which flaps should be open
		double	flapsCloseSpeed;	///< Actual airspeed abovce whicvh it is safe to close fpals
		double	minSpeed;		///< Min airspeed with flaps open
		double	maxClimbRate;	///< Max climb rate (vertical speed/ horizontal speed)
		double	maxDescentRate;	///< Max dive speed (vertical speed/ horizontal speed)
		
		
		double	safeAngle;		///< Safe angle for steady flight
		
		double	P, I, D;		///< verttica lspeed PID coefficients
		double	VVP;			///< vetical pos controller: vertical speed per meter of vertical deviation
		double	VVI;			///< I of vertical pos
	};

	Autopilot ( Machine* pParent, const QString& name = "" );
	virtual ~Autopilot();

	virtual void damage ( double force );
	virtual void simulate ( double dt );
	virtual void render( QPainter& painter, const QRectF& rect );
	
	void setElevator( ControlSurface* pElevator ) { _pElevator = pElevator; }
	void setEngine( Engine* pEngine ) { _pEngine = pEngine; }
	void setWing( Wing* pWing ){ _pWing = pWing; }
	
	void setOn( bool on ) { _on = on; }
	bool on() const { return _on; }
	
	void setSettings( const Settings& s ) { _settings = s; }
	
	void setMode( Mode m ) { _mode = m; }
	Mode mode() const { return _mode; }
	
	// Track
	QLinkedList<TrackSegment>& track(){ return _track; }
	const QLinkedList<TrackSegment>& track() const { return _track; }
	
private:

	// operations
	b2Vec2 position();		///< Returns current position
	b2Vec2 velocity();		///< Returns current linear velocity
	
	// control modes impelementations

	void controlKeepLevel( double dt );
	void controlFollowPath( double dt );
	
	// variables
	
	double _previousError;
	double _errorIntegral;
	double _posErrorIntegral;	///< integral of position error
	bool	_on;			///< Flag - is turned on?
	
	// config
	
	ControlSurface*	_pElevator;		///< Controlled elevator
	Engine*			_pEngine;		///< Controlled engine
	Wing*			_pWing;			///< Controlled wing
	Settings	_settings;
	Mode		_mode;				///< mode
	QLinkedList<TrackSegment>	_track;
};

}

#endif // FLYERAUTOPILOT_H

// EOF


