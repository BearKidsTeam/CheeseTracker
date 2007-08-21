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
                          editor_insertion.cpp  -  description
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

#include <cassert>
#include "editor.h"


#define IS_LETTER(value) ((value>='A') && (value<='Z'))
#define IS_NUMBER(value) ((value>='0') && (value<='9'))
#define IS_HEX_NUMBER(value) ( ((value>='0') && (value<='9')) || ((value>='A') && (value<='F')) )
#define CHAR_TO_HEX(value) ((value>='0') && (value<='9'))?(value-'0'):((value>='A') && (value<='F'))?(10+(value-'A')):0
//a few macros!



//insert data stuff


bool Editor::insert_note_value_at_cursor(int p_note) {


	CURRENT_NOTE_SET.note=p_note;
	//remember mask
	last_note=CURRENT_NOTE_GET.note;
	//and APPLY mask
	if (volume_mask) CURRENT_NOTE_SET.volume=last_volume;

	if (command_mask) {

		CURRENT_NOTE_SET.command=last_command;
		CURRENT_NOTE_SET.parameter=last_parameter;
	}

	if (instrument_mask) {

		CURRENT_NOTE_SET.instrument=last_instrument;
	}

	//cursor moves
	cursor_move_down(cursor_spacing);

	return true;
}

bool Editor::insert_note_at_cursor(char keynote) {

        Uint8 tmpnote;
	Uint8 tmpinc;
	bool success;
	success=false;

	tmpinc=get_note_increment(toupper(keynote));

	if (tmpinc!=99) {

		tmpnote=(12*default_octave+tmpinc); // 0 = nothing
		if (tmpnote>=Note::NOTES) tmpnote=Note::NOTES-1;
                insert_note_value_at_cursor(tmpnote);
		success=true;
	}

	return success;

}

void Editor::insert_octave_at_cursor(Uint8 octave) {

	if (CURRENT_NOTE_GET.note>=Note::NOTES) return;
	CURRENT_NOTE_SET.note=(octave*12+(CURRENT_NOTE_GET.note) % 12); // 0 = nothing
	cursor_move_down();
}

void Editor::insert_instrument_at_cursor(Uint8 instrument, int field) {

	int tmp_inst;

	assert(field == 1 || field == 2);

	tmp_inst=CURRENT_NOTE_GET.instrument;

	if (tmp_inst==EMPTY_FIELD) tmp_inst=0;
        else tmp_inst++;

	if(field==1) {
		tmp_inst=(tmp_inst % 10);
		tmp_inst+=instrument*10;
	}
	else {
		tmp_inst=(tmp_inst / 10)*10;
		tmp_inst+=instrument;
	}

	if (tmp_inst==0) tmp_inst=EMPTY_FIELD;
	else {

		if (tmp_inst>99) tmp_inst=99;
		tmp_inst--;
	}


	CURRENT_NOTE_SET.instrument=tmp_inst;
	last_instrument=tmp_inst;

	switch(field)
	{
		case 1:
			cursor_move_field_right();
			break;
		case 2:
			cursor_move_down();
			cursor_move_field_left();
			break;
	}
	
}

// FIXME: Arrrghh! The *_field1 and *_field2 functions are near-duplicates
//        of each other!!!!

void Editor::insert_volume_at_cursor_field1(Uint8 volume) {

	Uint8 tmpvol,rightvol=0;

        tmpvol=CURRENT_NOTE_GET.volume;
	
	if (tmpvol==EMPTY_FIELD) {

		rightvol=0;

	} else if (tmpvol<65) {

		rightvol=tmpvol % 10;

	} else if (tmpvol<125) {

		rightvol=(tmpvol-65) % 10;

	} else if (tmpvol<193) {

		rightvol=(tmpvol-128) % 10;

	} else if (tmpvol<213) {

		rightvol=(tmpvol-193) % 10;
	}

	if (IS_NUMBER(volume)) {

                tmpvol=(volume-'0')*10+rightvol;
		if (tmpvol>64) tmpvol=64;  	
		if (volume_pan_mode) tmpvol+=128;

	} else if ((volume>='A') && (volume<='F')) {

		tmpvol=65+(volume-'A')*10+rightvol;				

	} else if ((volume>='G') && (volume<='H')) {

		tmpvol=193+(volume-'G')*10+rightvol;				

	} else return;

	CURRENT_NOTE_SET.volume=tmpvol;
	last_volume=tmpvol;

	cursor_move_field_right();
	
}

void Editor::insert_volume_at_cursor_field2(Uint8 volume) {

	Uint8 tmpvol,leftvol=0;

        tmpvol=CURRENT_NOTE_GET.volume;


 	if (tmpvol==EMPTY_FIELD) {

		leftvol=0;

	} else if (tmpvol<65) {

		leftvol=tmpvol/10*10;

	} else if (tmpvol<125) {

		leftvol=(((tmpvol-65) / 10)*10)+65;

	} else if (tmpvol<193) {

		leftvol=((tmpvol-128) / 10)*10;

	} else if (tmpvol<213) {

		leftvol=(((tmpvol-193) / 10)*10)+193;
	}

	if (leftvol<65) {

		tmpvol=leftvol+volume;
		if (tmpvol>64) tmpvol=64;
		if (volume_pan_mode) tmpvol+=128;

	} else {

		tmpvol=leftvol+volume;
	}

	CURRENT_NOTE_SET.volume=tmpvol;
	last_volume=tmpvol;

	cursor_move_down();
	cursor_move_field_left();
	
}


void Editor::insert_command_at_cursor(char command) {

	if (IS_LETTER(command)) { // A-Z command?

        	CURRENT_NOTE_SET.command=(command-'A');

	} else { // 0-9 command?
	
		CURRENT_NOTE_SET.command=26+(command-'0');
	}

	last_command=CURRENT_NOTE_GET.command;
	last_parameter=CURRENT_NOTE_GET.parameter;

	cursor_move_down();

}

void Editor::insert_parameter_at_cursor_field1(Uint8 parameter) {

	CURRENT_NOTE_SET.parameter=parameter*0x10+CURRENT_NOTE_GET.parameter % 0x10;
	last_command=CURRENT_NOTE_GET.command;
	last_parameter=CURRENT_NOTE_GET.parameter;

	cursor_move_field_right();
}

void Editor::insert_parameter_at_cursor_field2(Uint8 parameter) {

	CURRENT_NOTE_SET.parameter=parameter+CURRENT_NOTE_GET.parameter/0x10*0x10;
	last_command=CURRENT_NOTE_GET.command;
	last_parameter=CURRENT_NOTE_GET.parameter;

	cursor_move_down();
	cursor_move_field_left();
}


bool Editor::press_key_at_cursor(int key)
{
	
	bool success;
	success=false;

	switch (cursor_field) {

		case 0: { success=insert_note_at_cursor(key); } break;
		case 1: { if IS_NUMBER(key) insert_octave_at_cursor(key-'0'); } break;
		case 2: { if IS_NUMBER(key) insert_instrument_at_cursor(key-'0', 1); } break;
		case 3: { if IS_NUMBER(key) insert_instrument_at_cursor(key-'0', 2); } break;
		case 4: { if (IS_NUMBER(key) || IS_LETTER(key)) insert_volume_at_cursor_field1(key); } break;
		case 5: { if IS_NUMBER(key) insert_volume_at_cursor_field2(key-'0'); } break;
		case 6: { if IS_LETTER(key) insert_command_at_cursor(key); } break;
		case 7: { if IS_HEX_NUMBER(key) insert_parameter_at_cursor_field1(CHAR_TO_HEX(key)); } break;
		case 8: { if IS_HEX_NUMBER(key) insert_parameter_at_cursor_field2(CHAR_TO_HEX(key)); } break;
	}

	return success;
}


void Editor::press_note_off_at_cursor() {

	if (cursor_field<2) {

		CURRENT_NOTE_SET.note=Note::OFF;
		cursor_move_down();

	} else if ((cursor_field>3) && (cursor_field<6)) {

		volume_pan_mode=!volume_pan_mode;
	}

}
void Editor::press_mask_write_cursor() {

	switch (cursor_field) {

		case 0:{

			CURRENT_NOTE_SET.note=last_note;
		} break;
		case 1: break;
		case 2:
		case 3:{

			CURRENT_NOTE_SET.instrument=last_instrument;
		} break;
		case 4:
		case 5:{

			CURRENT_NOTE_SET.volume=last_volume;
		} break;
			
		case 6:
		case 7:
		case 8:{

			CURRENT_NOTE_SET.parameter=last_parameter;
			CURRENT_NOTE_SET.command=last_command;
		} break;
	}

	cursor_move_down();
}

void Editor::get_mask_from_cursor() {

	switch (cursor_field) {

		case 0:{
			
			last_note=CURRENT_NOTE_GET.note;
		} break;
		case 1: break;
		case 2:
		case 3:{

			last_instrument=CURRENT_NOTE_GET.instrument;
		} break;
		case 4:
		case 5:{

			last_volume=CURRENT_NOTE_SET.volume;
		} break;
			
		case 6:
		case 7:
		case 8:{

			last_parameter=CURRENT_NOTE_SET.parameter;
			last_command=CURRENT_NOTE_SET.command;
		} break;
	}
}


void Editor::press_note_cut_at_cursor() {

	CURRENT_NOTE_SET.note=Note::CUT;
	cursor_move_down();
}

void Editor::perform_raise_at_cursor() {

	CURRENT_NOTE_SET.raise();

}

void Editor::perform_lower_at_cursor() {

	CURRENT_NOTE_SET.lower();
}




