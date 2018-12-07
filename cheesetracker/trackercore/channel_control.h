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

#ifndef _CHANNEL_CONTROL_H_
#define _CHANNEL_CONTROL_H_

#include "sample.h"
#include "instrument.h"
#include "components/audio/iir_svf.h"

/**
 *
 * Juan Linietsky
 **/

struct Filter_Control {

	Sint32 it_reso;
	Sint32 it_cutoff;
	Sint32 envelope_cutoff;
	Sint32 final_cutoff;
	int type;
	IIR_SVF filter;
	IIR_SVF::Coeffs coeffs;

	void recalculate_coheficients(float);

};

//tells you if a channel is doing
//noteoff/notekill/notefade/etc
enum {

	END_NOTE_NOTHING=0,
	END_NOTE_OFF=1,
	END_NOTE_FADE=2,
	END_NOTE_KILL=3
};

//Tells you what should a channel restart




enum {

	KICK_NOTHING,
	KICK_NOTE,
	KICK_NOTEOFF,
	KICK_ENVELOPE
};

struct Voice_Control;

struct Channel_Control {

	struct Layer {

		/* NOTE info */
		Uint8  note;                      /* the audible note as heard, direct rep of period */
		Uint32 period;       /* period to play the sample at */
		Uint16 aux_period;    /* temporary period */
		Uint16 slide_to_period; /* period to slide to (with effect 3 or 5) */

		Uint8 kick;

		Filter_Control filter;
		Uint16 reverb_send;
		Uint16 chorus_send;

		Sint16  output_volume; /* output volume (vol + sampcol + instvol) //volume */

		Uint16  fadeout_volume;      /* fading volume rate */

		Sint16 random_volume_variation; /* 0-100 - 100 has no effect */

		int note_end_flags;

		Sint16  panning;   /* panning position */

		Sint16  volume;       /* amiga volume (0 t/m 64) to play the sample at */
		Sint16 aux_volume;

		/* INSTRUMENT INFO */

		Instrument::Layer* layer_ptr;
		Sample*     sample_ptr;
		int sample_index;
		bool sample_changed;

		/* SAMPLE SPECIFIC INFO */
//		Uint32 base_speed;        /* what finetune to use */

		/* INSTRUMENT SPECIFIC INFO */

		Sint8 NNA_type;
		Sint8 duplicate_check_type;
		Sint8 duplicate_check_action;

		bool volume_envelope_on;
		bool panning_envelope_on;
		bool pitch_envelope_on;

		/* Slave Voice Control */

		Voice_Control *slave_voice;        /* Audio Slave of current effects control channel */



        } layers [Instrument::MAX_LAYERS];

	bool kicking;

	size_t sample_start_index;        /* The starting byte index in the sample */
	Uint8  instrument_index;
	Instrument *instrument_ptr;
	bool new_instrument;

	bool has_own_period;

	bool row_has_note;

	Uint8  real_note;                 /* the note that indexes the audible */
	Uint8  old_note;

	/* VOLUME COLUMN */

	bool has_own_volume;
	bool mute;

	/* VOLUME/PAN/PITCH MODIFIERS */

	Sint8	default_volume; // CHANNEL default volume (0-64)
	Sint16  channel_volume; // CHANNEL current volume //chanvol - current!


	/* PAN */

	Sint16  channel_panning;
	Sint8   sliding;


	/* TIMING */
	Uint8 note_delay;    /* (used for note delay) */

//	Uint8       slave_voice_index;     /* Audio Slave of current effects control channel */

	Uint8*      row;          /* row currently playing on this channel */

	/* effect memory variables */

	Uint8 current_command;
	Uint8 current_parameter;
	Uint8 current_volume_command;
	Uint8 current_volume_parameter;
	Uint8 volcol_volume_slide;

	/* Sample Offset */

	Uint32 lo_offset;
	Uint32 hi_offset;

	/* Panbrello waveform */
	Uint8       panbrello_type;     /* current panbrello waveform */
	Uint8       panbrello_position;      /* current panbrello position */
	Sint8       panbrello_speed;      /* "" speed */
	Uint8       panbrello_depth;    /* "" depth */
	Uint8	    panbrello_info;
	/* Arpegio */

	Uint8	     arpegio_info;
	/* Pattern Loop */

	int pattern_loop_position;
	Sint8 pattern_loop_count;

	/* Vibrato */
	bool doing_vibrato;
	Sint8       vibrato_position;       /* current vibrato position */
	Uint8       vibrato_speed;       /* "" speed */
	Uint8       vibrato_depth;     /* "" depth */
	Uint8	    vibrato_type;
	/* Tremor */
	Sint8       tremor_position;
	Uint8       tremor_speed;    /* s3m tremor ontime/offtime */
	Uint8	    tremor_depth;
	Uint8	    tremor_info;

	/* Tremolo */
	Sint8       tremolo_position;
	Uint8       tremolo_speed;    /* s3m tremor ontime/offtime */
	Uint8	    tremolo_depth;
	Uint8	    tremolo_info;
	Uint8	    tremolo_type;

	/* Retrig */
	Sint8       retrig_counter;  /* retrig value (0 means don't retrig) */
	Uint8       retrig_speed;  /* last used retrig speed */
	Uint8       retrig_volslide;  /* last used retrig slide */

	/* Sample Offset */
	Uint32       sample_offset_hi;     /* last used high order of sample offset */
	Uint16       sample_offset;      /* last used low order of sample-offset (effect 9) */
	Uint16       sample_offset_fine;    /* fine sample offset memory */

	/* Portamento */
	Uint8 portamento_speed;

	/* Volume Slide */

	Uint8 volume_slide_info;

	/* Channel Volume Slide */

	Uint8 channel_volume_slide_info;

	/* Global Volume Slide */

	Uint8 global_volume_slide_info;

	/* Channel Pan Slide */

	Uint8 channel_pan_slide_info;

	/* Pitch Slide */

	Uint8 pitch_slide_info;
	/* Tempo Slide */

	Uint8 tempo_slide_info;

	/* S effects memory */

	Uint8 current_S_effect;
	Uint8 current_S_data;

	/* Volume column memory */

	Uint8       volume_column_effect_mem;
	Uint8       volume_column_data_mem;

	bool override_defaults; //for external chans I guess

	void reset();

  Channel_Control();
  ~Channel_Control();
};

#endif
