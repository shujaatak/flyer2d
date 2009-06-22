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

#ifndef FLYERMESSAGE_H
#define FLYERMESSAGE_H

#include <QString>

namespace Flyer
{

/**
Text message. Stores radio and syster mmessages.
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Message
{

public:

	enum Type			/// Message type
	{
		System,			///< Machine systems message
		Game,			///< Game message
		Radio,			///< Radio message
	};

	Message();
	Message( const QString& text, double time, Type type );
	
	/// Initializes instance
	void init( const QString& text, double time, Type type );

	// properties
	
	QString text() const { return _text; }
	double time() const { return _time; }
	Type type() const { return _type; }
	

private:

	QString _text;	///< Message text
	double	_time;	///< Time when message is created (game clock)
	Type	_type;	///< Message type
};

}

#endif // FLYERMESSAGE_H

// EOF


