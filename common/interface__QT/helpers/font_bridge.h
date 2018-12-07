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
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FONT_BRIDGE_H
#define FONT_BRIDGE_H

#include <q3hbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qfont.h>

/**
@author Juan Linietsky
*/
class Font_Bridge : public Q3HBox {

	Q_OBJECT

	QFont* bridge;
	QLabel *label_name;
	QPushButton *font_change;

        void update_font_descr();

protected slots:

	void select_new_font();
public:

        void update();


	Font_Bridge(QFont *p_bridge,QWidget *p_parent);
	~Font_Bridge();
};

#endif
