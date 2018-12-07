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
                          midi_event.cpp  -  description
                             -------------------
    begin                : Mon Feb 4 2002
    copyright            : (C) 2002 by Juan Linietsky
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

#include "midi_control.h"
#include "components/midi/midi_event.h"
#include "components/midi/midi_client_manager.h"

/*

	Stuff to do in here!

	-Replace check dead voices for a signal, which will be cleaner and way more efficient
	-move note check to midi voice
	-make the proper friends for each part of voice and midi voice? status and all that..
	-bank management as it is now _SUCKS_. I need to make sure HSB selects engine and LSB selects bank
		
*/

//#define DEBUG_EVENTS

/* NOTE ON HANDLER */
void Midi_Control::note_on(char p_chan,char p_note,char p_velocity) {

        //printf("note -- banks %i\n",engine_manager->get_bank_count());  
        event_count++; //increase number of processed events
	
	check_dead_voices(p_chan);
	/* Data to compute new note */
	    
        Midi_Voice *new_voice=NULL;  //this is going to point to the new voice
	Engine * engine=engine_manager->get_engine(channel[p_chan].engine);
	
	if (engine==NULL)
		return;  //invalid engine

        if (p_chan==drums_channel) { //If this channel is a drums channel, get the midi note configured voice

		
		
		new_voice = engine->get_midi_drum_voice(channel[p_chan].bank,channel[p_chan].patch,p_chan,p_note);
	
        } else { //otherwise, get a normal voice

		new_voice = engine->get_midi_voice(channel[p_chan].bank,channel[p_chan].patch,p_chan);
	       
	}

        if (new_voice == NULL) { //Couldnt get a voice it seems
#ifdef DEBUG_EVENTS
	        printf("no voice here for chan %i, patch %i, note %i,\n",p_chan,channel[p_chan].patch,p_note);  
#endif
        	return; //thus nothing to do
	}
	
	
	new_voice->set_channel(p_chan); //the mixer needs to know the channel, or more like.. the buffer to mix to.

	
	if (channel[p_chan].voice_list.push_back(new_voice)==channel[p_chan].voice_list.end()) {
#ifdef DEBUG_EVENTS
		ERROR("Cant allocate more voices for channel!");
#endif
		return;
	}
	
//	printf("Pushed back, have %i in here\n",channel[p_chan].voice_list.size());
	
  	mixer->allocate_voice(new_voice);

		

	/* IMPORTANT: registering the voice in the mixer must be done first */
	/* configuring it should be later, this way the mixer can set on it */
	/* parameters such as mixing frequency and such */

	//set the controllers	
	new_voice->set_controllers(&channel[p_chan].controls);
	//set a new note, p_note is ignored on drum channels.
	new_voice->set_note_on(p_note,p_velocity);

        /* TODO: set RPN/NRPN */
	
	new_voice->set_in_use(true);
	new_voice->clear_status();

}

Midi_Voice* Midi_Control::find_voice(char p_chan,char p_note) {


	
	check_dead_voices(p_chan);			

	Channel::Voice_List::Iterator I=channel[p_chan].voice_list.begin();
	Channel::Voice_List::Iterator E=channel[p_chan].voice_list.end();
		
	for (;I!=E;++I) { //iterate thru voices

		if ((*I)->is_off())
			continue;
		if ((*I)->get_note()==p_note)
			return (*I);
	}
	
	return NULL;
}

/* NOTE OFF HANDLER */
void Midi_Control::note_off(char p_chan,char p_note,char p_velocity) {

	event_count++;

	Midi_Voice *voice=find_voice(p_chan,p_note);

        /* Let's see if we have this note */
	if (voice==NULL) {

		/* If we dont print a warning, although
		this behavior is "legal", it's good for debugging
		*/
#ifdef DEBUG_EVENTS
		printf("Warning: I dont have note %i in channel %i for noteoff\n",p_note,p_chan); //good old warning
#endif
		return;
	}

	if (voice->is_sustaining()) //sustaining, dont TOUCH this
		return;
	//set note off
	voice->set_note_off(p_velocity);

}



/* CC HANDLER */
void Midi_Control::controller(char p_chan,char p_ctrl,char p_param) {

	event_count++;

	switch (p_ctrl) { 

		/* We first try to capture the non-voice specific controllers */
		/* CC commands */
			
		case 32: { // Set Bank

  			channel[p_chan].bank=p_param;
			update_patch_data_to_engine(p_chan);
			
  		} break;
		case 120: //kill/mute/wathever all notes
		case 121:
		case 123: {

			kill_chan_notes(p_chan);
		} break;
		
                /* Unhandled, just set the controller */
		
		default: { //now if we dont handle it, lets give it to the voices!
			
			channel[p_chan].controls.set_control(p_ctrl,p_param);
		}
	}
}

/* MIDI PITCH BEND CONTROL */
void Midi_Control::pitch_bend(char p_chan,int p_val) {

	channel[p_chan].controls.set_pitch_bend(p_val);
	
}


void Midi_Control::update_patch_data_to_engine(int p_chan) {
	
	Engine * engine=engine_manager->get_engine(channel[p_chan].engine);
	if (engine==NULL)
		return;
	engine->set_midi_channel_bank(p_chan,channel[p_chan].bank);
	engine->set_midi_channel_patch(p_chan,channel[p_chan].patch);
}


/* MIDI PROGRAM CHANGE HANDLER */
void Midi_Control::program_change(char p_chan,char p_program) {

	/*dont really bother if changing to the same */
	if (channel[p_chan].patch==p_program) return;

	/* As seen on most devices, this mutes the current channel */
	kill_chan_notes(p_chan);
	channel[p_chan].controls.reset();
        channel[p_chan].patch=p_program;
	    
	update_patch_data_to_engine(p_chan);

}

void Midi_Control::check_dead_voices(int p_chan) {

	Channel::Voice_List::Iterator N,I=channel[p_chan].voice_list.begin();
	Channel::Voice_List::Iterator E=channel[p_chan].voice_list.end();
	
	for (;I!=E;) { //iterate thru voices

		N=I;
		++N;
		check_dead_voice(p_chan,I);
		I=N;
	}

}

bool Midi_Control::check_dead_voice(int p_chan,Channel::Voice_List::Iterator p_I) {

	if (!(*p_I)->was_removed_from_mixer())
		return false; //not dead
		
//	printf("Eliminating %i\n",(*p_I)->get_note());
	(*p_I)->set_in_use(false); //not needed anymore
	channel[p_chan].voice_list.erase(p_I);
	
	return true;

}


/* Handler that silences all the channel voices */
void Midi_Control::kill_chan_notes(char p_chan) {


	check_dead_voices(p_chan);			

	Channel::Voice_List::Iterator I=channel[p_chan].voice_list.begin();
	Channel::Voice_List::Iterator E=channel[p_chan].voice_list.end();

		
	for (;I!=E;++I) { //iterate thru voices

       		mixer->eliminate_voice(*I);

	}
	check_dead_voices(p_chan);			
	

	channel[p_chan].controls.reset();
}

/* Some info retrieving methods */
int Midi_Control::get_chan_bank(int p_chan) {

	return channel[p_chan].bank;
}

int Midi_Control::get_chan_patch(int p_chan) {

	return channel[p_chan].patch;
}


void Midi_Control::poll_events() { 


	MIDI_Event event;
	MIDI_Client_Manager *mcm = MIDI_Client_Manager::get_singleton_instance();
	while (mcm->pop_midi_event(&event)) {
	
		switch (event.type) {
		
			case MIDI_Event::NOTE_ON: {		
		
				if (event.data.note.velocity>0)
					note_on(event.channel,event.data.note.note,event.data.note.velocity);
				else
					note_off(event.channel,event.data.note.note,event.data.note.velocity); // FUCKING YAMAHA
				
			} break;
			case MIDI_Event::NOTE_OFF: {		
				note_off(event.channel,event.data.note.note,event.data.note.velocity);
		
			} break;
			case MIDI_Event::CONTROLLER: {		
				controller(event.channel,event.data.control.index,event.data.control.parameter);
		
			} break;
			case MIDI_Event::PATCH_SELECT: {		

				program_change(event.channel,event.data.patch.index);
					
			} break;
			case MIDI_Event::PITCH_BEND: {		
				
				pitch_bend(event.channel,event.data.pitch_bend.bend);
			} break;
			default: {
			
				//nothing special here			
			} break;
	
		}
	}
}

void Midi_Control::set_mixer(Mixer_Base *p_mixer) {

	mixer=p_mixer;
	for (int i=0;i<16;i++)
		mixer->set_channel_midi_controls(i,&channel[i].controls);
		
}


void Midi_Control::set_engine_manager(Engine_Manager *p_engine_manager) {

	engine_manager=p_engine_manager;

}


Midi_Control::Midi_Control(){

	event_count=0;
	drums_channel=9; //This is channel 10 in your sequencer
}

Midi_Control::~Midi_Control(){

}
