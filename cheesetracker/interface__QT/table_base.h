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
#ifndef TABLE_BASE_H
#define TABLE_BASE_H

#include <qfont.h>
#include <qwidget.h>
#include <qcolor.h>

/**
@author Juan Linietsky
*/
class Table_Base : public QWidget {
public:

       	enum _Colormap {

		Col_BackGround,
		Col_ForeGround,
		Col_Cursor,
		Col_BeingPlayed,
		Col_Separator,
		Colormap_max_Colors
	};

protected:
	static int color_values[];


	static QColor colors[Colormap_max_Colors];
	static bool colormap_allocated;
	static bool font_allocated;

	static QFont font;

public:

	static QColor *get_color(_Colormap p_color);
	static QFont *get_font();

	static void allocate_colormap ();
	static void allocate_font();

	Table_Base(QWidget *p_parent);

	~Table_Base();

};

#endif
