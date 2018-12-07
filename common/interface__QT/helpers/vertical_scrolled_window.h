/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://reduz.com.ar/cheesetronic
    copyright            : (C) 2003 by Juan Linietsky
    email                : coding@reduz.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VERTICAL_SCROLLED_WINDOW_H
#define VERTICAL_SCROLLED_WINDOW_H


#include <q3scrollview.h>
//Added by qt3to4:
#include <QEvent>
#include <QResizeEvent>

/**
 *
 * Juan Linietsky
 **/
class Vertical_Scrolled_Window : public QScrollView {

	QWidget *child;

	//child eventfilter
	bool eventFilter( QObject *o, QEvent *e );
	void resizeEvent ( QResizeEvent * p_resevebt);
public:

        void set_child(QWidget *p_widget);

	Vertical_Scrolled_Window(QWidget *p_parent);
	~Vertical_Scrolled_Window();
};

#endif
