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

#include "common.h"

#include "grounddecoration.h"

namespace Flyer
{

// ============================================================================
// Constructor
GroundDecoration::GroundDecoration( World* pWorld ): WorldObject( pWorld )
{
	setRenderLayer( LayerForeground );
	_pTextures = NULL;
	setName("Ground decoration");
}

// ============================================================================
// Destrcutor
GroundDecoration::~GroundDecoration()
{
}

// ============================================================================
/// Returnsa item's boundoing rect.
QRectF GroundDecoration::boundingRect() const
{
	return _boundingRect;
}

// ============================================================================
/// Actually draws the surface.
void GroundDecoration::render( QPainter& painter, const QRectF& /*rect*/, const RenderingOptions& /*options*/ )
{
	// draw pixmaps
	painter.save();
		painter.setClipRect( _boundingRect );
		painter.setTransform( _transform, true );
		double x = 0;
		foreach( int index, _textureIndices )
		{
			const QImage& image = (*_pTextures)[ index ];
			painter.drawImage( x, -image.height(), image );
			//painter.setPen( Qt::red );
			//painter.drawRect( x, -1, 4, 1 );
			x += image.width();
		}
	painter.restore();
}

// ============================================================================
/// Initializes decoration
void GroundDecoration::init( const QList<int>& textureIndices, const QRectF& boundingRect
	, const QTransform& transform, QList<QImage>* pTextures  )
{
	_boundingRect = boundingRect;
	_textureIndices = textureIndices;
	_transform = transform;
	_pTextures = pTextures;
}

}
