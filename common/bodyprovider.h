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

#ifndef FLYERBODYPROVIDER_H
#define FLYERBODYPROVIDER_H

#include <QString>

namespace Flyer
{

class Body;

/**
Body library manager. Provides boduies form body library.
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class BodyProvider
{
public:
	
	/// Loads body from library.
	static Body* loadBody( const QString& name );
	
	/// Returns path to body library
	static QString libraryPath();
};

}

#endif // FLYERBODYPROVIDER_H

// EOF


