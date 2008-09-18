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

#include <QFile>

#include "gexception.h"

#include "serializable.h"

namespace Flyer
{

// ============================================================================
/// Serializes object to memroy buffer, creating memory stream and calling toStream()
/// on itself.
QByteArray Serializable::toBuffer() const
{
	QByteArray buffer;
	QDataStream stream( &buffer, QIODevice::WriteOnly );
	toStream( stream );
	
	return buffer;
}

// ============================================================================
/// Loads object from memoery buffer, creating stream on the buffer and reading
/// data using fromStream() method.
void Serializable::fromBuffer( QByteArray& buffer )
{
	QDataStream stream( &buffer, QIODevice::ReadOnly );
	fromStream( stream );
}

// ============================================================================
/// Reads from device, with header.
void Serializable::fromDevice( QIODevice* pDevice )
{
	Q_ASSERT( pDevice );
	QDataStream stream( pDevice );
	
	// read/check header
	QString id;
	stream >> id;
	
	if ( id != classId() )
	{
		throw GDatasetError(
			QString("Cant read form device - ivalid type id. Expected: %1, got: %2")
				.arg( classId() ).arg( id )
				);
	}
	
	QByteArray buffer;
	stream >> buffer;
	fromBuffer( buffer );
}

// ============================================================================
/// Writes to device, together with header.
void Serializable::toDevice( QIODevice* pDevice ) const
{
	Q_ASSERT( pDevice );
	QDataStream stream( pDevice );

	stream << classId();
	stream << toBuffer();
}

// ============================================================================
/// Reads from file
void Serializable::fromFile( const QString& path )
{
	QFile file( path );
	if ( ! file.open( QIODevice::ReadOnly ) )
	{
		throw GSysError( QString("Can't open file for reading %1 for reading: %2")
			.arg( path ).arg( file.errorString() ) );
	}
	
	fromDevice( & file );
}

// ============================================================================
/// Saves to file.
void Serializable::toFile( const QString& path ) const
{
	QFile file( path );
	if ( ! file.open( QIODevice::WriteOnly | QIODevice::Truncate) )
	{
		throw GSysError( QString("Can't open file %1 for writing: %2")
			.arg( path ).arg( file.errorString() ) );
	}

	toDevice( & file );
}

// ============================================================================
/// Stream write operator
QDataStream & operator << ( QDataStream & out, const Serializable & item )
{
	// do it via buffer, so always proepr number of bytes is ger from srtream
	out << item.toBuffer();
	return out;
}

// ============================================================================
/// Stream read operator
QDataStream & operator >> ( QDataStream & in, Serializable & item )
{
	QByteArray buffer;
	in >> buffer;
	item.fromBuffer( buffer );
	return in;
}

}
