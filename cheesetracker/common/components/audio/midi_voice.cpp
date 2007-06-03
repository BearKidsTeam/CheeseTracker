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
                          midi_voice.cpp  -  description
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

#include "midi_voice.h"


bool Midi_Voice::in_use() {

	return used;
}


	

void Midi_Voice::set_in_use(bool p_in_use) {

	used=p_in_use;
}


void Midi_Voice::set_sustain(bool p_sustain) {

	if (!p_sustain) {
	
		if (sustain)
			set_note_off(127);
	}		
	
	sustain=p_sustain;

}

bool Midi_Voice::is_sustaining() {

	return sustain;
}

MIDI_Controls* Midi_Voice::get_controllers() {
	
	return controllers;	
}


void Midi_Voice::set_controllers(MIDI_Controls *p_controllers) {
	
	controllers=p_controllers;	
}


int Midi_Voice::get_channel() {
	
	return channel;
}

void Midi_Voice::set_channel(int p_channel) {
	
	channel=p_channel;
}

Midi_Voice::Midi_Voice() {

	sustain=false;
	used=false;
	controllers=NULL;
	channel=-1;

}
