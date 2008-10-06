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

#ifndef FLYERGAMEUIMAIN_H
#define FLYERGAMEUIMAIN_H

#include "gameuidialog.h"
#include "ui_GameUIMain.h"

namespace Flyer
{

/**
Game's main menu
@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class GameUIMain : public GameUIDialog, private Ui::GameUIMain
{
Q_OBJECT

public:
	GameUIMain( WorldScene* pScene );
	virtual ~GameUIMain();

private slots:

	void on_buttonQuit_clicked();
	void on_buttonResume_clicked();
	void on_buttonNewGame_clicked();
	void on_buttonHelp_clicked();
};

}

#endif // FLYERGAMEUIMAIN_H

// EOF


