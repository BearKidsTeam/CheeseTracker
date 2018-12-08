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

#ifndef PATTERN_EDIT_WIDGET_H
#define PATTERN_EDIT_WIDGET_H


#include "interface__QT/pattern_edit.h"
#include "interface__QT/helpers/cspinbutton.h"
#include <q3hbox.h>
#include <q3vbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcombobox.h>

#include <q3grid.h>
#include <qscrollbar.h>

/**
 *
 * Juan Linietsky
 **/
class Pattern_Edit_Widget : public Q3VBox {

	Q_OBJECT

	int timer_callback_count;

	struct DisplayCache {

		int patterns_used;
		int orders_used;
		string last_instrument_text;
	} display_cache;

	Q3HBox *editing_options;
	Q3HBox *editing_options_instrument;

	CSpinButton *pattern_spin;
	QLabel *pattern_spin_max;

	CSpinButton *order_spin;
	QLabel *order_max;

	CSpinButton *pattern_len_spin;
	QPushButton *pattern_len_set;

	CSpinButton *instrument_edit;

        QComboBox *undo_history;
	QPushButton *undo_button;



	QScrollBar *hscroll,*vscroll;
	Pattern_Edit *pattern_edit;

	Song *song;
	Editor *editor;



public Q_SLOTS:

	void pattern_spin_changed_cbk(int p_to_which);
	void order_spin_changed_cbk(int p_to_which);
	void instrument_spin_changed_cbk(int p_to_which);

	void set_pattern_len();

	void undo_button_callback();
	void create_undo_list();

	void row_offset_changed_callback(int p_to_which);
	void column_offset_changed_callback(int p_to_which);
public:
Q_SIGNALS:

	void instrument_changed(int p_new_one);

public:

	Pattern_Edit * get_pattern_edit() { return pattern_edit; }
	void set_variables_lock(Mutex_Lock *p_mtx_lock) { pattern_edit->set_variables_lock(p_mtx_lock); }
	void reupdate_components(); //needed from outside :(

	void focus_pattern_edit_widget();
	void timer_callback();
	void configure(Song *p_song,Editor *p_editor,Player_Data *p_player);


	Pattern_Edit_Widget(QWidget *p_parent);
	~Pattern_Edit_Widget();
};

#endif
