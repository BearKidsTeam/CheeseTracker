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
/***************************************************************************
                          editor_commands.cpp  -  description
                             -------------------
    begin                : Thu Jan 25 2001
    copyright            : (C) 2001 by Juan Linietsky
    email                : reduz@anime.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "editor.h"
#include <stdio.h>
#include "interface_binds/tracker_instance.h"
#include "components/data/keyboard_input.h"

void Editor::cursor_move_up(int amount) {

        int limit;

	limit=0;

	cursor_y-=amount;
	if (cursor_y<0) cursor_y=0;

	shift_selection_update();

	set_flag_redraw_row();

}

void Editor::cursor_move_up() {

	cursor_move_up(cursor_spacing);
}

void Editor::cursor_move_down(int amount) {

        int limit;

	limit=song->get_pattern(current_pattern)->get_length()-1;

	cursor_y+=amount;

	if (cursor_y>limit) cursor_y=limit;

        shift_selection_update();
	set_flag_redraw_row();
}

void Editor::cursor_move_down() {

	cursor_move_down(cursor_spacing);	
}

void Editor::cursor_move_left(int amount) {

	cursor_x-=amount;
	if (cursor_x<0) cursor_x=0;
	shift_selection_update();
	set_flag_redraw_row();

}

void Editor::cursor_move_right(int amount) {

	cursor_x+=amount;
	if (cursor_x>=PATTERN_WIDTH) cursor_x=PATTERN_WIDTH-1;
	shift_selection_update();
	set_flag_redraw_row();
}


void Editor::cursor_move_field_right() {

	if (shift_selection_in_progress) {
		cursor_move_right(1);
		return;
	}

	cursor_field++;
	if (cursor_field>=FIELDS_MAX) {

		if (cursor_x==PATTERN_WIDTH-1) cursor_field=FIELDS_MAX-1;
		else {

			cursor_move_right(1);
			cursor_field=0;
		}
	}

	shift_selection_update();
	set_flag_redraw_row();

}

void Editor::cursor_move_field_left() {

	if (shift_selection_in_progress) {
		cursor_move_left(1);
		return;
	}

	cursor_field--;
	if (cursor_field<0) {

		if (cursor_x==0) cursor_field=0;
		else {
		
			cursor_move_left(1);
			cursor_field=FIELDS_MAX-1;
		}
	}

	shift_selection_update();
	set_flag_redraw_row();
}

void Editor::cursor_page_up() {

	cursor_move_up(song->variables.row_highlight_major);

}


void Editor::cursor_page_down() {

	cursor_move_down(song->variables.row_highlight_major);
}


void Editor::cursor_home() {

	if (cursor_field>0) {

		cursor_field=0;

	} else if (cursor_x>0) {

		cursor_x=0;
		cursor_field=0;
		
	} else {

		cursor_y=0;
	}

	shift_selection_update();
}

void Editor::cursor_end() {

	if (cursor_field<FIELDS_MAX-1) {

		cursor_field=FIELDS_MAX-1;

	} else if (cursor_x<PATTERN_WIDTH-1) {

		cursor_x=PATTERN_WIDTH-1;
		cursor_field=FIELDS_MAX-1;

	} else {

		cursor_y=song->get_pattern(current_pattern)->get_length()-1;
	}

	shift_selection_update();
}


void Editor::cursor_tab() {

	cursor_move_right(1);
	cursor_field=0;

}

void Editor::cursor_back_tab() {

	shift_selection_end();

	if (cursor_field==0) cursor_move_left(1);
	else cursor_field=0;

}


void Editor::cursor_next_pattern() {

	cursor_goto_pattern(current_pattern+1);

}

void Editor::cursor_prev_pattern() {

	cursor_goto_pattern(current_pattern-1);
}

void Editor::cursor_goto_pattern(int p_pattern) {
	
	if ((p_pattern<0) || (p_pattern>=MAX_PATTERNS)) return;

       	char tmp_reason[20];
	tmp_reason[19]=0;
       	snprintf(tmp_reason,19,"Pattern %i",current_pattern);
       	current_pattern=p_pattern;
	push_current_pattern_to_undo_list(tmp_reason);
       	normalize_selection();
}

void Editor::cursor_insert() {

	int i,limit;
	Note aux_note;
	limit=song->get_pattern(current_pattern)->get_length()-1;


	for(i=limit;i>cursor_y;i--) {

		aux_note=song->get_pattern(current_pattern)->get_note(cursor_x,i-1);
		song->get_pattern(current_pattern)->get_note_ref(cursor_x,i)=aux_note;
	}

	CURRENT_NOTE_SET.clear();
}

void Editor::cursor_delete() {

	int i,limit;
	
	Note aux_note;
	limit=song->get_pattern(current_pattern)->get_length()-1;

        for(i=cursor_y;i<limit;i++) {

		aux_note=song->get_pattern(current_pattern)->get_note(cursor_x,i+1);
		song->get_pattern(current_pattern)->get_note_ref(cursor_x,i)=aux_note;
//		song->get_pattern(current_pattern)->note_ref(cursor_x,i)=song->get_pattern(current_pattern)->note_ref(cursor_x,i+1);

	}

	song->get_pattern(current_pattern)->get_note_ref(cursor_x,limit).clear();

}




void Editor::toggle_mask_at_cursor() {

	switch (cursor_field) {

		case 2:
		case 3:{ instrument_mask=!instrument_mask; } break;
		case 4:
		case 5:{ volume_mask=!volume_mask; } break;
		case 6:
		case 7:
		case 8:{ command_mask=!command_mask; } break;
	}

}

void Editor::clear_field_at_cursor() {

	switch (cursor_field) {

		case 0:{

			CURRENT_NOTE_SET.note=EMPTY_FIELD;
			if (volume_mask) CURRENT_NOTE_SET.volume=EMPTY_FIELD;
			if (instrument_mask) CURRENT_NOTE_SET.instrument=EMPTY_FIELD;
			if (command_mask) CURRENT_NOTE_SET.command=EMPTY_FIELD;
			last_note=CURRENT_NOTE_GET.note;

		} break;
		case 1: break;
		case 2:
		case 3:{

			CURRENT_NOTE_SET.instrument=EMPTY_FIELD;
			last_instrument=EMPTY_FIELD;
		} break;
		case 4:
		case 5:{

			CURRENT_NOTE_SET.volume=EMPTY_FIELD;
			last_volume=EMPTY_FIELD;;
		} break;
			
		case 6: {

			CURRENT_NOTE_SET.command=EMPTY_FIELD;
			last_command=CURRENT_NOTE_GET.command;
			last_parameter=CURRENT_NOTE_GET.parameter;
		} break;
		case 7:
		case 8:{

			CURRENT_NOTE_SET.parameter=0;
			last_command=CURRENT_NOTE_GET.command;
			last_parameter=CURRENT_NOTE_GET.parameter;
		} break;
	}

	cursor_move_down();

}


// get_note_increment - Returns the musical-keyboard binding
//                      associated with {key}.
//
// arguments          - key: The ASCII value of the key.
//
// notes              - Lowercase letters are not handled.
//
// bugs               - The design of this function doesn't
//                      allow special keys, such as F1-F12,
//                      the cursor keys, SHIFT, CTRL, or ALT,
//                      to be used as musical-keyboard keys.
//
//                    - It is unknown if QKeyEvent::ascii(),
//                      used to obtain the ASCII values passed
//                      as {key}, handles keypresses such as
//                      Esc, Del, and Backspace in the expected
//                      manner (\033, \177, and \008 respectively).
int Editor::get_note_increment(char key)
{

	//fixme: internationaliza it! :D

	int i;
	int tmpval=99;

	Keyboard_Input *kinput=Keyboard_Input::get_singleton_instance();
	
	for (i=KEY_C_1;i<KEY_MAX;i++) {
		if (toupper(kinput->get_key_bind_key_nocase(i))==key)
			tmpval=i-KEY_C_1;
	}

	return tmpval;
}


void Editor::default_octave_raise() {

	default_octave++;
	default_octave=BOUND(default_octave,0,8);

}

void Editor::default_octave_lower() {

	default_octave--;
	default_octave=BOUND(default_octave,0,8);


}
void Editor::set_default_octave(char value) {

	if (value<0) return;
	if (value>8) return;
	default_octave=value;
}

char Editor::get_default_octave() {

	return default_octave;
}

void Editor::perform_action(Pattern_Actions p_action) {


	switch (p_action) {

		case Pattern_MARK_BLOCK_BEGIN: {
			mark_block_begining();
		} break;
		case Pattern_MARK_BLOCK_END: {
                	mark_block_end();
		} break;
		case Pattern_MARK_COLUMN_SIXTEEN: {
			mark_column_sixteen();		  
		} break;

		case Pattern_MARK_COLUMN_QUARTER_HALF: {
			mark_column_quarter_half();  	  
		} break;

		case Pattern_MARK_COLUMN_ALL: {
                	mark_column_all();
		} break;
		case Pattern_BLOCK_COPY: {
	                selection_copy();
		} break;
		case Pattern_BLOCK_PASTE_INSERT: {
                	selection_paste_insert();
		} break;
		case Pattern_BLOCK_PASTE_OVERWRITE: {
	                selection_paste_overwrite();
		} break;
		case Pattern_BLOCK_UNMARK: {
	                selection_release();
		} break;
		case Pattern_BLOCK_CUT: {
	                selection_zap();
		} break;
		case Pattern_BLOCK_RAISE_NOTES: {
			selection_raise();
		} break;
		case Pattern_BLOCK_LOWER_NOTES: {
	                selection_lower();
		} break;
		case Pattern_BLOCK_SET_CURRENT_INSTRUMENT: {
			selection_set_instrument_mask();
		} break;
		case Pattern_BLOCK_SET_CURRENT_VOLUME: {
	                selection_set_volume_mask();
		} break;
		case Pattern_BLOCK_WIPE_STRAY_VOLUMES: {
	                selection_wipe_stray_volumes();
		} break;
		case Pattern_BLOCK_RAMP_WIPE_VOLUMES: {

	                selection_volume_ramp();
		} break;
		case Pattern_BLOCK_RAMP_WIPE_EFFECTS: {

	                selection_parameter_ramp();
		} break;
		case Pattern_BLOCK_DOUBLE_BLOCK_LENGTH: {

			selection_expand();
		} break;
		case Pattern_BLOCK_HALVE_BLOCK_LENGTH: {

			selection_shrink();

		} break;
		case Pattern_FIELD_TOGGLE_MASK: {

			toggle_mask_at_cursor();

		} break;
		case Pattern_FIELD_CLEAR: {

			clear_field_at_cursor();

		} break;
		case Pattern_FIELD_COPY_CURRENT: {

                        get_mask_from_cursor();
		} break;
		case Pattern_FIELD_PASTE_CURRENT: {

			press_mask_write_cursor();
		} break;
		default: {

		} break; //bleh :)
	}

}

