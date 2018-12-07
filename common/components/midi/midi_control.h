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
                          midi_event.h  -  description
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

#ifndef MIDI_CONTROL_H
#define MIDI_CONTROL_H


#include "components/audio/midi_voice.h"
#include "components/audio/mixer_base.h"
//#include "device_manager.h"
#include "components/midi/midi_control_base.h"
#include <vector>
	using std::vector;
	
#include "prealloc_list.h"


#include "components/midi/engine_manager.h"
/**
  *@author Juan Linietsky
  */

#define PAN_LEFT 0
#define PAN_RIGHT 127
#define PAN_CENTER 64


class Midi_Control : public Midi_Control_Base {

	Mixer_Base *mixer;
	Engine_Manager *engine_manager;


	enum {

		MAX_VOICES_PER_CHANNEL=60
	};

	struct Channel {

		int patch;
		int bank; //set using MIDI BANK LSB
		int engine; //set using MIDI BANK MSB

		
		typedef PreallocList<Midi_Voice*> Voice_List;
		Voice_List voice_list;
	
                MIDI_Controls controls;

		Channel() : voice_list(MAX_VOICES_PER_CHANNEL) {

			patch=0;
			bank=0;
			engine=0;
		};
	};


        int drums_channel;

	Channel channel[16]; //hardcoded 16 for now, this will change!

	void kill_chan_notes(char p_chan);

	int event_count;

  	void check_registered_parameter_number(int p_chan);
	
	void update_patch_data_to_engine(int p_chan);
	

	//this is intenral now, just call poll_events()	
	void note_on(char p_chan,char p_note,char p_velocity);
	void note_off(char p_chan,char p_note,char p_velocity);
	void controller(char p_chan,char p_ctrl,char p_param);
	void program_change(char p_chan,char p_program);
 	void pitch_bend(char p_chan,int p_val);
	
	bool check_dead_voice(int p_chan,Channel::Voice_List::Iterator p_I);
	void check_dead_voices(int p_chan);
	Midi_Voice* find_voice(char p_chan,char p_note);
public:

	void set_drums_channel(int p_chan) { drums_channel=p_chan; }
	int get_drums_channel() { return drums_channel; }

	int get_chan_bank(int p_chan);
	int get_chan_patch(int p_chan);

        int get_event_count() { return event_count; }


	void poll_events(); //poll events, TODO add timestamp as PARAMETER
	
	void set_mixer(Mixer_Base *p_mixer);
	void set_engine_manager(Engine_Manager *p_engine_manager);
	
	

	Midi_Control();
	~Midi_Control();
};

#endif
