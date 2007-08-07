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
                          Player_Realtime_Keyboard.cpp  -  description
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

#include "player_rt_keyboard.h"


int Player_Realtime_Keyboard::find_empty_channel()
{
	int i;

	for (i=PATTERN_WIDTH-1;i>=0;i--) {
         	if (channel_per_note[i]==-1) return i;
	}

	return 0; // let's empty channel 0
}

int Player_Realtime_Keyboard::find_note_in_channel(int p_note)
{

	int i;

	for (i=PATTERN_WIDTH-1;i>=0;i--) {
         	if (channel_per_note[i]==p_note) return i;
	}

	return -1;
}

void Player_Realtime_Keyboard::instrument_set(int p_instrument_index) {

	current_instrument=p_instrument_index;
}

void Player_Realtime_Keyboard::instrument_press_key(int p_note,int p_volume) {

	if ((p_note>=Note::NOTES) || (p_note<0)) return;
	
	int channel;

	Note note;

	note.clear();
	note.note=p_note;
        note.volume=p_volume;
	note.instrument=current_instrument;


        channel=find_empty_channel();
	
	channel_per_note[channel]=p_note;

	player->play_note(channel,note);

}

void Player_Realtime_Keyboard::instrument_stop_key(int p_note) {

	if ((p_note>=Note::NOTES) || (p_note<0)) return;

	int channel;
	Note note;

	note.clear();
	note.note=Note::OFF;

	key_pressed[p_note]=false;

	if ((channel=find_note_in_channel(p_note))!=-1) {

		channel_per_note[channel]=-1;
		player->play_note(channel,note);
	}
}

void Player_Realtime_Keyboard::instrument_stop_all() {

	int i;

	for (i=0;i<PATTERN_WIDTH;i++) {

		if (channel_per_note[i]!=-1) {

			Note note;
			note.clear();
			note.note=Note::OFF;

			player->play_note(i,note);
			channel_per_note[i]=-1;

		}
	}

       	for (i=0;i<Note::NOTES;i++) key_pressed[i]=false;
}

void Player_Realtime_Keyboard::sample_set(Sample_Data *p_sample_data) {

	if (current_sample!=NULL) player->reserved_voice_stop(0);
	current_sample=p_sample_data;
}

void Player_Realtime_Keyboard::sample_press_key(int p_note) {

	latest_key_pressed=p_note;
	if (current_sample==NULL)  {

		return;
	}

	/* URGENT
	player->reserved_voice_start_sample(0,current_sample,get_frequency(get_period((p_note+current_sample->note_offset) << 1,current_sample->finetune,song->variables.use_linear_slides),song->variables.use_linear_slides));
        */

	//player->reserved_voice_start_sample(0,current_sample,freq);

	////printf("C-5 Constant is %i\n",Tables::get_linear_frequency(Tables::get_linear_period(60<<1,0)));

	mixing.voice.set_frequency((Uint32)freq);


//	player->reserved_voice_start_sample(0,current_sample,player->get_frequency(get_period((p_note+current_sample->note_offset) << 1,current_sample->finetune,song->variables.use_linear_slides)));

}

void Player_Realtime_Keyboard::sample_adjust_latest_key() {


	if (latest_key_pressed==-1) return;

	/* URGENT
	player->reserved_voice_set_frequency(0,get_frequency(get_period((latest_key_pressed+current_sample->note_offset) << 1,current_sample->finetune,song->variables.use_linear_slides),song->variables.use_linear_slides));
	*/

}

void Player_Realtime_Keyboard::sample_stop_key(int p_note) {

	key_pressed[p_note]=false;

	if (latest_key_pressed!=p_note) return;

/* Commented for IT behavior	
	if (current_sample!=NULL) player->reserved_voice_stop(0);
*/

}

void Player_Realtime_Keyboard::sample_stop_all() {

	int i;

	if (current_sample!=NULL) player->reserved_voice_stop(0);

       	for (i=0;i<Note::NOTES;i++) key_pressed[i]=false;

        latest_key_pressed=-1;
}

void Player_Realtime_Keyboard::sample_set_multichannel(bool p_multichannel) {


}

bool Player_Realtime_Keyboard::is_multichannel_set() {

	return multichannel_set;

}

void Player_Realtime_Keyboard::link_to_player(Player_Data *p_player) {

	player=p_player;
	player->set_reserved_voices(1);

}

Player_Realtime_Keyboard::Player_Realtime_Keyboard(){
	
	int i;

	for (i=0;i<PATTERN_WIDTH;i++) channel_per_note[i]=-1;
	for (i=0;i<Note::NOTES;i++) key_pressed[i]=false;
	current_sample=NULL;
	latest_key_pressed=-1;
	song=NULL;
}
Player_Realtime_Keyboard::~Player_Realtime_Keyboard(){
}
