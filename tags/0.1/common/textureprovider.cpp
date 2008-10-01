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
#include <QHash>


#include "textureprovider.h"

namespace Flyer
{

static QString _libraryPathCache; ///< Library path cache

/// Texture cache
static QHash< QString, Texture > _textures;

// ============================================================================
///Loads texture.
Texture TextureProvider::loadTexture( const QString& name )
{
	// texture in cache
	if ( _textures.contains( name ) )
	{
		return _textures[ name ];
	}
	
	// teture not in cache, load 
	QString path = libraryPath() + "/" + name; // TODO stupid and unreliable
	QImage baseImage( path );

	Texture t( baseImage );
	_textures.insert( name, t );
	
	return t;
}

// ============================================================================
///Returns path to the library.
QString TextureProvider::libraryPath()
{
	if ( _libraryPathCache.isNull() )
	{
		QDir lib( QCoreApplication::applicationDirPath() );
		lib.cd( "../textures" );
		_libraryPathCache = lib.absolutePath();
	}
	
	return _libraryPathCache;
}


}
