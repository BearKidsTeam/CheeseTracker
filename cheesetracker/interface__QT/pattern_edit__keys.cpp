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
#include "pattern_edit.h"
#include <qevent.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include "interface__QT/popups/cspindialog.h"
#include "components/data/keyboard_input.h"
#include "interface_binds/tracker_instance.h"


void Pattern_Edit::screen_to_pattern_coords(int &x,int &y,int &field, bool &outside) {

		outside=false;

		y-=editor->get_top_height();
		if (y<0) {
			outside=true;
		}

		y/=editor->get_row_height();
		if (y>=editor->get_visible_rows()) {
			outside=true;
		}

                y+=editor->get_row_offset();


		x-=editor->left_numbers_width();
		if (x<0) {
			outside=true;
		}
		field=x % editor->get_column_width();
		x/=editor->get_column_width();
		if (x>=editor->get_visible_columns()) {
			outside=true;
		}
                x+=editor->get_column_offset();

                Pattern *pat=song->get_pattern( editor->get_current_pattern() );
		if (!pat) {
			ERROR("damn damn damn!! why is pattern null here?");
			return;
		}

         	if (y<0) y=0;

		if (y>=pat->get_length()) y=pat->get_length()-1;

         	if (x<0) x=0;

		if (x>=64) x=63;

		field/=editor->get_font_width();

		switch (field) {

			case 1:
			case 0: field=0; break;
			case 2: field=1; break;
			case 3:
			case 4: field=2; break;
			case 5: field=3; break;
			case 6:
			case 7: field=4; break;
			case 8: field=5; break;
			case 9:
			case 10: field=6; break;
			case 11: field=7; break;
			case 12: field=8; break;
			default: field=8; break;
		};



}


bool Pattern_Edit::event ( QEvent * e  ) {

	if (e->type()==QEvent::MouseButtonPress) {

		QMouseEvent *me=(QMouseEvent*)e;
		if (me->button()!=Qt::LeftButton)
			return QWidget::event(e);

		int x,y,field;
		if (!editor->get_row_height() || !editor->get_column_width() || !song->get_pattern( editor->get_current_pattern() )) {

			return QWidget::event(e);

		}
		x=me->x();
		y=me->y();

		bool outside=false;
		field=0;

		screen_to_pattern_coords(x,y,field,outside);

		if (!outside) {
			editor->set_cursor_field(field);
			editor->set_cursor_y(y);
			editor->set_cursor_x(x);

			mouse_select.begun=true;
			mouse_select.begin_x=x;
			mouse_select.begin_y=y;
			grabMouse();
			update();

		}


		return QWidget::event(e);

	} else if (e->type()==QEvent::MouseMove) {

		if (!editor->get_row_height() || !editor->get_column_width() || !song->get_pattern( editor->get_current_pattern() )) {

			return QWidget::event(e);

		}

		QMouseEvent *me=(QMouseEvent*)e;

//		int x,y,field;

		if (mouse_select.begun || mouse_select.active) {



			if (mouse_select.begun) {

				mouse_select.begun=false;
				mouse_select.active=true;
				editor->set_cursor_y(mouse_select.begin_y);
				editor->set_cursor_x(mouse_select.begin_x);
				editor->selection_begin_at_cursor();
			}

			int x=me->x();
			int y=me->y();
			int field=0;
			bool outside=false;

			screen_to_pattern_coords(x,y,field,outside);

			editor->set_cursor_field(field);
			editor->set_cursor_y(y);
			editor->set_cursor_x(x);

			editor->selection_end_at_cursor();
			editor->adjust_view_to_cursor();
			update();
		}

		return QWidget::event(e);

	} else if (e->type()==QEvent::MouseButtonRelease) {

		if (!editor->get_row_height() || !editor->get_column_width() || !song->get_pattern( editor->get_current_pattern() )) {

			return QWidget::event(e);
		}
		QMouseEvent *me=(QMouseEvent*)e;

		if (me->button()!=Qt::LeftButton)
			return QWidget::event(e);

		if (mouse_select.active || mouse_select.begun) {

			releaseMouse();
		}


		if (mouse_select.begun) {
			editor->selection_release();
			update();

		}

		mouse_select.active=false;
		mouse_select.begun=false;


		return QWidget::event(e);

	} else if (e->type()==QEvent::Wheel) {

        	QWheelEvent *we=(QWheelEvent*)e;

		if (we->delta()>0) {

			editor->set_row_offset( editor->get_row_offset() -1 );
		} else {

			editor->set_row_offset( editor->get_row_offset() +1 );

		}
		return QWidget::event(e);

	} else if (e->type()!=QEvent::KeyPress) {

		return QWidget::event(e);
	}

	QKeyEvent *event=(QKeyEvent*)e;
	Keyboard_Input *kinput=Keyboard_Input::get_singleton_instance();

	bool alt,shift,control;
	int previous_cursor_x,previous_cursor_y;
        int aux_instrument;
	int key_value=event->ascii();
	bool grab_event=false;
	bool must_repaint=false;

	alt=event->state() & Qt::AltButton;
	shift=event->state() & Qt::ShiftButton;
	control=event->state() & Qt::ControlButton;

	if (shift && !previous_shift) editor->shift_selection_begin();
	if (!shift && previous_shift) editor->shift_selection_end();

        if (key_value>='a' && key_value<='z') key_value-=('a'-'A');

	if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

	aux_instrument=editor->get_instrument_mask_value();

	// CAUTION: The if-blocks below only allow ONE of the
	// keys Control, Alt, and Shift to be pressed for the
	// key combination to work. If you want a key combination
	// that uses both control and alt, you must create a new
	// if-block.

	if (control) {
		switch (event->key()) {

			case Qt::Key_Plus: {
				editor->orderlist_move_down();
				if (song->get_order(editor->orderlist_get_cursor_y())<MAX_PATTERNS) editor->cursor_goto_pattern(song->get_order(editor->orderlist_get_cursor_y()));
				must_repaint=true;

			} break;
			case Qt::Key_Minus: {

				editor->orderlist_move_up();
				if (song->get_order(editor->orderlist_get_cursor_y())<MAX_PATTERNS) editor->cursor_goto_pattern(song->get_order(editor->orderlist_get_cursor_y()));
				must_repaint=true;

			} break;
			case Qt::Key_Home: {

				editor->cursor_move_up(1);
				must_repaint=true;

			} break;
			case Qt::Key_F7: {

				editor->set_mark();
				must_repaint=true;
			} break;
			case Qt::Key_End: {

				editor->cursor_move_down(1);
				must_repaint=true;
			} break;
			case Qt::Key_Backspace: {

				editor->undo_index(0);
				must_repaint=true;
			} break;
		}

	} else if (shift) {
	  switch (event->key()) {
	  case Qt::Key_F11: {
	    //printf("Mute attempt\n");
	    editor->current_channel_mute_toggle();
	    must_repaint=true;
	  }break;
	  
	  case Qt::Key_F10: {
	    //printf("Solo attempt\n");
	    editor->current_channel_solo_toggle();
	    must_repaint=true;
	  }break;
	  
	  case Qt::Key_Backtab: {
	    editor->cursor_back_tab();
	    must_repaint=true;
	    grab_event=true;
	  } break;

	  }
	} else if (alt) {

	  grab_event=true;
		switch (key_value) {
			case '1': {
				editor->set_cursor_spacing(1);
			}break;
			case '2': {
				editor->set_cursor_spacing(2);
			}break;
			case '3': {
				editor->set_cursor_spacing(3);
			}break;
			case '4': {
				editor->set_cursor_spacing(4);
			}break;
			case '5': {
				editor->set_cursor_spacing(5);
			}break;
			case '6': {
				editor->set_cursor_spacing(6);
			}break;
			case '7': {
				editor->set_cursor_spacing(7);
			}break;
			case '8': {
				editor->set_cursor_spacing(8);
			}break;
			case '9': {
				editor->set_cursor_spacing(9);
			}break;
			case 'A': {

				editor->selection_lower();
				must_repaint=true;
			} break;

			case 'B': {

				editor->mark_block_begining();
				must_repaint=true;
			} break;
			case 'C': {

				editor->selection_copy();
				must_repaint=true;
			} break;

			case 'D': {

				editor->mark_column_quarter_half();
			} break;

			case 'E': {

				editor->mark_block_end();
				must_repaint=true;
			} break;

			case 'U': {

				editor->selection_release();
				must_repaint=true;
			} break;

			case 'L': {

				editor->mark_column_all();
				must_repaint=true;
			} break;
			case 'V': {

				editor->selection_set_volume_mask();
				must_repaint=true;
			} break;
			case 'S': {

				editor->selection_set_instrument_mask();
				must_repaint=true;
			} break;
			case 'W': {

				editor->selection_wipe_stray_volumes();
				must_repaint=true;
			} break;

			case 'Q': {

				editor->selection_raise();
				must_repaint=true;
			} break;
			case 'J': {

				CSpinDialog * ask_value = new CSpinDialog(this);
				ask_value->setCaption("Volume Amplifier");
				ask_value->set_description("Amplify Volume by:");
				ask_value->get_spin()->setMaxValue(200);
				ask_value->get_spin()->setSuffix("%");
				ask_value->get_spin()->setValue(volume_scale_memory);

				if (ask_value->exec() == QDialog::Accepted) {

					volume_scale_memory=ask_value->get_spin()->value();
					editor->selection_volume_scale(volume_scale_memory);
				}

				delete ask_value;

			} break;
			case 'Z': {

				editor->selection_zap();
				must_repaint=true;
			} break;
			case 'X': {

				editor->selection_parameter_ramp();
				must_repaint=true;
			} break;
			case 'K': {

				editor->selection_volume_ramp();
				must_repaint=true;
			} break;
			case 'O': {

				editor->selection_paste_overwrite();
				must_repaint=true;
			} break;
			case 'M': {

				editor->selection_paste_mix();
				must_repaint=true;
			} break;

			case 'P': {

				editor->selection_paste_insert();
				must_repaint=true;
			} break;
			case '[': {
				//tmpres=song->editor->remove_column_to_current_track();
				//if (tmpres>=0) {

				//	selected_track=tmpres;

				//	button_track_remove_pressed();
				//}

			//	must_repaint=true;
			} break;

			default: {
				grab_event=false;
			} break;

		}

		switch (event->key()) {
			case Qt::Key_Left: {

				editor->cursor_move_left(1);
				must_repaint=true;
				grab_event=true;
			} break;
			case Qt::Key_Right: {

				editor->cursor_move_right(1);
				must_repaint=true;
				grab_event=true;
			} break;
			case Qt::Key_Backspace: {


				/* @TODO
				pattern_undo_popup = new Popup_Undo_Window;
				pattern_undo_popup->button_undo.clicked.connect(slot(this,&Interface::on_pattern_undo_popup_select_row));
				for (j=0;j<editor->get_undo_max();j++) {

					pattern_undo_popup->add_row(editor->get_undo_pattern(j),editor->get_undo_info(j));
				}
				*/
				grab_event=true;


			} break;

		}

	} else {

		switch (event->key()) {


			case Qt::Key_Tab: {
				editor->cursor_tab();
				must_repaint=true;
				grab_event=true;
			} break;
			case Qt::Key_Backtab: {
				editor->cursor_back_tab();
				must_repaint=true;
				grab_event=true;
			} break;
			case Qt::Key_Insert: {

				editor->cursor_insert();
				must_repaint=true;
			} break;
			case Qt::Key_Delete: {

				editor->cursor_delete();
				must_repaint=true;
			} break;
			case Qt::Key_ScrollLock: {

				// @TODO
				//song_follow_mode=!song_follow_mode;
			} break;
		}


		if ((key_value=='+') || (key_value=='=')) {

			editor->cursor_next_pattern();
			must_repaint=true;

		}

		if (key_value=='-')  {

			editor->cursor_prev_pattern();
			must_repaint=true;

		}


		if (key_value==kinput->get_key_bind_key_nocase(KEY_TOGGLE_MASK) ) {


		editor->toggle_mask_at_cursor();
		must_repaint=true;
			grab_event=true;
		}


		if (key_value==kinput->get_key_bind_key_nocase(KEY_ERASE_FIELD)) {

			editor->clear_field_at_cursor();
			must_repaint=true;
			grab_event=true;
		}


		if (key_value==kinput->get_key_bind_key_nocase(KEY_NOTE_OFF)) {

			editor->press_note_off_at_cursor();
			must_repaint=true;
			grab_event=true;
		}
		if (key_value==kinput->get_key_bind_key_nocase(KEY_NOTE_CUT)) {

			if (editor->get_cursor_field()==0) {

				editor->press_note_cut_at_cursor();
				must_repaint=true;
			}
			grab_event=true;
		}

		if (key_value==' ') {

			editor->press_mask_write_cursor();
			must_repaint=true;
			grab_event=true;
		}
		if (key_value==kinput->get_key_bind_key_nocase(KEY_NEXT_INSTRUMENT)) {

			if (editor->get_instrument_mask_value() < 
			    MAX_INSTRUMENTS-1)
				editor->set_instrument_mask_value(editor->get_instrument_mask_value()+1);
			grab_event=true;
		}

		if (key_value==kinput->get_key_bind_key_nocase(KEY_PREV_INSTRUMENT)) {

			if (editor->get_instrument_mask_value()>0) editor->set_instrument_mask_value(editor->get_instrument_mask_value()-1);
			grab_event=true;
		}

		if (event->key()==Qt::Key_Return) {

			editor->get_mask_from_cursor();
			must_repaint=true;
			grab_event=true;
		}


		if ((editor->get_cursor_field()==0) && (key_value==kinput->get_key_bind_key_nocase(KEY_PLAY_NOTE_ON_CURSOR))) {

			if (variables_lock) variables_lock->release();
			player->play_note(editor->get_cursor_x(),song->get_pattern(editor->get_current_pattern())->get_note(editor->get_cursor_x(),editor->get_cursor_y()));
			if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

			editor->cursor_move_down();
			must_repaint=true;
			grab_event=true;

		} else if ((editor->get_cursor_field()==0) && (key_value==kinput->get_key_bind_key_nocase(KEY_PLAY_ROW_ON_CURSOR))) {

			int i;
			if (variables_lock) variables_lock->release();
			for (i=0;i<PATTERN_WIDTH;i++) player->play_note(i,song->get_pattern(editor->get_current_pattern())->get_note(i,editor->get_cursor_y()),false);
			if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

			editor->cursor_move_down();


			must_repaint=true;
			grab_event=true;

		} else if (editor->get_note_from_key(key_value) != EMPTY_FIELD
		           || ((key_value>='0') && (key_value<='9'))
		           || ((key_value>='A') && (key_value<='Z') ))  { 

			previous_cursor_x=editor->get_cursor_x();
			previous_cursor_y=editor->get_cursor_y();

			if (editor->press_key_at_cursor(key_value)) {

				if (variables_lock) variables_lock->release();
				if (key_value!='`') player->play_note(previous_cursor_x,song->get_pattern(editor->get_current_pattern())->get_note(previous_cursor_x,previous_cursor_y));
				if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

			}
			must_repaint=true;
			grab_event=true;
		}
	}

	// The arrow keys work if the SHIFT key is depressed, but
	// not if control or alt are depressed.

	if(!(control || alt)) {
		switch(event->key()) {
			case Qt::Key_Up: {
				editor->cursor_move_up();
				must_repaint=true;
				grab_event=true;

			} break;
			case Qt::Key_Down: {

				editor->cursor_move_down();
				must_repaint=true;
				grab_event=true;
			} break;
			case Qt::Key_Left: {

				editor->cursor_move_field_left();
				must_repaint=true;
				grab_event=true;
			} break;
			case Qt::Key_Right: {

				editor->cursor_move_field_right();
				must_repaint=true;
				grab_event=true;
			} break;
			case Qt::Key_PageUp: {

				editor->cursor_page_up();
				must_repaint=true;
			} break;
			case Qt::Key_PageDown: {

				editor->cursor_page_down();
				must_repaint=true;
			} break;
			case Qt::Key_Home: {

				editor->cursor_home();
				must_repaint=true;
			} break;
			case Qt::Key_End: {

				editor->cursor_end();
				must_repaint=true;
			} break;
		}
	}


	if (variables_lock) variables_lock->release();

	//doin this here, so we dont hold the lock while drawing (that would be suicidal :)
	if (must_repaint)
		repaint_after_command();


	if (aux_instrument!=editor->get_instrument_mask_value()) {
		//something changed the insturment mask.. let's update..
		instrument_changed_signal(editor->get_instrument_mask_value());
	}

	previous_shift=shift;

	operation_performed_signal();

	if (grab_event) {
		event->accept();
		return false;
	} else {
	  //printf("Doing nothing!!\n");
	  //event->ignore();
	  return QWidget::event(e);
	}
}
