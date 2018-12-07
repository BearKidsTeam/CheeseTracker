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
                          editor.cpp  -  description
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
#include <cassert>


char noteincr_qwerty[32] = {'Z','S','X','D','C','V','G','B','H','N','J','M','Q','2','W','3','E','R','5','T','6','Y','7','U','I','9','O','0','P','[','=',']'};

int Editor::default_octave=4;

const char* Editor::notes[128] = {

	"C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "B-0",
	"C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
	"C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
	"C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
	"C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
	"C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
	"C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
	"C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "B-7",
	"C-8", "C#8", "D-8", "D#8", "E-8", "F-8", "F#8", "G-8", "G#8", "A-8", "A#8", "B-8",
	"C-9", "C#9", "D-9", "D#9", "E-9", "F-9", "F#9", "G-9", "G#9", "A-9", "A#9", "B-9",
	"C-A", "C#A", "D-A", "D#A", "E-A", "F-A", "F#A", "G-A"
};



Editor::Editor(){

     	song=NULL;

	note_increments=noteincr_qwerty;
	cursor_spacing=1;

	last_note=EMPTY_FIELD;
	last_instrument=1;
	last_volume=EMPTY_FIELD;
	last_command=EMPTY_FIELD;
	last_parameter=0;

	previous_cursor_y=0;
	previous_cursor_x=0;

	current_pattern=0;
        volume_pan_mode=false;
	clear_redraw_flags();

	instrument_mask=true;
	volume_mask=true;
	command_mask=false;

	selection_active=false;
        undo_max=-1;
	playing=false;
	playing_row_old=-1;

	orderlist_cursor_x=0;
	orderlist_cursor_y=0;
	current_pattern=0;

	cursor_x=0;
	cursor_y=0;
	cursor_field=0;

	undo_display_pending=true;

	mark_active=false;

	shift_selection_in_progress=false;
	window_height=0;

	song_follow=false;
	row_offset=0;
	column_offset=0;
}

void Editor::set_instrument_mask_value(int p_value) {
	assert(p_value < MAX_INSTRUMENTS || p_value==EMPTY_FIELD);
	last_instrument=p_value;
}

int Editor::get_instrument_mask_value() {
	assert(last_instrument < MAX_INSTRUMENTS ||
	       last_instrument==EMPTY_FIELD);
	return last_instrument;
}

void Editor::adjust_view_to_cursor() {


	if (cursor_x>=PATTERN_WIDTH) cursor_x=PATTERN_WIDTH-1;

	if (column_offset>cursor_x) {

		//[todo] gdk_copy_area for SCROLLING! would be neat in the future
		column_offset=cursor_x;
		set_flag_redraw_all();
        }

	if (column_offset+(get_visible_columns()-1)<cursor_x) {

		//[todo] gdk_copy_area for SCROLLING! would be neat in the future
		column_offset=cursor_x-(get_visible_columns()-1);
		set_flag_redraw_all();
	}

	if (row_offset>cursor_y) {


		//[todo] gdk_copy_area for SCROLLING! would be neat in the future
		row_offset=cursor_y;
		set_flag_redraw_all();
	}

	if (row_offset+(get_visible_rows()-1)<cursor_y) {


		//[todo] gdk_copy_area for SCROLLING! would be neat in the future
		row_offset=cursor_y-(get_visible_rows()-1);
		set_flag_redraw_all();
	}

}


int Editor::get_visible_columns() {

	int tmpint;

	tmpint=get_column_width() ? (window_width-left_numbers_width())/get_column_width() : 0;

	if (column_offset+tmpint>=PATTERN_WIDTH) tmpint-=((column_offset+tmpint)-PATTERN_WIDTH);

	return tmpint;
}

int Editor::get_visible_rows() {

	int tmpint;

	tmpint=(window_height-get_top_height())/get_row_height();

	if ( tmpint>(song->get_pattern(current_pattern)->get_length()-row_offset) )
		tmpint=song->get_pattern(current_pattern)->get_length()-row_offset;

	return tmpint;
}

void Editor::get_note_string(int column,int row, char *buf) {

	Note note;

//	const char *notestring;
//      	unsigned char tmpvol;

	//get note from editor;

	try {
		note=song->get_pattern(current_pattern)->get_note(column,row);
	} catch(Out_Of_Bounds OOB) {
		note.clear();
	}

	/* the note */



	buf[0] = get_pattern_note_string(note)[0];
	buf[1] = get_pattern_note_string(note)[1];
	buf[2] = get_pattern_note_string(note)[2];


	/* instrument */
	buf[3]=get_pattern_instrument_string(note)[0];
	buf[4]=get_pattern_instrument_string(note)[1];
	buf[5]=get_pattern_instrument_string(note)[2];

	/* volume effects */

	buf[6] = get_pattern_volume_string(note)[0];
	buf[7] = get_pattern_volume_string(note)[1];
	buf[8] = get_pattern_volume_string(note)[2];
	buf[9] = ' ';

	/* Command */
	buf[10] = get_pattern_command_string(note)[0];
	buf[11] = get_pattern_command_string(note)[1];
	buf[12] = get_pattern_command_string(note)[2];

	buf[13] = 0; // string ends here

}

string Editor::get_pattern_note_string(Note p_note) {

	string note;

	if (p_note.note==EMPTY_FIELD) {

		note = "...";

	} else if (p_note.note<Note::NOTES) {

	       	note = notes[p_note.note];

	} else if (p_note.note==Note::OFF) {

		note = "===";
	} else if (p_note.note==Note::CUT) {

		note = "^^^";
	} else {

		note = "???";
	}

	return note;
}

string Editor::get_pattern_instrument_string(Note p_note) {

	string buf=" xx";

	if (p_note.instrument==EMPTY_FIELD) {

		buf[1] = '.';
		buf[2] = '.';

	} else {

		buf[1] = '0'+(p_note.instrument+1)/10;
		buf[2] = '0'+(p_note.instrument+1) % 10;
	}

	return buf;
}

string Editor::get_pattern_volume_string(Note p_note) {

	string buf=" ..";

	int tmpvol=p_note.volume;

	/* volume */

	if (tmpvol<65) {

		buf[1] = '0'+tmpvol/10;
		buf[2] = '0'+tmpvol % 10;


	} else if (tmpvol<75) {

		buf[1] = 'A';
		buf[2] = '0'+(tmpvol-65) % 10;

	} else if (tmpvol<85) {

		buf[1] = 'B';
		buf[2] = '0'+(tmpvol-75) % 10;

	} else if (tmpvol<95) {

		buf[1] = 'C';
		buf[2] = '0'+(tmpvol-85) % 10;

	} else if (tmpvol<105) {

		buf[1] = 'D';
		buf[2] = '0'+(tmpvol-95) % 10;

	} else if (tmpvol<115) {

		buf[1] = 'E';
		buf[2] = '0'+(tmpvol-105) % 10;

	} else if (tmpvol<125) {

		buf[1] = 'F';
		buf[2] = '0'+(tmpvol-115) % 10;

	} else if (tmpvol<128) {

		buf[1] = '?';
		buf[2] = '?';

	} else if (tmpvol<193) {

		buf[0] = 'P';
		buf[1] = '0'+(tmpvol-128)/10;
		buf[2] = '0'+(tmpvol-128) % 10;


	} else if (tmpvol<203) {

		buf[1] = 'G';
		buf[2] = '0'+(tmpvol-193) % 10;

	} else if (tmpvol<213) {

		buf[1] = 'H';
		buf[2] = '0'+(tmpvol-203) % 10;

	} else if (tmpvol<EMPTY_FIELD) {

		buf[1] = '?';
		buf[2] = '?';
	} else {

		buf[1] = '.';
		buf[2] = '.';
	}


	return buf;

}
string Editor::get_pattern_command_string(Note p_note) {

	string buf="...";
 	static const char hexn[] = {
        	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
	};
	

	if (p_note.command==EMPTY_FIELD) {

		buf[0] = '.';
	} else {

		buf[0] ='A'+p_note.command;

	}

	/* Parameter */


	buf[1] = hexn[p_note.parameter/0x10];
	buf[2] = hexn[p_note.parameter&0xF];

	return buf;


}

void Editor::set_note_increment_table(Uint8* p_table) {

	note_increments=(char*)p_table;

}

Uint8 Editor::get_note_from_key(int key)
{
	int tmpinc=get_note_increment(toupper(key));
	if (tmpinc!=99) {
		return BOUND((12*default_octave+tmpinc),0,Note::NOTES);
        } else {
		return EMPTY_FIELD;
	}

}

string Editor::get_single_note_string(int p_note)
{
	if ((p_note<0) || (p_note>=Note::NOTES)) return "???";

	else return notes[p_note];
}

void Editor::channel_mute_toggle(int p_track)
{
	if ((p_track<0) || (p_track>=PATTERN_WIDTH)) return;

	song->initial_variables.channel[p_track].mute=!song->initial_variables.channel[p_track].mute;
}

void Editor::channel_solo_toggle(int p_track)
{
	if ((p_track<0) || (p_track>=PATTERN_WIDTH)) return;

        int i,muted_tracks;

        muted_tracks=0;

        // Count the muted tracks besides the current one
        for (i=0;i<PATTERN_WIDTH;i++) {

                if ((i!=p_track) && (song->initial_variables.channel[i].mute)) muted_tracks++;
        }

        if ((muted_tracks==PATTERN_WIDTH-1) && (!song->initial_variables.channel[p_track].mute)) {

                //[[UNMUTE ALL]]
                for (i=0;i<PATTERN_WIDTH;i++) song->initial_variables.channel[i].mute=false;

        } else {

                for (i=0;i<PATTERN_WIDTH;i++) {

                        if (i==p_track) {

				song->initial_variables.channel[i].mute=false;
                        } else {

				song->initial_variables.channel[i].mute=true;
                        }
                }
        }
}

void Editor::set_column_offset(int value)
{
	column_offset=value;

	if (cursor_x<column_offset)
		cursor_x=column_offset;
	if (cursor_x>=(column_offset+get_visible_columns())) {

		cursor_x=(column_offset+get_visible_columns())-1;
	}
}
void Editor::set_row_offset(int value)
{

	int new_row_offset=value;

	if (new_row_offset<0)
		new_row_offset=0;

	Pattern *pat=song->get_pattern(get_current_pattern());

	if (!pat)
		return;

	if (new_row_offset>(pat->get_length()-get_visible_rows()))
		new_row_offset=(pat->get_length()-get_visible_rows());

	row_offset=new_row_offset;
			
	if (cursor_y<row_offset)

		cursor_y=row_offset;
	if (cursor_y>=(row_offset+get_visible_rows())) {

		cursor_y=(row_offset+get_visible_rows())-1;
	}
}

void Editor::set_mark()
{

        if (mark_active && (current_pattern==marked_pattern) && (cursor_y==marked_row)) {

		mark_active=false;

	} else {

		marked_pattern=current_pattern;
		marked_row=cursor_y;
		mark_active=true;
	}
}

void Editor::current_channel_mute_toggle()
{
	channel_mute_toggle(cursor_x);

}
void Editor::current_channel_solo_toggle()
{
	channel_solo_toggle(cursor_x);
}

Editor::~Editor()
{
}
