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

#ifndef FLYERSTATUSWINDOW_H
#define FLYERSTATUSWINDOW_H

#include <QTimer>
#include "ui_statuswindow.h"
#include "gameuidialog.h"

namespace Flyer
{
class Machine;

/**
	Windows showing status of plane systems
	@author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/
class StatusWindow : public GameUIDialog, private Ui::StatusWindow
{
	Q_OBJECT
public:
	StatusWindow( WorldScene* pScene );
	~StatusWindow();

	void setMachine( Machine* pMachine );

private slots:

	void refresh();
	
private:

	QTimer	_timer;
	Machine *_pMachine;
};

}

#endif // FLYERSTATUSWINDOW_H

// EOF


