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

#include <QApplication>
#include <QTextEdit>
#include <QTimer>

#include "worldscene.h"
#include "game.h"

#include "gameuimain.h"

namespace Flyer
{

// ============================================================================
// Constructor
GameUIMain::GameUIMain( WorldScene* pScene ) : GameUIDialog( pScene )
{
	setupUi( this );
}

// ============================================================================
// Destructor
GameUIMain::~GameUIMain()
{
}

// ============================================================================
// Quit button handler
void GameUIMain::on_buttonQuit_clicked()
{
	// TODO rude!
	QApplication::quit();
}

// ============================================================================
// Resume button handler
void GameUIMain::on_buttonResume_clicked()
{
	scene()->start();
}

// ============================================================================
// 'New Game' button handler
void GameUIMain::on_buttonNewGame_clicked()
{
	scene()->restart();
	scene()->start();
}

// ============================================================================
// Help button handler
void GameUIMain::on_buttonHelp_clicked()
{
	QString help = 
	"Controls\n\n"
	"Mouse Up/Down\t- elevator\n"
	"Mouse Wheel\t- throttle\n"
	"Mouse Left Button\t- fire gun\n"
	"Mouse Right Button\t- release bomb\n"
	"Page Up/Page Down\t- zoom in/out\n"
	"F/V\t- flaps\n"
	"Space\t- flip/turn\n"
	"B\t- wheel brake\n"
	"A\t- toggle autopilot\n"
	"P\t- pause";
	
	GameUIDialog* pHelpDialog = new GameUIDialog( scene() );
	pHelpDialog->setWindowFlags(  Qt::WindowSystemMenuHint | pHelpDialog->windowFlags() );
	pHelpDialog->setWindowTitle( "Help" );
	pHelpDialog->setLayout( new QVBoxLayout );
	pHelpDialog->setSizeGripEnabled( true );
	
	connect( this, SIGNAL(destroyed()), pHelpDialog, SLOT(deleteLater()) );
	
	QTextEdit *pHelp = new QTextEdit( pHelpDialog );
	pHelpDialog->layout()->addWidget( pHelp );
	pHelp->setText( help );
	pHelp->setReadOnly( true );
	
	scene()->addWindow( pHelpDialog, 1.0 );
	pHelpDialog->setFocus();
	
}

}
