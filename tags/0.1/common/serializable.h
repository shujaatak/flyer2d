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

#ifndef FLYERSERIALIZABLE_H
#define FLYERSERIALIZABLE_H

#include <QByteArray>
#include <QDataStream>

namespace Flyer
{

/**
Base class 
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Serializable
{

public:

	// memory buffer i/o

	/// Serializes object to memty buffer.
	QByteArray toBuffer() const;
	/// Loads object from memory buffer.
	void fromBuffer( QByteArray& buffer );
	
	// device io (with added header)
	
	void fromDevice( QIODevice* pDevice );
	void toDevice( QIODevice* pDevice ) const;
	
	// file i/o (based on device i/o, with header)
	
	void fromFile( const QString& path );
	void toFile( const QString& path ) const;

    // connected stream operations
	/// \relates GSerializableItem
	/// Writes item \a item to the stream \a out and returns
	/// a reference to the stream.
	friend QDataStream & operator << ( QDataStream & out,
										const Serializable & item );
	/// \relates GSerializableItem
	/// Reads item into \a item from the stream \a in and returns
	/// a reference to the stream.
	friend QDataStream & operator >> ( QDataStream & in,
                                       Serializable & item );
protected:

	/// Puts object into stream
	virtual void toStream( QDataStream& stream ) const = 0;
	/// Loads object from stream
	virtual void fromStream( QDataStream& stream ) = 0;
	
	/// Provides ID which identifies object type
	virtual QString classId() const = 0;
	

};

}

#endif // FLYERSERIALIZABLE_H

// EOF


