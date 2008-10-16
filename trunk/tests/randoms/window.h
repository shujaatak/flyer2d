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

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

#include "ui_window.h"

/**
 @author Maciek Gajewski <maciej.gajewski0@gmail.com>
*/

class Window : public QWidget, private Ui::Window
{
	Q_OBJECT

public:
	Window( QWidget *parent = 0 );
	~Window();
	
private slots:

	void on_button01_clicked();

private:

	virtual void paintEvent( QPaintEvent* e );

	QPolygonF 		_expectedResults;
	QVector<double>	_measuredResults;
};

#endif // WINDOW_H

// EOF


