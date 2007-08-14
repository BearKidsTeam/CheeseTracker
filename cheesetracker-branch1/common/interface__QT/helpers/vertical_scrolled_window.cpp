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
//
//
// C++ Implementation: cpp
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "vertical_scrolled_window.h"
//Added by qt3to4:
#include <QEvent>
#include <QResizeEvent>

bool Vertical_Scrolled_Window::eventFilter( QObject *o, QEvent *e ) {

	if (child==NULL)
		return false;
	if (o != child)
		return false;

	if (QEvent::Resize != e->type())
		return false;

	//QResizeEvent * resize_event = (QResizeEvent*)e;

	viewport()->setMinimumWidth(child->minimumWidth());

	return false;
}

void Vertical_Scrolled_Window::resizeEvent ( QResizeEvent * p_resevebt) {
	if (child==NULL)
		return;

//	child->setWidth(viewport()->width());
 	child->resize(viewport()->width(),child->height());
}

Vertical_Scrolled_Window::Vertical_Scrolled_Window(QWidget *p_parent) : Q3ScrollView(p_parent) {

	child=NULL;
}

void Vertical_Scrolled_Window::set_child(QWidget *p_widget) {

	if (p_widget)
		return;

	p_widget->installEventFilter(this);
	child=p_widget;
}

Vertical_Scrolled_Window::~Vertical_Scrolled_Window()
{
}
