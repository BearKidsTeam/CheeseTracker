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

#ifndef SAMPLE_INSTRUMENT_TABLE_H
#define SAMPLE_INSTRUMENT_TABLE_H



/**
 *
 * Juan Linietsky
 **/


#include <qwidget.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QEvent>
#include "trackercore/instrument.h"
#include "interface_binds/editor.h"
#include "interface__QT/table_base.h"
/**
  *@author Juan Linietsky
  */

/******************************
 interface_widget_equivtable.h
 ----------

This edits the note -> note/sample
relation in an instrument
********************************/


class Sample_Instrument_Table : public Table_Base {

	enum {

		LINE_CHAR_WIDTH=11
	};


	void draw_single_mode();

	int column_offset, row_offset;
	int cursor_x,cursor_y;
        int sample_mask;

	Instrument::Layer *instrument; //songptr!
	Editor *editor;

	void paintEvent( QPaintEvent * p_event );
	bool event(QEvent *e);
public:

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void PageUp();
	void PageDown();

	void InsertValue(int number);
	void AddSeparator();
	void ClearOrder();
        void InsertBlank();
	void DeleteRow();
	void InsertRepeat();

	void link_to_instrument_layer(Instrument::Layer *p_instrument);
	void link_to_editor(Editor *p_editor);


	Sample_Instrument_Table(QWidget *p_widget);
	~Sample_Instrument_Table();

};


#endif
