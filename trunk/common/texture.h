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
	Texture( const QImage& baseImage );
	~Texture();
	
	/// Applies effect to image to get specified style
	static QImage applyStyle( const QImage& src, Style style );
	
	/// Provides texture image in specified version.
	QImage image( Style style );
	
	bool isNull() const { return _images.isEmpty(); }

private:

	QMap< Style, QImage >	_images;	///< Cached converted images
};

}

#endif // FLYERTEXTURE_H

// EOF


