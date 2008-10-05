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

#include <QKeyEvent>
#include <QGLFormat>
#include <QTextEdit>
#include <QDialog>
#include <QStyleFactory>

#include "mainwindow.h"
#include "plane.h"
#include "statuswindow.h"
#include "world.h"

namespace Flyer
{

// ============================================================================
// Constructor
MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
	// turn on sample buffers
	QGLFormat format = QGLFormat::defaultFormat();
	format.setSampleBuffers( true );
	QGLFormat::setDefaultFormat( format );
	
 	setupUi( this );

	// status window
	_pStatusWindow = new StatusWindow();
	_pStatusWindow->show();
	_pStatusWindow->setMachine( worldWidget->plane() );
	
	// enemy status window
	StatusWindow* pEnemy = new StatusWindow();
	pEnemy->setMachine( worldWidget->world()->_pEnemyPlane );
	pEnemy->show();
	pEnemy->setWindowTitle( "Enemy plane" );
	
	// help
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
	
	QDialog* pHelpDialog = new QDialog
		( NULL, Qt::CustomizeWindowHint | Qt::WindowTitleHint 
			| Qt::WindowShadeButtonHint | Qt::WindowSystemMenuHint
			| Qt::WindowMinimizeButtonHint
		);
		
	pHelpDialog->setWindowOpacity( 0.8 );
	pHelpDialog->setWindowTitle( "Help" );
	pHelpDialog->setLayout( new QVBoxLayout );
	pHelpDialog->setSizeGripEnabled( true );
	pHelpDialog->setStyle( QStyleFactory::create( "plastique" ) );
	
	
	QTextEdit *pHelp = new QTextEdit( pHelpDialog );
	pHelp->setStyle( QStyleFactory::create( "plastique" ) );
	pHelpDialog->layout()->addWidget( pHelp );
	pHelp->setText( help );
	pHelp->setReadOnly( true );
	worldWidget->addWidget( pHelpDialog );
	
	
	
}

// ============================================================================
// Destructor
MainWindow::~MainWindow()
{
}

// ============================================================================
// Quits app on close
void MainWindow::closeEvent( QCloseEvent* )
{
	qApp->quit();
}

}
