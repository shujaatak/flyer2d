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

#ifndef FLYERTEXTURE_H
#define FLYERTEXTURE_H

#include <QMap>
#include <QImage>

#include "renderingoptions.h"

namespace Flyer
{

/**
Texture class. Holds image in different version.

@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Texture
{
public:
	/// Texture style
	enum Style {
		Normal,				///< Normal texture, unchanged
		Background,			///< Dimmed texture for objects moved to background
	};

	Texture();
	Texture( double width, double height, double resolution = 0.0 );
	Texture( const QImage& baseImage, double resolution = 0.0 );
	~Texture();
	
	/// Applies effect to image to get specified style
	static QImage applyStyle( const QImage& src, int style );
	
	
	/// Renders texture at specified position
	void render( QPainter& p, const QPointF& pos, const RenderingOptions o );
	
	/// Fills shape with texture
	//void fill( QPainter& p, const QPointF& pos, const QPainterPath& shape, const RenderingOptions o );
	
	double width() const;		///< Texture width [m]
	double height() const;		///< Texture height [m]
	
	double resolution() const { return _resolution; }
	void setResolution( double r ) { _resolution = r; }
	
	void setIsSprite( bool sprite );
	bool isSprite() const { return _isSprite; }
	
	/// Provides texture image in specified version.
	///@obsolete Use render() instead.
	QImage& image( int style );
	
	/// Returns base image of the texture
	QImage& baseImage() { return image( Normal ); }
	
	/// Checks if texture is null
	bool isNull() const { return _images.isEmpty(); }
	
private:

	QMap< int, QImage >	_images;		///< Cached converted images
	double				_resolution;	///< Texture resolution [meters per pixel]
	
	// sprite support
	
	/// Returns sprite-version of image
	QImage& sprite( int style );
	
	bool				_isSprite;		///< If texture is sprite. Sprite is painted using glDrawPixles. Deadly fast.
	QMap< int, QImage >	_sprites;		///< Cached converted images
};

}

#endif // FLYERTEXTURE_H

// EOF


