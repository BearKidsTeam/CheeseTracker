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

#ifndef CCOLOR_PANEL_H
#define CCOLOR_PANEL_H


#include <qwidget.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>

/**
 *
 * Juan Linietsky
 **/
class CColor_Panel : public QWidget{

	QColor color;
	void paintEvent( QPaintEvent * p_event );
public:

	void set_color(const QColor& p_color);
	const QColor &get_color();

	CColor_Panel(QWidget *p_parent);
	~CColor_Panel();
};

#endif
