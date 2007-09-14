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

#ifndef _VOICE_CONTROL_H_
#define _VOICE_CONTROL_H_

#include "tracker_voice.h"
#include "channel_control.h"
#include "instrument.h"
#include "sample.h"
#include "components/audio/mixer_base.h"
/**
 * 
 * Juan Linietsky
 **/
struct Voice_Control {


	struct Envelope_Control {

		int pos_index;
		int status;
		int value;
		bool sustain_looping;
		bool looping;
		bool terminated;
		bool active;
		bool kill;

	};

	Mixer_Base *mixer; /*outside set*/
	
	Filter_Control filter;
	bool use_filter; /* outside set control.use_filter */
	
	struct Effect_Send {

		int reverb;
		int chorus;
	} effect_send;

	struct MasterChannel {

		//bool has;
		//int index;
		int layer;
		Channel_Control* ptr;       /* index of "master" effects channel */

	} master_channel;

	struct UsedSample {

		Uint32 base_speed;        /* what finetune to use */
		int period;
		//sample start index
        	size_t offset;        /* The starting byte index in the sample */
		Sample* ptr;
		
		int index; //which samples (if applies, -1 if played from somewhere else )

	} sample;

	struct UsedInstrument {

		Sint8 NNA_type;
		Instrument::Layer* ptr;
	} layer;


	struct Volume {

		int output;       /* output volume (vol + sampcol + instvol) */
		int channel;      /* channel's "global" volume */
		int fadeout;      /* fading volume rate */
		int total;     /* total volume of channel (before global mixings) */
		int global; /*outside set*/
		Envelope_Control envelope_ctrl;
		int mixing; /*outside set*/

	} volume;


	struct Note {
	
		Uint8 current;         /* the audible note (as heard, direct rep of period) */
		int end_flags;
		Uint8 kick;         /* if true = sample has to be restarted */
	
	
	} note;
	
	struct Pitch {

		Uint16       auto_vibrato_pos;      /* autovibrato pos */
		Uint16       auto_vibrato_sweep_pos;      /* autovibrato sweep pos */
		Envelope_Control envelope_ctrl;
		
		bool linear_slides;  /*outside set*/

	} pitch;

	static Uint8 auto_vibrato_table[128];

	struct Panning {
	
		Sint16 current;      /* panning position */
		Envelope_Control envelope_ctrl;
                bool mix_stereo; /*outside set*/

	} panning;

	struct Mixing {

		bool voice_registered;
		Tracker_Voice voice;

	} mixing;

	bool first_processing;
	bool sudden_end_requested;

	void reset();


	void update_info_from_master_channel();
	void start_envelope(Envelope *p_envelope,Envelope_Control *p_envelope_ctrl);
	void process_envelope(Envelope *p_envelope,Envelope_Control *p_envelope_ctrl);


	bool init(Mixer_Base *p_mixer);
	bool process();
	void deinitialize(bool use_mutex=true);



	Voice_Control();
	~Voice_Control();
};

#endif
