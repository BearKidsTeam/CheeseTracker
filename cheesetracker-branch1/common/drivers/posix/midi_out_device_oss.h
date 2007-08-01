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
                          midioutdevice_oss.h  -  description
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

#ifndef MIDIOUTDEVICE_OSS_H
#define MIDIOUTDEVICE_OSS_H

#include "components/midi/midi_out_device.h"

#ifdef OSS_ENABLED

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/soundcard.h>

/**OSS midiout device
  *@author Juan Sebastian Linietsky
  */


#define MAX_OSS_DEVICES 50

class MidiOutDevice_OSS : public MidiOutDevice  {

	struct midi_info midi_devices_info[MAX_OSS_DEVICES];
	struct synth_info synt_devices_info[MAX_OSS_DEVICES];

	int midi_ports_found;
	int midi_synths_found;


public:

	bool enable_subdevice(int subdevicenum); // true if error enabling
	bool disable_subdevice(int subdevicenum); // true if error disabling

	string get_subdevice_name(int subdevicenum);

	//midi
	void send_MIDI_noteon(int subdevice,char channel, char note, char velocity);
	void send_MIDI_noteoff(int subdevice,char channel, char note, char velocity);
	void send_MIDI_program(int subdevice,char channel, char patch);
	void send_MIDI_control(int subdevice,char channel, char controller, char value);
	void send_MIDI_pitch_bender(int subdevice,char channel, int value);
	void send_MIDI_note_aftertouch(int subdevice,char channel, char note, char pressure);
	void send_MIDI_channel_aftertouch(int subdevice,char channel, char pressure);
	void send_MIDI_RPN(int subdevice, char channel, char lsb, char msb, char datamsb, char datalsb);
	void send_MIDI_reset(int subdevice);

	void device_wait(int wait_amount); // in msecs

	void play_notify();
	void stop_notify();

	MidiOutDevice_OSS();
	~MidiOutDevice_OSS();
};

#endif /* oss enabled */
#endif


