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
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "table_base.h"

QColor Table_Base::colors[Colormap_max_Colors];

bool Table_Base::colormap_allocated=false;
bool Table_Base::font_allocated=false;
QFont Table_Base::font;

int Table_Base::color_values[] = {
    10, 20, 30,
    100, 200, 100,
    230, 250, 230,
    30, 30, 30,
    255, 255, 255
};

QColor *Table_Base::get_color(_Colormap p_color) {

	return &colors[p_color];
}

QFont *Table_Base::get_font() {

	return &font;
}

void Table_Base::allocate_colormap () {


	if (colormap_allocated)
		return;
	int i;

	for (i=0;i<Colormap_max_Colors*3;i+=3) {

		colors[i/3].setRgb(color_values[i],color_values[i+1],color_values[i+2]);
	}

	colormap_allocated=true;
}

void Table_Base::allocate_font() {

	if (font_allocated)
		return;
#if defined(MACOSX_ENABLED)
	font.setFamily("Monaco CYls");
	font.setPointSize(11);
#elseif defined(CYGWIN_ENABLED)

	font.setFamily("Terminal");
	font.setPointSize(10);

#else

	font.setFamily("Fixed");
	font.setPointSize(11);
	font.setBold(true);
#endif

	font.setStyleHint(QFont::Courier,QFont::NoAntialias);

	font_allocated=true;
}


Table_Base::Table_Base(QWidget *p_parent) : QWidget(p_parent) {

	allocate_colormap();
	allocate_font();
}


Table_Base::~Table_Base()
{
}


