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

#include "mainwindow.h"
#include "plane.h"

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
	connect( worldWidget, SIGNAL(elevatorChanged(double)), SLOT(elevatorChanged(double)) );
	connect( worldWidget, SIGNAL(throttleChanged(double)), SLOT(throttleChanged(double)) );
	connect( worldWidget, SIGNAL(flapsChanged(double)), SLOT(flapsChanged(double)) );
}

// ============================================================================
// Destructor
MainWindow::~MainWindow()
{
}

// ============================================================================
// Elevator slider handle
void MainWindow::on_sliderElevator_valueChanged( int value )
{
	worldWidget->plane()->setElevator( value / 100.0 );
}

// ============================================================================
// Throttle slider handle
void MainWindow::on_sliderThrottle_valueChanged( int value )
{
	worldWidget->plane()->setThrottle( value / 100.0 );
}

// ============================================================================
// button start stop
void MainWindow::on_buttonStartStop_toggled( bool state )
{
	if ( state ) worldWidget->start();
	else worldWidget->stop();
}

// ============================================================================
// button step
void MainWindow::on_buttonStep_clicked()
{
	worldWidget->step();
}

// ============================================================================
// on brake pressed
void MainWindow::on_buttonBrake_pressed()
{
	worldWidget->plane()->applyWheelBrake( true );
}

// ============================================================================
// on brake released
void MainWindow::on_buttonBrake_released()
{
	worldWidget->plane()->applyWheelBrake( false );
}

// ============================================================================
// Flips plane
void MainWindow::on_buttonFlip_clicked()
{
	worldWidget->plane()->flipPlane();
}

// ============================================================================
// Flaps handler.
void MainWindow::on_sliderFlaps_valueChanged( int value )
{
	worldWidget->plane()->setFlaps( value / 3.0 );
}

// ============================================================================
// Handles plane elevator change
void MainWindow::elevatorChanged( double e )
{
	sliderElevator->blockSignals( true );
	sliderElevator->setValue( 100 * e );
	sliderElevator->blockSignals( false );
}

// ============================================================================
// Handle throttle change
void MainWindow::throttleChanged( double t )
{
	sliderThrottle->blockSignals( true );
	sliderThrottle->setValue( 100*t );
	sliderThrottle->blockSignals( false );
}

// ============================================================================
// Handles flaps change
void MainWindow::flapsChanged( double f )
{
	sliderFlaps->blockSignals( true );
	sliderFlaps->setValue( f * 4 );
	sliderFlaps->blockSignals( false );
}

}
