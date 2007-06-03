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
                          player_data_control.cpp  -  description
                             -------------------
    begin                : Sun Mar 18 2001
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



void Player_Data::play_start_pattern(int p_pattern,int p_row) {

	play_start(p_pattern,-1,p_row);
}

void Player_Data::play_start_song() {

	play_start(-1,-1,-1);
}

void Player_Data::play_start_song_from_order(int p_order) {

	play_start(-1,p_order,-1);
}

void Player_Data::play_start_song_from_order_and_row(int p_order,int p_row) {

	play_start(-1,p_order,p_row);
}

void Player_Data::get_voice_status_info(vector<Player_Data::VoiceStatusInfo>& p_info) {

	if (info_lock) info_lock->grab();

	p_info=voice_status_info_copy;

	if (info_lock) info_lock->release();

}

void Player_Data::lock_player_variables() {

	if (variables_lock) variables_lock->grab();
}
void Player_Data::unlock_player_variables() {

	if (variables_lock) variables_lock->release();

}

void Player_Data::erase_voices_using_sample(Sample *p_sample) {

	for (Voice_List::iterator I=voice_list.begin();I!=voice_list.end();){

		Voice_List::iterator N; //next
		N=I;
		N++;

		if ( (*I)->sample.ptr==p_sample ) {
			(*I)->deinitialize();
			delete *I;
			voice_list.erase(I);
		}

		I=N;
	}
}

void Player_Data::play_start(int p_pattern, int p_order, int p_row) {


	if (control.play_mode!=PLAY_NOTHING) play_stop();

	if (variables_lock) variables_lock->grab();

	reset();

        if (p_pattern!=-1) {

		control.play_mode=PLAY_PATTERN;
		control.position.current_pattern=p_pattern;
		control.position.current_row=(p_row!=-1)?p_row:0;

	} else {

        	control.position.current_order=song->get_next_order((p_order==-1)?p_order:p_order-1);
		if (control.position.current_order!=-1) {

			control.play_mode=PLAY_SONG;
			control.position.current_pattern=song->get_order(control.position.current_order);
			control.position.current_row=(p_row!=-1)?p_row:0;
		}
	}

	control.reached_end=false;
	set_tick_size();
	if (variables_lock) variables_lock->release();
}


void Player_Data::play_stop() {

	int i;

	if (variables_lock) variables_lock->grab();

	control.play_mode=PLAY_NOTHING;

	clear_voice_list();

	for (i=0;i<PATTERN_WIDTH;i++) {

		control.channel[i].reset();
	}

	reset();
	if (variables_lock) variables_lock->release();
}

void Player_Data::play_note(int p_channel,Note note,bool p_override_def) {


	if (variables_lock) variables_lock->grab();

	if (control.play_mode==PLAY_NOTHING) {

		control.ticks_counter=0;
	}

	process_note(p_channel,note);
	control.channel[p_channel].override_defaults=p_override_def;

	if (variables_lock) variables_lock->release();
}
/*
int Player_Data::get_voice_envelope_pos(int p_voice,Envelope *p_envelope) {

	int i,tmp_index=-1;

	i=p_voice;


	if (variables_lock) variables_lock->grab();


	if ((song->variables.use_instruments) && (voice[i]->instrument_ptr!=NULL) && (voice[i]->fadeout_volume>0)) {

		if ((p_envelope==&voice[i]->instrument_ptr->volume.envelope) && (voice[i]->instrument_ptr->volume.envelope.on)) {

			tmp_index=voice[i]->volume_envelope_ctrl.pos_index;
		}

		if ((p_envelope==&voice[i]->instrument_ptr->panning.envelope) && (voice[i]->instrument_ptr->panning.envelope.on)) {

			tmp_index=voice[i]->panning_envelope_ctrl.pos_index;
		}

		if ((p_envelope==&voice[i]->instrument_ptr->pitch.envelope) && (voice[i]->instrument_ptr->pitch.envelope.on)) {

			tmp_index=voice[i]->pitch_envelope_ctrl.pos_index;
		}

	}

	if (variables_lock) variables_lock->release();

	return tmp_index;
}
*/
void Player_Data::goto_next_order() {


	if (control.play_mode!=PLAY_SONG) return;

	if (variables_lock) variables_lock->grab();

	control.position.current_row=0;

	control.position.current_order=song->get_next_order(control.position.current_order);
        control.position.current_pattern=song->get_order(control.position.current_order);

	if (control.position.current_order==-1) {

         	reset();
	}

	if (variables_lock) variables_lock->release();

}
void Player_Data::goto_previous_order() {

	if (control.play_mode!=PLAY_SONG) return;

	if (variables_lock) variables_lock->grab();

	int next_order,current_order;

	control.position.current_row=0;

	current_order=control.position.current_order;

	next_order=song->get_next_order(current_order);

	while ((next_order!=control.position.current_order) && (next_order!=-1)) {

		current_order=next_order;
		next_order=song->get_next_order(current_order);
	}

	if (next_order==-1) {

         	reset();	
	} else {

		control.position.current_order=current_order;
	        control.position.current_pattern=song->get_order(control.position.current_order);

	}

	if (variables_lock) variables_lock->release();

}
/*
int Player_Data::get_channel_voice(int p_channel) {

	if (control.channel[p_channel].slave_voice==NULL) return -1;
	else return control.channel[p_channel].slave_voice_index;
}

string Player_Data::get_voice_sample_name(int p_voice) {

	string name;

	if (variables_lock) variables_lock->grab();

	if (voice[p_voice]->sample_ptr!=NULL) name=voice[p_voice]->sample_ptr->name;

	if (variables_lock) variables_lock->release();

	return name;

}
string Player_Data::get_voice_instrument_name(int p_voice) {


	string name;

	if (variables_lock) variables_lock->grab();

	if (voice[p_voice]->instrument_ptr!=NULL) name=voice[p_voice]->instrument_ptr->name;

	if (variables_lock) variables_lock->release();

	return name;

}
*/
void Player_Data::set_filters_enabled(bool p_enable){

	control.filters=p_enable;
}
  /*
int Player_Data::get_voice_sample_index(int p_voice) {

	return voice[p_voice]->instrument_index;
}
    */
