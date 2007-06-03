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

#ifndef ENGINE_H
#define ENGINE_H

#include "components/audio/midi_voice.h"

/**
  *@author Juan Linietsky
  * Base class for engine
  */

class Engine : public SigC::Object {
public:

	virtual string get_name()=0; // get engine name
	/*
	  Add a bank, return the index of it
	*/
        virtual int add_bank()=0;
	
	virtual int get_bank_count()=0; //how many banks does this engine have?
	virtual string get_bank_name(int p_bank_index)=0;
	
	virtual bool bank_has_patch(int p_bank_index,int p_patch_index)=0;
	
	virtual string get_patch_name(int p_bank_index,int p_patch_index)=0;

	/* get voice! */
	virtual Midi_Voice * get_midi_voice(int p_bank, int p_pach,int p_channel)=0;
	virtual Midi_Voice * get_midi_drum_voice(int p_bank,int p_drum_bank,int p_channel,char p_instrument)=0;

	virtual void set_midi_channel_bank(int p_channel,int p_bank)=0;
	virtual void set_midi_channel_patch(int p_channel,int p_patch)=0;
		
	virtual void passive_poll();
	
	Engine();
	~Engine();
};

#endif
