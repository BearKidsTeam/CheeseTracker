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
#include "ccolor_panel.h"
//Added by qt3to4:
#include <QPaintEvent>

void CColor_Panel::paintEvent( QPaintEvent * p_event ) {

	QPainter painter(this);
	painter.fillRect(0,0,width(),height(),color);
}

const QColor &CColor_Panel::get_color() {

	return color;
}

void CColor_Panel::set_color(const QColor& p_color) {

	color=p_color;
	update();
}

CColor_Panel::CColor_Panel(QWidget *p_parent) : QWidget(p_parent) {

}

CColor_Panel::~CColor_Panel()
{
}
