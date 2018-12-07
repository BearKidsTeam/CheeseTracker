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
                          midioutdevice_oss.cpp  -  description
                             -------------------
    begin                : Sat Nov 4 2000
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

#include "midi_out_device_oss.h"
#include <stdio.h>
// Requered stuff by OSS
#ifdef OSS_ENABLED

static int seq_id; // sequencer device descriptor ID

SEQ_DEFINEBUF (4096);

void seqbuf_dump () {

	if (_seqbufptr)

		if (write(seq_id,_seqbuf,_seqbufptr)==-1) {

			ERROR("***DANGER!!***-> Cannot write to /dev/sequencer");
                }

        _seqbufptr=0;

}

//end of requered stuff by OSS


MidiOutDevice_OSS::MidiOutDevice_OSS(){

	int i;



	subdevice_enabled[0]=false;
	name="OSS Sequencer Output";
	info="This device uses /dev/sequencer output proovidedby OSS";

	seq_id = open("/dev/sequencer", O_WRONLY | O_NONBLOCK, 0);

	if (seq_id==-1) {

		ERROR("***ERROR!!*** Can't open /dev/sequencer");
		device_enabled=false;

        } else {

		/* Get amount of devices */

		ioctl(seq_id,SNDCTL_SEQ_NRSYNTHS,&midi_synths_found);
		ioctl(seq_id,SNDCTL_SEQ_NRMIDIS,&midi_ports_found);

		//cout << "Found: " << midi_ports_found << "ports and " << midi_synths_found << " synths." << endl;

		/* Get info on MIDI ports */

		for(i=0;i<midi_ports_found;i++) {

			midi_devices_info[i].device=i;

	                if (ioctl(seq_id,SNDCTL_MIDI_INFO,&midi_devices_info[i])==-1) {
		
				ERROR("Can't get info on OSS device" << i );

			} else subdevice_enabled[i]=true;

			//printf("enabling device %i\n",i);
						
		}

		device_enabled=true;
		subdevices_count=midi_ports_found;

	}
}



bool MidiOutDevice_OSS::enable_subdevice(int subdevicenum) { // true if error enabling

        subdevice_enabled[subdevicenum]=true;
	return false;

}
bool MidiOutDevice_OSS::disable_subdevice(int subdevicenum) { // true if error disabling

        subdevice_enabled[subdevicenum]=false;
	return true;
}

string MidiOutDevice_OSS::get_subdevice_name(int subdevicenum) {

	string tmpname;

	tmpname=midi_devices_info[subdevicenum].name;

	return tmpname;

}

void MidiOutDevice_OSS::send_MIDI_noteon(int subdevice,char channel, char note, char velocity) {

	SEQ_MIDIOUT(subdevice,MIDI_NOTEON | channel);
	SEQ_MIDIOUT(subdevice,note);
	SEQ_MIDIOUT(subdevice,velocity);
	SEQ_DUMPBUF();
}
void MidiOutDevice_OSS::send_MIDI_noteoff(int subdevice,char channel, char note, char velocity) {

	SEQ_MIDIOUT(subdevice,MIDI_NOTEOFF | channel);
	SEQ_MIDIOUT(subdevice,note);
	SEQ_MIDIOUT(subdevice,velocity);
	SEQ_DUMPBUF();

}
void MidiOutDevice_OSS::send_MIDI_program(int subdevice,char channel, char patch) {

	SEQ_MIDIOUT(subdevice, MIDI_PGM_CHANGE | channel);
	SEQ_MIDIOUT(subdevice, patch);
	SEQ_DUMPBUF();

}
void MidiOutDevice_OSS::send_MIDI_control(int subdevice,char channel, char controller, char value) {

	SEQ_MIDIOUT(subdevice, MIDI_CTL_CHANGE | channel);
	SEQ_MIDIOUT(subdevice, controller);
	SEQ_MIDIOUT(subdevice, value);
	SEQ_DUMPBUF();
}
void MidiOutDevice_OSS::send_MIDI_pitch_bender(int subdevice,char channel, int value) {

	char tmpbyte;

	SEQ_MIDIOUT(subdevice, MIDI_PITCH_BEND | channel);
	tmpbyte=value & 63;
	SEQ_MIDIOUT(subdevice,tmpbyte);
	tmpbyte=value >> 7;
	SEQ_MIDIOUT(subdevice,tmpbyte);
	SEQ_DUMPBUF();
}

void MidiOutDevice_OSS::send_MIDI_note_aftertouch(int subdevice,char channel, char note, char pressure) {

	SEQ_MIDIOUT(subdevice, MIDI_KEY_PRESSURE | channel);
	SEQ_MIDIOUT(subdevice, note);
	SEQ_MIDIOUT(subdevice, pressure);
	SEQ_DUMPBUF();
}
void MidiOutDevice_OSS::send_MIDI_channel_aftertouch(int subdevice,char channel, char pressure) {

	SEQ_MIDIOUT(subdevice, MIDI_CHN_PRESSURE | channel);
	SEQ_MIDIOUT(subdevice, pressure);
	SEQ_DUMPBUF();

}


void MidiOutDevice_OSS::send_MIDI_RPN(int subdevice, char channel, char lsb, char msb, char datamsb, char datalsb) {

	SEQ_MIDIOUT(subdevice, MIDI_CTL_CHANGE | channel);
	SEQ_MIDIOUT(subdevice, 0x65);
	SEQ_MIDIOUT(subdevice, msb);
	SEQ_MIDIOUT(subdevice, MIDI_CTL_CHANGE | channel);
	SEQ_MIDIOUT(subdevice, 0x64);
	SEQ_MIDIOUT(subdevice, lsb);
	SEQ_MIDIOUT(subdevice, MIDI_CTL_CHANGE | channel);
	SEQ_MIDIOUT(subdevice, 0x06);
	SEQ_MIDIOUT(subdevice, datamsb);
	SEQ_MIDIOUT(subdevice, MIDI_CTL_CHANGE | channel);
	SEQ_MIDIOUT(subdevice, 0x26);
	SEQ_MIDIOUT(subdevice, datalsb);
	SEQ_DUMPBUF();
}
void MidiOutDevice_OSS::send_MIDI_reset(int subdevice) {

	SEQ_MIDIOUT(subdevice,255); // 255 = RESET!
	SEQ_DUMPBUF();
}

void MidiOutDevice_OSS::play_notify() {


	SEQ_START_TIMER();
	SEQ_DUMPBUF();

}

void MidiOutDevice_OSS::stop_notify() {


}


void MidiOutDevice_OSS::device_wait(int wait_amount) {

	SEQ_DELTA_TIME(wait_amount/10);
	SEQ_DUMPBUF();
}

MidiOutDevice_OSS::~MidiOutDevice_OSS(){

	if (device_enabled) close(seq_id);
}
#endif

