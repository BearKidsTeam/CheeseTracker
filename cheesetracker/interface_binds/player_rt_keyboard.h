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
                          player_rt_keyboard.h  -  description
                             -------------------
    begin                : Sun Apr 1 2001
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

#ifndef PLAYER_RT_KEYBOARD_H
#define PLAYER_RT_KEYBOARD_H

#include "player_data.h"

#ifndef NO_MUTEX
# define NO_MUTEX false
#endif

/**realtime keyboard for testing
stuff
  *@author Juan Linietsky
  */

/******************************
 player_rt_keyboard.h
 ------------------------

this is cool because, besides being cool,
allows you to abstract that keyboard-playing
note-testing things of IT.
********************************/

class Player_Realtime_Keyboard {

	bool key_pressed[Note::NOTES];
	int channel_per_note[PATTERN_WIDTH];

	Player_Data *player;
	Mixer_Base *mixer;

        int find_note_in_channel(int p_note);
	int find_empty_channel();

	int current_instrument;
        Sample_Data *current_sample;
	bool multichannel_set;
        int latest_key_pressed;
	Tracker_Voice voice;

	bool sample_playing;
public:

	void instrument_set(int p_instrument_index);
	void instrument_press_key(int p_note, int p_volume);
	void instrument_stop_key(int p_note);
	void instrument_stop_all();

	void sample_set(Sample_Data *p_sample_data);
	Sample_Data* sample_get();
	int sample_pos_get();
	void sample_press_key(int p_note);
	void sample_stop_key(int p_note);
	void sample_stop_all(bool use_mutex=true);
	void sample_set_multichannel(bool p_multichannel);
	bool is_multichannel_set();
        void sample_adjust_latest_key();

	void link_to_player(Player_Data *p_player);
	void link_to_mixer(Mixer_Base *p_mixer);

	Player_Realtime_Keyboard();
	~Player_Realtime_Keyboard();
};

#endif
