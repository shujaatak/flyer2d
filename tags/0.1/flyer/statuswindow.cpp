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

#include "statuswindow.h"
#include "machine.h"
#include "system.h"

namespace Flyer
{

// ============================================================================
// Constructor
StatusWindow::StatusWindow() : QWidget( NULL )
{
	setupUi( this );
	
	table->setColumnCount( 2 );
	_pMachine = NULL;
	_timer.setInterval( 1000 );
	connect( & _timer, SIGNAL(timeout()), SLOT(refresh()) );
	_timer.start();
}

// ============================================================================
// Destructor
StatusWindow::~StatusWindow()
{
}

// ============================================================================
// Sets machein to be monitored
void StatusWindow::setMachine( Machine* pMachine )
{
	_pMachine = pMachine;
	refresh();
}

// ============================================================================
// Refreshes display
void StatusWindow::refresh()
{
	if ( _pMachine )
	{
		const QList<System*>& systems = _pMachine->systems();
		
		table->setRowCount( systems.size() );
		
		int r = 0;
		foreach( System* s, systems )
		{
			QString name = s->name();
			QString status = QString("%1%").arg( int( s->status() * 100 ) );
			
			table->setItem( r, 0, new QTableWidgetItem( name ) );
			table->setItem( r, 1, new QTableWidgetItem( status ) );
			r++;
		}
	}
}

}
