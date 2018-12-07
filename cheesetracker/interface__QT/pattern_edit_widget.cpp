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
#include "pattern_edit_widget.h"
#include "interface__QT/popups/cspindialog.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QLabel>


void Pattern_Edit_Widget::focus_pattern_edit_widget() {

	pattern_edit->setFocus();

}

void Pattern_Edit_Widget::configure(Song *p_song,Editor *p_editor,Player_Data *p_player) {

	pattern_edit->configure(p_song,p_editor,p_player);
	editor=p_editor;
	song=p_song;
}


void Pattern_Edit_Widget::row_offset_changed_callback(int p_to_which) {

	editor->set_row_offset(p_to_which);
	pattern_edit->update();
}

void Pattern_Edit_Widget::column_offset_changed_callback(int p_to_which) {

	editor->set_column_offset(p_to_which);
	pattern_edit->update();
}

void Pattern_Edit_Widget::timer_callback() {

	if ((editor==NULL) || (song==NULL))
		return;

	if (timer_callback_count==0) //since it's the first time..
		pattern_len_spin->setValue(song->get_pattern(editor->get_current_pattern())->get_length());

	timer_callback_count++;

	if (pattern_spin->value()!=editor->get_current_pattern()) {

		pattern_spin->setValue(editor->get_current_pattern());
		//only if the pattern changed, i can change the length
		pattern_len_spin->setValue(song->get_pattern(editor->get_current_pattern())->get_length());
	}


	if ((timer_callback_count%2)==0) {

		int vis_rows=editor->get_visible_rows();
		int vis_cols=editor->get_visible_columns();
		if (hscroll->value()!=editor->get_column_offset())
			hscroll->setValue( editor->get_column_offset() );

		if (vscroll->pageStep()!=vis_rows) {

			vscroll->setPageStep(vis_rows);
		}
		Pattern *pat = song->get_pattern( editor->get_current_pattern() );

		if ((pat != NULL) && (vscroll->maxValue()!=(pat->get_length()-vis_rows) ))
			vscroll->setMaxValue( pat->get_length()-vis_rows );

		if (vscroll->value()!=editor->get_row_offset())
			vscroll->setValue( editor->get_row_offset() );

		if (hscroll->maxValue()!=(64-vis_cols)) {

			hscroll->setMaxValue(64-vis_cols);
		}
		if (hscroll->pageStep()!=vis_cols) {

			hscroll->setPageStep(vis_cols);
		}

	}

	if ((timer_callback_count%7)==0) {

		int patterns_used=song->get_patterns_used();

		if (display_cache.patterns_used!=patterns_used) {
			char buf[20];
			buf[19]=0;
			snprintf(buf, 19, "/%i",song->get_patterns_used()-1);
			display_cache.patterns_used=patterns_used;
			pattern_spin->setSuffix(buf);

		}
	}



	if (editor->orderlist_get_cursor_y()!=order_spin->value()) {

		order_spin->setValue(editor->orderlist_get_cursor_y());
	}

	if ((timer_callback_count%7)==0) {

		int orders_used=song->get_used_orders_amount();

		if (display_cache.orders_used!=orders_used) {
			char buf[20];
			buf[19]=0;
			snprintf(buf,19,"/%i",orders_used-1);
			display_cache.orders_used=orders_used;
			order_spin->setSuffix(buf);
		}
	}

	if ((timer_callback_count%7)==0) {

		int current_instrument=editor->get_instrument_mask_value();

		string instrument_text;

		if(current_instrument != EMPTY_FIELD) {
			instrument_text=song->get_instrument(current_instrument)?song->get_instrument(current_instrument)->name:"";
		}

		if ((current_instrument!=(instrument_edit->value()-1)) || (display_cache.last_instrument_text!=instrument_text)) {
			instrument_edit->setValue(current_instrument+1);
			display_cache.last_instrument_text=instrument_text;
			instrument_text+=" - ";

			instrument_edit->setPrefix(instrument_text.c_str());
		}
	}
}

void Pattern_Edit_Widget::create_undo_list() {

	if (!editor->has_undo_display_pending())
		return;

	undo_history->clear();
	char buf[100];
	buf[99]=0;

	for (int i=0;i<editor->get_undo_max();i++) {

		snprintf(buf, sizeof(buf)-1, "%i- %s",editor->get_undo_pattern(i),editor->get_undo_info(i).c_str());
		undo_history->insertItem( buf, i );
	}

	undo_history->setCurrentItem(0);
	editor->reset_pending_undo_display();
}


void Pattern_Edit_Widget::set_pattern_len() {

	CSpinDialog * ask_value = new CSpinDialog(this);
	ask_value->setCaption("Resize pattern");
	ask_value->set_description("Resize the next:");
	ask_value->get_spin()->setMaxValue(200);
	ask_value->get_spin()->setSuffix(" patterns");
	ask_value->get_spin()->setValue(0);

	if (ask_value->exec() == QDialog::Accepted) {

		int resize_count=ask_value->get_spin()->value()+1;

		Pattern *pat;
		for (int i=0;i<resize_count;i++) {
			pat=song->get_pattern(editor->get_current_pattern()+i);
			if (pat==NULL)
				break;
			pat->set_length(pattern_len_spin->value());
			pattern_edit->update();
		}
	}

	delete ask_value;


}

void Pattern_Edit_Widget::undo_button_callback() {

	editor->undo_index(undo_history->currentItem());
	create_undo_list();
	pattern_edit->update();

}

void Pattern_Edit_Widget::pattern_spin_changed_cbk(int p_to_which) {

	editor->set_current_pattern(pattern_spin->value());
	char buf[40];
	buf[39]=0;

	snprintf(buf,39, "Pattern %i",editor->get_current_pattern());
	editor->push_current_pattern_to_undo_list(buf);
	create_undo_list();
	pattern_edit->update();
}

void Pattern_Edit_Widget::order_spin_changed_cbk(int p_to_which) {

	int new_pattern=order_spin->value();
	editor->orderlist_set_cursor_y(new_pattern);
	new_pattern=song->get_order(new_pattern);
	if (new_pattern<199)
	editor->set_current_pattern(new_pattern);
	pattern_edit->update();

//	pattern_edit->update();


}

void Pattern_Edit_Widget::instrument_spin_changed_cbk(int p_to_which) {

	instrument_changed(p_to_which-1);
}

void Pattern_Edit_Widget::reupdate_components() { //needed from outside :(

	create_undo_list();
	pattern_edit->update();
}

Pattern_Edit_Widget::Pattern_Edit_Widget(QWidget *p_parent) : Q3VBox(p_parent) {

	editing_options=new Q3HBox(this);
	editing_options->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));

	new QLabel("Pattern:",editing_options);
	pattern_spin= new CSpinButton(editing_options);
	pattern_spin->setMaxValue(199);
	pattern_spin->setSuffix("/0");
	pattern_spin->setFocusPolicy(Qt::NoFocus);
	QObject::connect( pattern_spin, SIGNAL(valueChanged(int)), this, SLOT(pattern_spin_changed_cbk(int)) );

	Q3Frame * aux_frame = new Q3Frame(editing_options);
	aux_frame->setFrameStyle(VLine+Sunken);
	aux_frame->setMinimumWidth(20);

	new QLabel("Order:",editing_options);
	order_spin= new CSpinButton(editing_options);
	order_spin->setSuffix("/199");
	order_spin->setMaxValue(199);
	order_spin->setFocusPolicy(Qt::NoFocus);
	QObject::connect( order_spin, SIGNAL(valueChanged(int)), this, SLOT(order_spin_changed_cbk(int)) );

	aux_frame = new Q3Frame(editing_options);
	aux_frame->setFrameStyle(VLine+Sunken);
	aux_frame->setMinimumWidth(20);

	new QLabel("Pattern Length:",editing_options);
	pattern_len_spin= new CSpinButton(editing_options);
	pattern_len_spin->setMaxValue(199);
	pattern_len_set= new QPushButton("Change",editing_options);
	pattern_len_set->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum));


	aux_frame = new Q3Frame(this);
	aux_frame->setFrameStyle(HLine+Sunken);
	aux_frame->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	editing_options_instrument=new Q3HBox(this);
	editing_options_instrument->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Maximum));

	new QLabel("Undo History:",editing_options_instrument);
        undo_history = new QComboBox(editing_options_instrument);
	undo_history->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	editing_options_instrument->setStretchFactor(undo_history,3);
	undo_button = new QPushButton("Undo",editing_options_instrument);
	QObject::connect( undo_button, SIGNAL(clicked()), this, SLOT(undo_button_callback()) );

	aux_frame = new Q3Frame(editing_options_instrument);
	aux_frame->setFrameStyle(VLine+Sunken);
	aux_frame->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	aux_frame->setMinimumWidth(20);

	new QLabel("Instrument:",editing_options_instrument);
	//auxlabel->setAlignment(AlignRight | AlignVCenter | ExpandTabs);
	//editing_options_instrument->setStretchFactor(auxlabel,3);

	instrument_edit = new CSpinButton(editing_options_instrument);
	instrument_edit->setMinValue(1);
	instrument_edit->setFocusPolicy(Qt::NoFocus);

	editing_options_instrument->setStretchFactor(instrument_edit,2);
	QObject::connect( instrument_edit, SIGNAL(valueChanged(int)), this, SLOT(instrument_spin_changed_cbk(int)) );

	aux_frame = new Q3HBox(this);
	aux_frame->setFrameStyle(Panel+Sunken);
	aux_frame->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));



	Q3Grid *grid = new Q3Grid(2,aux_frame);
	pattern_edit = new Pattern_Edit(grid);
	pattern_edit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	vscroll = new QScrollBar(Qt::Vertical,grid);
	vscroll->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding));

	hscroll = new QScrollBar(Qt::Horizontal,grid);
	hscroll->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	//	editing_options->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	QObject::connect( pattern_edit, SIGNAL(instrument_changed_signal(int)), this, SLOT(instrument_changed(int)) );
	QObject::connect( pattern_edit, SIGNAL(operation_performed_signal()), this, SLOT(create_undo_list()) );

	QObject::connect( pattern_len_set, SIGNAL(clicked()), this, SLOT(set_pattern_len()) );

	QObject::connect( vscroll, SIGNAL(valueChanged(int)),this,SLOT(row_offset_changed_callback(int)));
	QObject::connect( hscroll, SIGNAL(valueChanged(int)),this,SLOT(column_offset_changed_callback(int)));

	timer_callback_count=0;

	display_cache.patterns_used=-1;
	display_cache.orders_used=-1;
}

Pattern_Edit_Widget::~Pattern_Edit_Widget()
{
}
