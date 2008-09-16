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

#include <QDir>
#include <QCoreApplication>

#include "textureprovider.h"

namespace Flyer
{

static QString libraryPathCache; ///< Library path cache

// ============================================================================
///Loads texture.
QPixmap TextureProvider::loadTexture( const QString& texture )
{
	QString path = libraryPath() + "/" + texture; // TODO stupid and unreliable
	return QPixmap( path );
}

// ============================================================================
///Returns path to the library.
QString TextureProvider::libraryPath()
{
	if ( libraryPathCache.isNull() )
	{
		QDir lib( QCoreApplication::applicationDirPath() );
		lib.cd( "../textures" );
		libraryPathCache = lib.absolutePath();
	}
	
	return libraryPathCache;
}


}
