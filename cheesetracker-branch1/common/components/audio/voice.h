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
                          voice.h  -  description
                             -------------------
    begin                : Fri Jul 5 2002
    copyright            : (C) 2002 by red
    email                : red@server
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VOICE_H
#define VOICE_H

#include "typedefs.h"
#include <sigc++/sigc++.h>

#include "sample_defs.h"
/**
  *@author red
  */

/* for now public libsigc, so it can notify when it's been killed */
/* this will probably be dropped soon */

class Voice : public SigC::Object {

	bool removed;
protected:
	void clear_status();
public:
	/* This emits when the voice is removed from the mixer */

	SigC::Signal0<void> on_removal;


	/* Voice Priority:
	   --------------
	   This enum defines the priority of the voice.
	   It helps the mixer in cases when it runs out of
	   free slots and it needs to allocate a new voice.
	   The mixer computes the voice to be killed according
	   to this priority.

	   Also proovided are the most common examples for how
	   priority is used. Giving higher resolution to voice
	   priorities usually ends up in misusing such ranges,
	   so a simple but descriptive method is proovided instead.
	*/

	
	enum Priority { //example priorities

		DEAD, /* The voice is DEAD (finished) - */
		VERY_LOW, /* Very Low, usually near the end of release envelope, or a low volume voice */		
		LOW,  /* Usually, a low volume voice in sustain or release*/		
		MEDIUM, /* Probbly a normal voice, in sustain mode */
		HIGH, /* A loud voice, in sustain mode, or a normal voice attack/release */
		VERY_HIGH, /* A loud voice in attack/release */
		CRITICAL /* Whathever it is, you dont want it killed */
	};

	virtual Priority get_priority()=0; //finally, this is proovided to poll priority.
	
public: /* MIXING! */

	/* Channel is not necesarily midi, it's
	   device*16+channel , since the sampler
	   should be able to proovide at least more
	   than one midiport.

	   The mixer uses this as the destination
	   buffer
	*/
	   
	virtual int get_channel()=0;

	/*
	   Channel is not necesarily midi, it's
	   device*16+channel , since the sampler
	   should be able to proovide at least more
	   than one midiport.

	   The mixer uses this as the destination
	   buffer
	*/

	/* set mix frequency (sampling conversion rate) */
	virtual void set_mix_frequency(float p_mixfreq)=0;
	
	/*
	  Set the amount of pre-amplification. In other words,
	  multiply the volume modifier for this when mixing
	*/
	virtual void set_preamp(float p_preamp)=0;

	/*
	  Asks the voice to mix into the specified buffer,
	  (it will only be called if can_mix() returns true)
	  This way the voice can add itself to the mixing
	  buffer without needing to do extra data copying.
	*/

	virtual void mix(size_t p_amount,sample_t* p_where, bool use_mutex=true)=0; //set where to mix


	virtual bool can_mix(); //return FALSE if the voice is not active, this hints the mixer to not do extra work
	void remove_from_mixer();
	
	virtual void removed_from_mixer_override();
	bool was_removed_from_mixer();

	Voice();
	virtual ~Voice();
};

#endif
