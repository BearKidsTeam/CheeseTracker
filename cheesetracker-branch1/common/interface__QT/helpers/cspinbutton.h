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

#ifndef CSPINBUTTON_H
#define CSPINBUTTON_H

#include <qspinbox.h>
//Added by qt3to4:
#include <QEvent>

/**
 *
 * Juan Linietsky
 **/
class CSpinButton : public QSpinBox {

	Q_OBJECT
	bool eventFilter( QObject *o, QEvent *e );
	QWidget *spin_widget;
	int page_value;
	bool mid_button_pressed;
	int grab_x,grab_y;
public:
	void set_page_value(int p_page_value);
	CSpinButton(QWidget *p_parent);
	~CSpinButton();
};

#endif
