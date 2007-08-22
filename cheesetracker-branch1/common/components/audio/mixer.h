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
                          mixer.h  -  description
                             -------------------
    begin                : Sun Jan 27 2002
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

#ifndef MIXER_H
#define MIXER_H
#define NO_MUTEX false

#include "voice.h"

#include <vector>

#include "mixer_base.h"
#include "sample_defs.h"
#include "components/audio/ring_buffer.h"
#include "components/audio/effect_chain.h"
	using std::vector;
	
#include "pvector.h"


/**
  *@author Juan Linietsky
  */


class Mixer : public Mixer_Base {
public:

	enum {
		MIXDOWN_BUFFER_INDEX=-1
	};

	enum {
		SILENCE_SHUTDOWN_THRESHOLD=3000 //shutdown channels after being silent for 3 miliseconds, will work with most stuff.
	};

private:

	enum { // i guess this is enough, should possibly be smaller

		MIX_BUFFER_SIZE=30000,
		MAX_CHANNELS=16,
		VOICE_NOT_FOUND=-1,
		DECLICK_BUFFER_SIZE=MIX_BUFFER_SIZE/30
	};


	vector<Voice*> voice;

  	vector<sample_t> mixdown_buffer;
	vector<sample_t> declick_buffer; //this makes voices not click when suddendly killed, it's a very VERY neat feature
  	vector<sample_t> thrash_buffer;


        struct Buffer {

                double idle_count; //idle time in miliseconds, when reaches SILENCE_SHUTDOWN_THRESHOLD the chain and outputs are not processed anymore

		vector<sample_t> buffer;

  		bool used_in_frame; //if it wasnt used, avoids overhead
  		bool last_used_in_frame; //if it wasnt used, avoids overhead
                string name;

		struct Output {

			int send_level; //0-127
			int send_control; //0-127
			int buffer_index; //MIXDOWN_GLOBAL_BUFFER_INDEX for global buffer
		};

		vector<Output> output_list;

		Effect_Parameter_Chain *effect_chain_parameters; //these are exported outside
		Effect_Chain *effect_chain; //and this remains here
		Effect_Chain *patch_effect_chain; //and this remains here

		MIDI_Controls *controls;
		
		Buffer();
		~Buffer();
	};



   	struct Channel : public Buffer {

		Ring_Buffer ring_buffer; //for declicker
		bool declick_buffer_used;
		

    		Channel() {

			ring_buffer.request_buffer_size(MIX_BUFFER_SIZE*2);
			declick_buffer_used=false;
   		}
    	};

  	VectorPointer<Channel> channel;
  	VectorPointer<Buffer> buffer;

   	int active_voices;
	int active_buffers;

	void on_voice_kill(int p_index);

        float preamp;

	int find_aviable_voice();

	int interval_remaining;

	void process_buffer_effects_and_outputs(Buffer* p_buffer,int p_samples);

        struct Config {

		float mix_freq;
        	int channels_used;
        	int voices_used;
		int process_callback_interval;
		float buffer_level_mute_threshold;

        } config;

	int get_voice_index(Voice* p_voice);
	void eliminate_voice(int p_index, bool use_mutex=true);

	int write_samples_internal(int p_samples);


public:

        sample_t *get_mixdown_buffer_ptr() { return &mixdown_buffer[0]; }

        sample_t *get_channel_buffer_ptr(int p_chan) { return &channel[p_chan].buffer[0]; }
	bool is_channel_in_use(int p_chan) { return channel[p_chan].last_used_in_frame; };
	int get_channel_count();
	void set_channel_name(int p_idx,string p_name);
	string get_channel_name(int p_idx);


        /* Buffer/Channel Output Control */

	int get_channel_output_count(int p_channel);

	int get_channel_output_buffer_index(int p_channel,int p_output);
	int get_channel_output_send_level(int p_channel,int p_output);
	int get_channel_output_send_control(int p_channel,int p_output);

	void remove_channel_output(int p_channel,int p_output);

	void set_channel_midi_controls(int p_channel,MIDI_Controls *p_controls);
	void set_channel_output_buffer_index(int p_channel,int p_output,int p_index);
	void set_channel_output_send_level(int p_channel,int p_output,int p_level);
	void set_channel_output_send_control(int p_channel,int p_output,int p_control);
	void set_channel_patch_effect_chain(int p_chan,Effect_Parameter_Chain *p_chain);

	void add_channel_output(int p_channel);
	Effect_Parameter_Chain * get_channel_effect_parameter_chain(int p_chan);


        void add_buffer();
	void remove_buffer(int p_idx);
	int get_buffer_count();
	void set_buffer_name(int p_idx,string p_name);
	string get_buffer_name(int p_idx);

	int get_buffer_output_count(int p_buffer);

	int get_buffer_output_buffer_index(int p_buffer,int p_output);
	int get_buffer_output_send_level(int p_buffer,int p_output);
	int get_buffer_output_send_control(int p_buffer,int p_output);

	void remove_buffer_output(int p_buffer,int p_output);

	void set_buffer_output_buffer_index(int p_buffer,int p_output,int p_index);
	void set_buffer_output_send_level(int p_buffer,int p_output,int p_level);
	void set_buffer_output_send_control(int p_buffer,int p_output,int p_control);

	void add_buffer_output(int p_buffer);

	Effect_Parameter_Chain * get_buffer_effect_parameter_chain(int p_chan);

        /* Voice Control */

	int get_active_voices_count() { return active_voices; };
	int get_active_buffers_count() { return active_buffers; };


	void allocate_voice(Voice* p_new_voice);
	void replace_voice(Voice* p_old_voice,Voice* p_new_voice);
	void eliminate_voice(Voice* p_new_voice, bool use_mutex=true);


	void set_mix_frequency(float p_freq) {config.mix_freq=p_freq;}
        float get_mix_frequency() { return config.mix_freq; };



	void set_process_callback_interval(int p_interval);
	
	void set_chorus_and_reverb_routes_preset();


        void set_preamp(float p_preamp);
	int write_samples(int p_samples);

	Mixer();
	~Mixer();
};

#endif
