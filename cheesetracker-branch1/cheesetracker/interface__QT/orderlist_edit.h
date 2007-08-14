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

#ifndef ORDERLIST_EDIT_H_
#define ORDERLIST_EDIT_H_

#include <qwidget.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QEvent>
#include "interface_binds/editor.h"
#include "interface__QT/table_base.h"

class OrderList_Edit : public Table_Base {



	void draw_single_mode();

	int column_offset, row_offset;
	int current_playing_order;
	int cache_font_height;
	int cache_font_width;



	Editor *editor;

	void paintEvent( QPaintEvent * p_event );
	bool event(QEvent *e);

public:

	void set_current_playing_order(int p_order);

	void link_to_editor(Editor *p_editor) { editor=p_editor; update(); };

	OrderList_Edit(QWidget *p_parent);
	~OrderList_Edit();

};


#endif
