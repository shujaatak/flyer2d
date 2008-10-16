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

#include <math.h>

#include <QPainter>
#include <QGLWidget>

#include "texture.h"

namespace Flyer
{

static const double DEFAULT_RESOLUTION = 0.05; // 5 cm per pixel

// ============================================================================
// Default (and hopefully not used) constructor.
Texture::Texture()
{
	_resolution = DEFAULT_RESOLUTION;
	_isSprite = false;
}

// ============================================================================
/// Constructor - creates texture using supplied basic image
Texture::Texture( const QImage& baseImage, double resolution )
{
	_images.insert( Normal, baseImage );
	
	if ( resolution > 0 ) _resolution = resolution;
	else _resolution = DEFAULT_RESOLUTION; // TODO read resolution form image
	_isSprite = false;
}

// ============================================================================
///. Creates empty texture
Texture::Texture( double width, double height, double resolution )
{
	if ( resolution > 0 ) _resolution = resolution;
	else _resolution = DEFAULT_RESOLUTION;
	
	int pixelsWidth = int( ceil( width / _resolution ) );
	int pixelsHeight = int( ceil( height / _resolution ) );
	
	QImage base( pixelsWidth, pixelsHeight, QImage::Format_ARGB32_Premultiplied );
	base.fill( 0x00000000 ); // fill with transparency
	_images.insert( Normal, base  );
	_isSprite = false;
}

// ============================================================================
// Destructor
Texture::~Texture()
{
}

// ============================================================================
/// Returns image in specified version
QImage& Texture::image( int style )
{
	if ( _images.contains( style ) )
	{
		return _images[ style ];
	}
	
	_images.insert( style, applyStyle( _images[ int(Normal) ], style ) );
	return _images[ style ];
}

// ============================================================================
/// Applies effects to pixmap
QImage Texture::applyStyle( const QImage& src, int style )
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

// ============================================================================
/// Returns texture width [in meters]
double Texture::width() const
{
	return _images[Normal].width() * _resolution;
}

// ============================================================================
/// Returns texture height [in meters]
double Texture::height() const
{
	return _images[Normal].height() * _resolution;
}

// ============================================================================
/// Renders texture on painter.
///\param pos - left-top corner position in painter corrdinates
///\param p - painter
///\param o - options
void Texture::render( QPainter& painter, const QPointF& pos, const RenderingOptions o )
{
	// fast rendering using glDrawPixels
	// TODO system meory-> video memroy copy is death slow!
	// TODO this trick will be useful when i find out how to store sprite in vmem
	/*
	if ( _isSprite )
	{
		QPointF center( o.viewportSize.width() / 2, o.viewportSize.height() / 2 );
		QPointF topLeftPixel		= painter.transform().map( pos ) - center;
		QPointF bottomRightPixel	= painter.transform().map( pos + QPointF( width(), height() ) ) - center;
		
		int w = bottomRightPixel.x() - topLeftPixel.x();
		int h = topLeftPixel.y() - bottomRightPixel.y();
		
		QImage& s = sprite( o.textureStyle );
		
		// TODO glPixelZoom here
		//glRasterPos2d( topLeftPixel.x(), topLeftPixel.y() );
		glDrawPixels( s.width(), s.height(), GL_RGBA, GL_UNSIGNED_BYTE, s.bits() );
		glFlush();
	}
	// normal rendering
	else
	*/
	{
		QTransform old = painter.transform();
		QTransform t = old;
		t.scale( _resolution, - _resolution );
		painter.setTransform( t, false );
		
		QPointF position( pos.x(), - pos.y() );
		
		// debug
		//QPointF p = position/_resolution;
		//QPointF pixelPos = painter.transform().map( p );
		
		painter.drawImage( position/_resolution, image( o.textureStyle ) );
		
		// restore previous trransform
		painter.setTransform( old );
	}
}

// ============================================================================
/// Fills supplied painter path with texture.
void Texture::fill( QPainter& painter, const QPointF& pos, const QPolygonF& shape, const RenderingOptions o )
{
	QTransform old = painter.transform();
	
	QTransform t;
	t.translate(  pos.x(), pos.y() ); // TODO test
	t.scale( _resolution, -_resolution );
	
	QBrush brush;
	brush.setTextureImage( image( o.textureStyle ) );
	brush.setTransform( t );
	
	painter.setBrush( brush );
	
	painter.setPen( Qt::NoPen );
	//painter.setPen( Qt::blue ); // TODO debug
	
	painter.drawPolygon( shape );
}

// ============================================================================
/// Makes texture a sprite. Sprites are draw using glDrawPixels, which is deadly fast.
void Texture::setIsSprite( bool sprite )
{
	if ( _isSprite != sprite )
	{
		_isSprite = sprite;
	}
}

// ============================================================================
/// Returns image converted to OpenGL format
QImage& Texture::sprite( int style )
{
	if (  ! _sprites.contains( style ) )
	{
	
		QImage& src = image( style );
		QImage converted = QGLWidget::convertToGLFormat( src );
		
		_sprites.insert( style, converted );
	}
	
	return _sprites[ style ];
}

}
