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

#ifndef CCOLOR_BRIDGE_H
#define CCOLOR_BRIDGE_H


#include <q3hbox.h>
#include <qlabel.h>
#include "interface__QT/helpers/ccolor_panel.h"
#include <qpushbutton.h>

/**
 *
 * Juan Linietsky
 **/
class CColor_Bridge : public Q3HBox {
	Q_OBJECT
	QLabel *label;
	QPushButton *button;
        CColor_Panel *color;
	QColor *bridge;


public slots:

	void color_select();

public:

	void set_color(const QColor& p_color);
	QColor * get_color();
	CColor_Bridge(QWidget *p_parent,QColor *p_bridge,const QString &p_text);
	~CColor_Bridge();
};

#endif
