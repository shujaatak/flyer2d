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

#ifndef FLYERPHYSICALOBJECT_H
#define FLYERPHYSICALOBJECT_H

class QPainter;
#include <QMap>
#include <QList>
#include <QPointF>

#include "Box2D.h"

#include "worldobject.h"

namespace Flyer
{
class Body;
class Joint;

/**
Common base class for world ojectes having physical representation.
The physical object consists of main body and other bodies attached via joints.

Box2D broadphase is used to search for these objects.
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class PhysicalObject : public WorldObject
{

public:
	PhysicalObject( World* pWorld );
	virtual ~PhysicalObject();
	
	virtual void simulate ( double dt );
	virtual QRectF boundingRect() const;
	virtual void render ( QPainter& painter, const QRectF& rect, const RenderingOptions& options );
	
	// geometrical operations
	
	/// Flip object around axis of symmetry
	virtual void flip( const QPointF& p1, const QPointF& p2 );
	
	/// Returns current orientation (flip) sign
	double orientation() const { return _orientation; }
	
	/// Returns estimated position, using main's body position.
	QPointF position() const;
	
	/// Returns machine angle
	double angle() const;
	
	/// Returns linear velocity
	b2Vec2 linearVelocity() const;
	
	/// Sets layers on whic hthis machine lives
	void setLayers( int layers );
	int layers() const { return _layers; }
	
	// damage modifiacations
	
	void breakJoint( Joint* pJoint );
	
	// bodies classes and manipulation
	enum BodyType {					/// Body type
		BodyRendered1	= 1,		///< Render layer 1
		BodyRendered2	= 2,		///< Render layer 1
		BodyRendered3	= 4			///< Render layer 1
		};
		
	void addBody( Body* pBody, int types );
	void removeBody( Body* pBody );
	
	void setMainBody( Body* pBody ) { _pMainBody = pBody; }
	Body* mainBody() const { return _pMainBody; }
	
	/// Message from body - body wiches to receive simulation events
	void bodyWakesUp( Body* pBody );
	/// Message from body - body wishes to not receive any messages any more
	void bodySleeps( Body* pBody );
	
	// joints
	void addJoint( Joint* pJoint );
	void removeJoint( Joint* pJoint );

	
private:

	// operations
	void doBreakJoint( Joint* pJoint );	///< Actually breaks joint
	void detachBody( Body* pBody );		///< Detaches body from machine
	
	// bodies
	QMap<int, QList<Body*> > _bodies;
	QList<Body*>	_allBodies;
	QList<Body*>	_simulatedBodies;	///< Bodies reciving 'simulate' events
	int				_layers;
	
	/// Object position is determined using it's pos, and destructicion of
	/// the body destroys the object.
	Body* _pMainBody;
	
	// joints
	QList<Joint*>	_allJoints;
	QList<Joint*>	_jointsToBreak; ///< Temporary list of joints that are to be break during next sim step
	
	// variables
	
	double _orientation;	//!< Orientation - 1 - normal, -1 - mirrored. 
	
	void updateSize();		///< Calculates _size;
	double	_size;			///< Approx. size used to calculate rough bounding rect.
	

};

}

#endif // FLYERPHYSICALOBJECT_H

// EOF


