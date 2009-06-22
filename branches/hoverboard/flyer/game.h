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

#ifndef FLYERGAME_H
#define FLYERGAME_H

#include <QObject>

#include "message.h"

namespace Flyer
{

class World;

/**
This is  general game object, and I really don't know what it will be used for.
Currently it is a World factory, creating predefined game worlds.
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class Game : public QObject
{
Q_OBJECT

public:
	virtual ~Game();

	/// Returns game world
	World* world() const { return _pWorld; }
	
	// factory
	
	static Game* createGame();
	
	// game actions
	
	virtual void respawn(){};
	virtual void restart();
	
	const QList<Message>& messages() const { return _messages; }

private:
	
	
	Game( QObject *parent = 0 );
	void setWorld( World* pWorld );
	void addMessage( const QString& text, double minTime = 0.0 );
	
	// helpers
	
	static void createTown(  World* pWorld, double start, double end, bool smallHouses ); // NOTE was 'small' here, but smetimes on Windows 'small' is defined as 'char' :(
	
	
	World* _pWorld;
	QList<Message> _messages;	///< Game messages
};

}

#endif // FLYERGAME_H

// EOF


