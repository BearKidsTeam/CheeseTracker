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
// Author: Juan Linietsky <coding@reduz.com.ar>", (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "tracker_instance.h"
#include "components/data/keyboard_input.h"

const char* key_captions[KEY_MAX] = {

 	"Note Cut",
	"Note Off",
	"Erase Field",
	"Next Instrument",
	"Prev Instrument",
	"Play Note On Cursor",
	"Play Row On Cursor",
	"Toggle Mask",
	"C-1",
	"C#1",
	"D-1",
	"D#1",
	"E-1",
	"F-1",
	"F#1",
	"G-1",
	"G#1",
	"A-1",
	"A#1",
	"B-1",
	"C-2",
	"C#2",
	"D-2",
	"D#2",
	"E-2",
	"F-2",
	"F#2",
	"G-2",
	"G#2",
	"A-2",
	"A#2",
	"B-2",
	"C-3",
	"C#3",
	"D-3",
	"D#3",
	"E-3"
};

const char key_defaults[KEY_MAX] = {

	'1',
        '`',
	'.',
	']',
	'[',
	'4',
	'8',
	',',
	'z',
	's',
	'x',
	'd',
	'c',
	'v',
	'g',
	'b',
	'h',
	'n',
	'j',
	'm',

	'q',
	'2',
	'w',
	'3',
	'e',
	'r',
	'5',
	't',
	'6',
	'y',
	'7',
	'u',
	'i',
	'9',
	'o',
	'0',
	'p'
};

static const char* key_names[KEY_MAX] = {

 	"KEY_NOTE_CUT",
	"KEY_NOTE_OFF",
	"KEY_ERASE_FIELD",
	"KEY_NEXT_INSTRUMENT",
	"KEY_PREV_INSTRUMENT",
	"KEY_PLAY_NOTE_ON_CURSOR",
	"KEY_PLAY_ROW_ON_CURSOR",
	"KEY_TOGGLE_MASK",
	"KEY_C_1",
	"KEY_Cs1",
	"KEY_D_1",
	"KEY_Ds1",
	"KEY_E_1",
	"KEY_F_1",
	"KEY_Fs1",
	"KEY_G_1",
	"KEY_Gs1",
	"KEY_A_1",
	"KEY_As1",
	"KEY_B_1",
	"KEY_C_2",
	"KEY_Cs2",
	"KEY_D_2",
	"KEY_Ds2",
	"KEY_E_2",
	"KEY_F_2",
	"KEY_Fs2",
	"KEY_G_2",
	"KEY_Gs2",
	"KEY_A_2",
	"KEY_As2",
	"KEY_B_2",
	"KEY_C_3",
	"KEY_Cs3",
	"KEY_D_3",
	"KEY_Ds3",
	"KEY_E_3"
 };


void Tracker_Instance::init_keyboard() {

	Keyboard_Input *kinput = Keyboard_Input::get_singleton_instance();
	for (int i=0;i<KEY_MAX;i++) {

		kinput->add_key_bind(key_names[i],key_captions[i],key_defaults[i]);
	}

}

Tracker_Instance::Tracker_Instance()
{

	format_manager.link_to_song(&song);
	loader_ct.set_mixer(&mixer);
#ifdef HAVE_LIBAUDIOFILE
	format_manager.register_loader(&loader_libaudiofile);
#endif
	format_manager.register_loader(&loader_ct);
	format_manager.register_loader(&loader_it);
	format_manager.register_loader(&loader_xm);
	format_manager.register_loader(&loader_s3m);
#ifndef HAVE_LIBAUDIOFILE
	format_manager.register_loader(&loader_wav);
#endif
	format_manager.register_loader(&loader_raw);

	saver_ct.set_mixer(&mixer);
	format_manager.register_saver(&saver_ct);
	format_manager.register_saver(&saver_it);
	format_manager.register_saver(&saver_wav);
	format_manager.register_saver(&saver_raw);

	player.link_to_song(&song);
	player.link_to_mixer(&mixer);

	editor.link_to_song(&song);

	rt_keyboard.link_to_player(&player);
	rt_keyboard.link_to_mixer(&mixer);
}

Tracker_Instance::~Tracker_Instance()
{
}
