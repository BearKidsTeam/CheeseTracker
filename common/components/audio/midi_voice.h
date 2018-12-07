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
                          midi_voice.h  -  description
                             -------------------
    begin                : vie nov 1 2002
    copyright            : (C) 2002 by Juan Linietsky
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

#ifndef MIDI_VOICE_H
#define MIDI_VOICE_H

#include "voice.h"
#include "components/midi/midi_controls.h"

/**
  *@author Juan Linietsky
  */
  
  
class Midi_Control;

class Midi_Voice : public Voice  {

	bool sustain;
	bool used;
	int channel;
	MIDI_Controls *controllers;
public:

	virtual void set_sustain(bool p_sustain);
	virtual bool is_sustaining();
	
	
	virtual bool is_off()=0; //note off triggered, probably decaying	

	virtual void set_note_on(char p_note,char p_volume)=0;
	virtual void set_note_off(char p_volume)=0;

	virtual void set_note_aftertouch(char p_aftertouch)=0;
	
	virtual char get_note()=0; //get note being played
	int get_channel();	
	MIDI_Controls *get_controllers();
	
	bool in_use();
	
	Midi_Voice();
			
friend class Midi_Control; //functions that only midi control has access to

	void set_channel(int p_channel);
	void set_in_use(bool p_in_use);
	void set_controllers(MIDI_Controls *p_controllers);
	

};

#endif
