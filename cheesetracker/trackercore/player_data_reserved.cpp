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
                          player_data_reserved.cpp  -  description
                             -------------------
    begin                : Sun Apr 8 2001
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

#include "player_data.h"



void Player_Data::reserved_voice_start_sample(int p_reserved_voice,Sample_Data *p_sample,int p_freq) {
                  
	/* TODOI
	if ( (p_reserved_voice+control.song_voices>=control.max_voices) || (p_reserved_voice<0)) return;
	else p_reserved_voice+=control.song_voices;

	variables_lock->grab();

	mixer->setup_voice(p_reserved_voice,p_sample,0);
	mixer->set_voice_volume(p_reserved_voice,255);
	mixer->set_voice_panning(p_reserved_voice,PAN_CENTER);
	mixer->set_voice_frequency(p_reserved_voice,p_freq);

	variables_lock->release();
	*/


}

void Player_Data::reserved_voice_set_frequency(int p_reserved_voice,int p_freq) {

	/* TODOI 
	if ( (p_reserved_voice+control.song_voices>=control.max_voices) || (p_reserved_voice<0)) return;
	else p_reserved_voice+=control.song_voices;

	variables_lock->grab();

	mixer->set_voice_frequency(p_reserved_voice,p_freq);

	variables_lock->release();
	
	*/

}
void Player_Data::reserved_voice_set_pan(int p_reserved_voice,int p_pan) {

	
	/* TODOI 

	if ( (p_reserved_voice+control.song_voices>=control.max_voices) || (p_reserved_voice<0)) return;
	else p_reserved_voice+=control.song_voices;

	variables_lock->grab();

	mixer->set_voice_panning(p_reserved_voice,p_pan);

	variables_lock->release();

	*/
}

void Player_Data::reserved_voice_set_volume(int p_reserved_voice,int p_volume) {

	/* TODOI 

	if ( (p_reserved_voice+control.song_voices>=control.max_voices) || (p_reserved_voice<0)) return;
	else p_reserved_voice+=control.song_voices;

	variables_lock->grab();

	mixer->set_voice_volume(p_reserved_voice,(p_volume<0)?0:((p_volume>255)?255:p_volume));

	variables_lock->release();
	
	*/

}
void Player_Data::reserved_voice_stop(int p_reserved_voice) {

	/* TODOI 

	if ( (p_reserved_voice+control.song_voices>=control.max_voices) || (p_reserved_voice<0)) return;
	else p_reserved_voice+=control.song_voices;

	variables_lock->grab();

	mixer->stop_voice(p_reserved_voice);

	variables_lock->release();

	*/

}
