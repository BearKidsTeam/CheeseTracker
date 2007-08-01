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
                          player_data.cpp  -  description
                             -------------------
    begin                : Wed Jan 31 2001
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

Player_Data::Player_Data(){

	song=NULL;
	mixer=NULL;
	variables_lock=NULL;
	info_lock=NULL;
	control.song_voices=0;
	control.max_voices=0;
	control.force_no_nna=false;
	control.external_vibrato=false;
	control.filters=true;
	control.speed=5;
	control.tempo=120;
	control.play_mode=0;
	control.ticks_counter=0;
	control.pattern_delay_2=0;
	voice_status_info.reserve(500); // dont think it will use more than this
	voice_status_info_copy.reserve(500); // dont think it will use more than this
}
Player_Data::~Player_Data(){
}
  /*
void Player_Data::set_virtual_channels(int p_amount,bool p_lock = true) {

	if (p_amount<1) return;
	if (p_amount==control.max_voices) return;

	if (p_lock) variables_lock->grab();

        if (p_amount>mixer->get_total_voices_count()) p_amount=mixer->get_total_voices_count();

	int i,aux_reserved;

	aux_reserved=control.max_voices-control.song_voices;
	reset();

	for (i=0;i<control.max_voices;i++) {

		delete voice[i];
	}

	if (voice!=NULL) free(voice);
	
	(void*)voice=malloc(sizeof(Voice_Control*)*p_amount);


	for (i=0;i<p_amount;i++) voice[i] = new Voice_Control;


	control.max_voices=p_amount;

	control.song_voices=control.max_voices-aux_reserved;
	if (control.song_voices<0) control.song_voices=0;

	if (p_lock) variables_lock->release();

}

*/
void Player_Data::process_tick() {

	handle_tick();
}

void Player_Data::clear_voice_list() {

	for (Voice_List::iterator I=voice_list.begin();I!=voice_list.end();){

		Voice_List::iterator N; //next
		N=I;
		N++;

		(*I)->deinitialize();
		delete *I;
		voice_list.erase(I);
		I=N;

	}
	
	voice_list.clear();
}
void Player_Data::reset() {

	if ( mixer==NULL ) return ;
	if ( song==NULL ) return ;

	int i;

	clear_voice_list();
	
	
	for (i=0;i<PATTERN_WIDTH;i++) {

         	control.channel[i].reset();
         	control.channel[i].channel_volume=song->initial_variables.channel[i].volume;
         	control.channel[i].channel_panning=(song->initial_variables.channel[i].surround)?PAN_SURROUND:((int)song->initial_variables.channel[i].pan*255/64);
	}


	control.speed=song->initial_variables.speed;
	control.tempo=song->initial_variables.tempo;
	control.global_volume=song->initial_variables.global_volume;

	control.position.current_pattern=0;
	control.position.current_row=0;
	control.position.current_order=0;
        control.position.force_next_order=-1;
	control.ticks_counter=control.speed;
        control.position.forbid_jump=false;
	
	set_tick_size();
}


void Player_Data::set_voice_amount_limit(int p_limit) {

	control.song_voices=p_limit;
}

void Player_Data::set_reserved_voices(int p_amount) {

	set_voice_amount_limit(control.max_voices-p_amount);

}

int Player_Data::get_reserved_voices_amount() {

	return control.max_voices-control.song_voices;
	

}

void Player_Data::link_to_song(Song *p_song) {

	song=p_song;         	
	reset();
	
}

void Player_Data::set_tick_size() {

	int current_tick_size=int((mixer->get_mix_frequency()*125L)/(control.tempo*50L));
	mixer->set_process_callback_interval(current_tick_size);
}


void Player_Data::link_to_mixer(Mixer_Base *p_mixer) {

	mixer=p_mixer;
	//handle callback
	mixer->process_callback.connect(SigC::slot(*this,&Player_Data::handle_tick));
	set_tick_size();
	reset();
}

bool Player_Data::reached_end_of_song() {

	return control.reached_end;

}
void Player_Data::set_force_external_vibratos(bool p_force) {

	control.external_vibrato=p_force;
}
void Player_Data::set_force_no_nna(bool p_force) {

	control.force_no_nna=p_force;
}
