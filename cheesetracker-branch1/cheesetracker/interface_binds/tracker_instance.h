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

#ifndef TRACKER_INSTANCE_H
#define TRACKER_INSTANCE_H
#include "loaders/loader_it.h"
#include "loaders/loader_xm.h"
#include "loaders/loader_s3m.h"
#include "loaders/loader_wav.h"
#include "loaders/loader_ct.h"
#include "loaders/loader_raw.h"
#include "loaders/loader_libaudiofile.h"
#include "savers/saver_it.h"
#include "savers/saver_ct.h"
#include "savers/saver_raw.h"
#include "savers/saver_wav.h"
#include "trackercore/player_data.h"
#include "components/audio/mixer.h"
#include "trackercore/file_format_manager.h"

#include "components/audio/resampler_manager.h"
#include "plugins/resamplers/resampler_raw.h"
#include "plugins/resamplers/resampler_linear.h"

#include "interface_binds/editor.h"
#include "interface_binds/player_rt_keyboard.h"


/**
 *
 * Juan Linietsky
 **/

 enum TrackerKeys {

 	KEY_NOTE_CUT,
	KEY_NOTE_OFF,
	KEY_ERASE_FIELD,
	KEY_NEXT_INSTRUMENT,
	KEY_PREV_INSTRUMENT,
	KEY_PLAY_NOTE_ON_CURSOR,
	KEY_PLAY_ROW_ON_CURSOR,
	KEY_TOGGLE_MASK,
	KEY_C_1,
	KEY_Cs1,
	KEY_D_1,
	KEY_Ds1,
	KEY_E_1,
	KEY_F_1,
	KEY_Fs1,
	KEY_G_1,
	KEY_Gs1,
	KEY_A_1,
	KEY_As1,
	KEY_B_1,
	KEY_C_2,
	KEY_Cs2,
	KEY_D_2,
	KEY_Ds2,
	KEY_E_2,
	KEY_F_2,
	KEY_Fs2,
	KEY_G_2,
	KEY_Gs2,
	KEY_A_2,
	KEY_As2,
	KEY_B_2,
	KEY_C_3,
	KEY_Cs3,
	KEY_D_3,
	KEY_Ds3,
	KEY_E_3,
	KEY_MAX
 };

 extern const char* key_captions[KEY_MAX];
 extern const char key_defaults[KEY_MAX];

class Tracker_Instance
{


	Loader_IT loader_it;
	Loader_CT loader_ct;
	Loader_XM loader_xm;
	Loader_S3M loader_s3m;
#ifndef HAVE_LIBAUDIOFILE
	Loader_WAV loader_wav;
#endif
	Loader_RAW loader_raw;
#ifdef HAVE_LIBAUDIOFILE
	Loader_libaudiofile loader_libaudiofile;
#endif


	Saver_IT saver_it;
	Saver_CT saver_ct;
	Saver_Wav saver_wav;
	Saver_Raw saver_raw;



public:

	static void init_keyboard();

	/* Mixer and Player, one per instance */

	Mixer mixer;
	Player_Data player;

	/* Song  */
	Song song;

	/* Editor */

	Editor editor;

	/* Realtime Keyboard -try out keys and stuff- */

	Player_Realtime_Keyboard rt_keyboard;

	/* File format manager */
	File_Format_Manager format_manager;
	Tracker_Instance();
	~Tracker_Instance();
};

#endif
