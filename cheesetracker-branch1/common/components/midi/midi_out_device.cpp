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
                          midioutdevice.cpp  -  description
                             -------------------
    begin                : Tue Oct 17 2000
    copyright            : (C) 2000 by Juan Sebastian Linietsky
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

#include "midi_out_device.h"

MidiOutDevice::MidiOutDevice(){
        int i;
	name="Null Midi Device";
	info="This is a Null MIDI device\n Everything you send to it wont be processed\n";
	subdevices_count=1;
	for(i=0;i<MAX_SUBDEVICES;i++) subdevice_enabled[i]=false;
	subdevice_enabled[0]=true;
}

bool MidiOutDevice::enable_subdevice(int subdevicenum) { // true if error enabling

        subdevice_enabled[subdevicenum]=true;
	return false;
}
bool MidiOutDevice::disable_subdevice(int subdevicenum) { // true if error disabling

        subdevice_enabled[subdevicenum]=false;
	return false;
}
void MidiOutDevice::send_MIDI_noteon(int subdevice,char channel, char note, char velocity) {

}
void MidiOutDevice::send_MIDI_noteoff(int subdevice,char channel, char note, char velocity) {

}
void MidiOutDevice::send_MIDI_program(int subdevice,char channel, char patch) {

}
void MidiOutDevice::send_MIDI_control(int subdevice,char channel, char controller, char value) {

}
void MidiOutDevice::send_MIDI_pitch_bender(int subdevice,char channel, int value) {

}
void MidiOutDevice::send_MIDI_RPN(int subdevice, char channel, char lsb, char msb, char datamsb, char datalsb) {

}
void MidiOutDevice::send_MIDI_reset(int subdevice) {

}

void MidiOutDevice::send_PANIC() {

	int i,j;

	for (i=0;i<subdevices_count;i++) {

		for(j=0;j<16;j++) {

			send_MIDI_control(i,j,120,0);
			send_MIDI_control(i,j,121,0);
			send_MIDI_control(i,j,123,0);
		}
		send_MIDI_reset(i);
       }

}


void MidiOutDevice::device_wait(int wait_amount) {

}

MidiOutDevice::~MidiOutDevice(){
}
