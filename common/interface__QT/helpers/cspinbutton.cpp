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
#include "cspinbutton.h"
#include <qlineedit.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>


bool CSpinButton::eventFilter( QObject *o, QEvent *e ) {
	if (o == spin_widget) {

		if (QEvent::MouseMove == e->type()) {
			QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);

					if (mid_button_pressed) {

						setValue(value()+page_value*(grab_y-mouseEvent->y()));
						grab_y=mouseEvent->y();
					}
		}

		if (QEvent::MouseButtonRelease == e->type()) {
			QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);

			if (Qt::MidButton == mouseEvent->button())	{

					if (mid_button_pressed) {

						mid_button_pressed=false;
						spin_widget->releaseMouse();

                                          	//printf("not grabbing anymore\n");
					}
				}
		}

		if (QEvent::MouseButtonPress == e->type()) {
			QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);
			
			// reworked to be simpler in Qt4 port
			// might be buggy, but is logically the
			// right way to do this

			if (Qt::MidButton == mouseEvent->button())	{
			  setSingleStep(page_value);
			    return true; // Eat it.
			} else if (Qt::RightButton == mouseEvent->button()) {
			  setValue(maxValue());
			  return false; // Eat it.
			} else {
			  setValue(1);
			}
		}
	}

	return QSpinBox::eventFilter(o, e);
}


void CSpinButton::set_page_value(int p_page_value) {

	page_value=p_page_value;
}
CSpinButton::CSpinButton(QWidget *p_parent) : QSpinBox(p_parent)
{

	page_value=100;
	setAlignment(Qt::AlignRight);

}

CSpinButton::~CSpinButton()
{
}
