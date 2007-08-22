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

#ifndef MIXER_BASE_H
#define MIXER_BASE_H

#include <sigc++/sigc++.h>

#include "voice.h"
#ifndef NO_MUTEX
#  define NO_MUTEX false
#endif

/* BASE MIXER CLASS, TO ABSTRACT INFO/DRIVER POLLING AND SPEED UP COMPILATION :) */

/* Inherits from sigc, to save some time */
/* sigc support will need to be dropped soon */
/* to make this more compatible */


class MIDI_Controls;


class Mixer_Base : public SigC::Object {


public:

	enum DefaultEffectNumber {
	
		DEFAULT_EFFECT_REVERB,
		DEFAULT_EFFECT_CHORUS,
		DEFAULT_EFFECT_COUNT //to know where to start from in custom ones
	};

	/**** MIXER CONTROL FUNCTIONS ****/

	/*
	   This asks the mixer to return a pointer
	   to the mixdown buffer (global buffer).
	   This buffer has the sumatory of all the
	   processed audio and it is used to
	   output to stereo/mono single outputs.
	*/
	
        virtual sample_t *get_mixdown_buffer_ptr()=0;

	/*
	   This asks the mixer to return a pointer
	   to a channel buffer. Notice this is
	   the channel buffer, raw and without
	   effect processing. It is good when
	   you want to do effect processing yourself
	   with an external library,  or just
	   multitrack recording. It only works with
	   output drivers that support registering
	   many output channels (like JACK).
	*/
       
        virtual sample_t *get_channel_buffer_ptr(int p_chan)=0;

	/*
	   This is a method to poll if a channel is in use.
	   (voices mixing to it).
	   It is good to avoid the extra effort to mixdown
	   channels to external devices.
	*/
	
	
	virtual bool is_channel_in_use(int p_chan)=0;

	
	/*
	   This method is for some info retrieving. It
	   returns the amount of voices processed in the
	   latest mixdown.
	*/
	
	virtual int get_active_voices_count()=0;

	
	/*
	   Configures the mix frequency for the mixer.
	   This is vital for voices and effects.
	*/
	virtual void set_mix_frequency(float p_freq)=0;
	virtual float get_mix_frequency()=0;

	
	/*
	   Set preamp. Voices will mix to the main buffers
	   by using this ratio. Preamp is good to increase
	   the volume of the mixer (in case a few voices are
	   used), or to decrease it (in case many are used).
	*/
        virtual void set_preamp(float p_preamp)=0;


        /*
           Write n samples (which you can later get from
           mixdown or individual channels). Return
           the actual amount of samples written
           (in case we reach to the internal buffer limit) */
        

	virtual int write_samples(int p_sample_count)=0; //asks the mixer to mix X samplers
        

	/**** VOICE_ALLOCATION FUNCTIONS ****/


	/*
	   Allocates a voice in the mixers fixed internal voice
	   pool. The mixer must give priority to new voices
	   ALLWAYS :) so if it runs out of voices, it will
	   try to kill the one with the least priority.
	*/
                                                                   
	virtual void allocate_voice(Voice* p_new_voice)=0;

	
	/*
	   Replaces a voice for a new one (eliminate/reallocate).
	   This is used when a note is pressed twice (duplicate check).
	*/
	virtual void replace_voice(Voice* p_old_voice,Voice* p_new_voice)=0;

	
	/*
	  Eliminate a voice from the mixer.
	  The mixer will try to not kill it, but fade it first,
	  to avoid nasty sound artifacts such as clicks and
	  pops.
	*/
	virtual void eliminate_voice(Voice* p_new_voice, bool use_mutex=true)=0;
	
	
	/*
	  Set the interval between processing callbacks, 0 means
	  no callbacks
	*/
	
	virtual void set_process_callback_interval(int p_interval)=0;

	virtual void set_channel_midi_controls(int p_channel,MIDI_Controls *p_controls)=0;
		
	/* 
	  This signal is emitted when the mixer processes every X samples,
	*/
	
	SigC::Signal0<void> process_callback;
	
	Mixer_Base();
	virtual ~Mixer_Base();
        
};

#endif
