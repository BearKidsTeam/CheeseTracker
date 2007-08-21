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
                          midi_control_base.h  -  description
                             -------------------
    begin                : jue oct 31 2002
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

#ifndef MIDI_CONTROL_BASE_H
#define MIDI_CONTROL_BASE_H


/**
  *@author Juan Linietsky
  */

#include "components/audio/mixer_base.h"

  
class Midi_Control_Base : public SigC::Object {
public:

	virtual void set_drums_channel(int p_chan)=0;
	virtual int get_drums_channel()=0;

	virtual int get_chan_bank(int p_chan)=0;
	virtual int get_chan_patch(int p_chan)=0;

        virtual int get_event_count()=0;

	virtual void note_on(char p_chan,char p_note,char p_velocity)=0;
	virtual void note_off(char p_chan,char p_note,char p_velocity)=0;
	virtual void controller(char p_chan,char p_ctrl,char p_param)=0;
	virtual void program_change(char p_chan,char p_program)=0;
 	virtual void pitch_bend(char p_chan,int p_val)=0;



	Midi_Control_Base();
	~Midi_Control_Base();
};


#endif

