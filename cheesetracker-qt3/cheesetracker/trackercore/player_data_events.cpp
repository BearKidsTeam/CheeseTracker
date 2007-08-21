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
                          player_data_events.cpp  -  description
                             -------------------
    begin                : Fri Feb 2 2001
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
#include <stdio.h>
#include "cycles.h"

/*
	  setup_voices():

This will go throught all the REAL channels, if it finds a channel
that needs to be restarted or assigned a new VIRTUAL channel, then it
will just find one and do it.

*/



void Player_Data::setup_voices() {

	int i;


	for (i=0;i<PATTERN_WIDTH;i++) {

		if (control.channel[i].override_defaults)
			control.channel[i].mute=false;
		else
			control.channel[i].mute=song->initial_variables.channel[i].mute;

		if (control.channel[i].note_delay) continue;

		for (int j=0;j<Instrument::MAX_LAYERS;j++) {


			if (control.channel[i].layers[j].kick==KICK_NOTE) {


				//note kick? let's get a voice
				if (!control.channel[i].instrument_ptr) {

					ERROR("No instrument on channel " << i);
					continue;
				}

				Voice_Control *vcontrol = new Voice_Control;

				vcontrol->master_channel.ptr=&control.channel[i];
				vcontrol->master_channel.layer=j;
				control.channel[i].layers[j].slave_voice=vcontrol; //luckily for check purposes only, will never be used
				vcontrol->update_info_from_master_channel();
				vcontrol->init(mixer);
				vcontrol->mixing.voice.set_channel(control.channel[i].instrument_ptr->buffer.output_index);

				voice_list.push_back(vcontrol);

			};
			// check if we need a new empty voice
			// nope..

				// IMPROVE: Code a method for this:
			// voice will update
			// voice[voice_index]->update_info_from_master_channel();

			control.channel[i].layers[j].kick=KICK_NOTHING;

		}

	}


}



void Player_Data::update_voice_control_priority(Voice_Control *p_voice) {

	Voice::Priority prio;

//must use this
//	prio=p_voice->volume.total;

	//virtual channel
	if (p_voice->master_channel.ptr->layers[p_voice->master_channel.layer].slave_voice!=p_voice) {

		prio=Voice::LOW;
	} else {

		prio=Voice::MEDIUM;
	}



        p_voice->mixing.voice.set_priority(prio);
}

 void Player_Data::update_voice_control_data(Voice_Control *p_data) {

	p_data->use_filter=control.filters;
//	p_data->instrument.mode_enabled=song->variables.use_instruments;
	p_data->volume.global=control.global_volume;
//	p_data->volume.mute= TODO??? mute?
	p_data->volume.mixing=song->variables.mixing_volume;
	p_data->pitch.linear_slides=song->variables.use_linear_slides;
	p_data->panning.mix_stereo=song->variables.use_stereo;
}


void Player_Data::update_mixer() {

	int voices=0;
	for (Voice_List::iterator I=voice_list.begin();I!=voice_list.end();) {

		voices++;
		Voice_List::iterator N; //next;
		N=I;
		N++;

		update_voice_control_data( (*I) );
		update_voice_control_priority( (*I) );

		if ( (*I)->process() ) {
			//return true? process failed!
			(*I)->deinitialize();
			delete (*I);
			voice_list.erase(I);
		}
		I=N;
	}

	voice_status_info.clear();

	for (Voice_List::iterator I=voice_list.begin();I!=voice_list.end();I++) {

		VoiceStatusInfo new_info;
		new_info.sample.ptr=(*I)->sample.ptr;
		new_info.sample.index=(*I)->sample.index;
		new_info.sample.pos=(*I)->mixing.voice.get_current_sample_pos();
		new_info.instrument_layer.ptr=(*I)->layer.ptr;
		new_info.instrument_layer.vol_env_index=(*I)->volume.envelope_ctrl.pos_index;
		new_info.instrument_layer.pan_env_index=(*I)->panning.envelope_ctrl.pos_index;
		new_info.instrument_layer.frq_env_index=(*I)->pitch.envelope_ctrl.pos_index;

		new_info.voice.pan=(*I)->panning.current;
		new_info.voice.freq=(*I)->mixing.voice.get_current_freq();
		voice_status_info.push_back(new_info);
	}

        if (info_lock) info_lock->grab(__FILE__, __LINE__);

	voice_status_info_copy=voice_status_info; //copy over

        if (info_lock) info_lock->release();

	////printf("HOHO VIRTUAL VOICES %i\n",voices);

}


void Player_Data::handle_tick() {

	int i;

	if ( mixer==NULL ) return;
	if ( song==NULL ) return;

	cycles_t cyc,cycfin;

	cyc=get_cycles();
	/* update time counter (sngtime is in milliseconds (in fact 2^-10)) */

	if (control.ticks_counter>=control.speed) { // time to process... ***THE ROW***!

		/* process pattern-delay. pf->patdly2 is the counter and pf->patdly is
		   the command memory. */


		if (control.play_mode!=PLAY_NOTHING) {

			control.ticks_counter=0;

			if (control.position.force_next_order>=0) {

				control.position.current_order=control.position.force_next_order;
			}

			control.position.force_next_order=-1;

			control.previous_position=control.position; // for those special cases...
                        control.position.forbid_jump=false;

			if(control.position.current_pattern == EMPTY_FIELD) {
				goto empty_pattern;
			}

			Pattern *current_pattern =
				song->get_pattern(control.position.current_pattern);


			if(current_pattern != NULL) {

				for (i=0;i<PATTERN_WIDTH;i++) {

					Note note=current_pattern->get_note(i,control.position.current_row);
					if (note.note!=EMPTY_FIELD)
						control.channel[i].override_defaults=false;
					process_note(i,note);
				}
			}

			control.position.current_row++;

			if ( control.position.current_row>=song->get_pattern(control.position.current_pattern)->get_length() ) {

				if (control.play_mode==PLAY_SONG) {

					int next_order;

					next_order=song->get_next_order(control.position.current_order);

					if (next_order!=-1) {
						// Do we have a "next order?"
						control.position.current_pattern=song->get_order(next_order);
						if (next_order<=control.position.current_order) control.reached_end=true;
						control.position.current_order=next_order;

					} else {
						// no, probably the user deleted the orderlist.
						control.play_mode=PLAY_NOTHING;
						reset();
						control.reached_end=true;
					}
				}
				control.position.current_row=0;
			}

		}


	}
empty_pattern:

	cycfin=get_cycles()-cyc;
	////printf("1 PARSE %f cycles\n",CYCMSEC(cycfin));
	cyc=get_cycles();

	pre_process_effects();
	process_NNAs();

	cycfin=get_cycles()-cyc;
	////printf("2 FX&NNA %f cycles\n",CYCMSEC(cycfin));
	cyc=get_cycles();

	setup_voices();

	cycfin=get_cycles()-cyc;
	////printf("3 Voices %f cycles\n",CYCMSEC(cycfin));
	cyc=get_cycles();

	/* now set up the actual hardware channel playback information */
	update_mixer();

	cycfin=get_cycles()-cyc;
	////printf("4 Mixer %f cycles\n",CYCMSEC(cycfin));
	cyc=get_cycles();

	control.ticks_counter++;
	set_tick_size();


}
