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

#include <QPainter>

#include "texture.h"

namespace Flyer
{

// ============================================================================
// Default (and hopefully not used) constructor.
Texture::Texture()
{
}

// ============================================================================
// Constructor
Texture::Texture( const QImage& baseImage )
{
	_images.insert( Normal, baseImage );
}

// ============================================================================
// Destructor
Texture::~Texture()
{
}

// ============================================================================
/// Returns image in specified version
QImage Texture::image( Style style )
{
	if ( _images.contains( style ) )
	{
		return _images[ style ];
	}
	
	_images.insert( style, applyStyle( _images[ Normal ], style ) );
	return _images[ style ];
}

// ============================================================================
/// Applies effects to pixmap
QImage Texture::applyStyle( const QImage& src, Style style )
{
	switch( style )
	{
		case Background:
		{
			QImage dst( src );
			QPainter painter( & dst );
			painter.setCompositionMode( QPainter::CompositionMode_SourceAtop );
			painter.fillRect( src.rect(), QColor( 128, 128, 255, 64 ) );
			
			return dst;
		}
		
		default:
		
			qWarning("Unknwonwn style: %d", int(style) );
			return src;
	}
}


}
