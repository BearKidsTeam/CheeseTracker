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
                          midioutdevice.h  -  description
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

#ifndef MIDIOUTDEVICE_H
#define MIDIOUTDEVICE_H
#include "typedefs.h"

/**base midiout device class
  *@author Juan Sebastian Linietsky
  */

#define MAX_SUBDEVICES 50

class MidiOutDevice {
protected:

	int subdevices_count;	
	
	string name;
	string info;

	bool device_enabled;
	bool subdevice_enabled[MAX_SUBDEVICES];
public:


	bool is_device_enabled() { return device_enabled; };

	string get_device_name() { return name; };
	string get_device_info() { return info; };	

	int get_subdevices_count () { return subdevices_count; };
	virtual string get_subdevice_name(int subdevicenum) { return ("Null Output"); };

	virtual bool enable_subdevice(int subdevicenum); // true if error enabling
	virtual bool disable_subdevice(int subdevicenum); // true if error disabling
	virtual bool is_subdevice_enabled(int subdevice) { return subdevice_enabled[subdevice]; };

	//midi
	virtual void send_MIDI_noteon(int subdevice,char channel, char note, char velocity);
	virtual void send_MIDI_noteoff(int subdevice,char channel, char note, char velocity);
	virtual void send_MIDI_program(int subdevice,char channel, char patch);
	virtual void send_MIDI_control(int subdevice,char channel, char controller, char value);
	virtual void send_MIDI_pitch_bender(int subdevice,char channel, int value);
	virtual void send_MIDI_note_aftertouch(int subdevice,char channel, char note, char pressure) {} ;
	virtual void send_MIDI_channel_aftertouch(int subdevice,char channel, char pressure) {} ;
	virtual void send_MIDI_RPN(int subdevice, char channel, char lsb, char msb, char datamsb, char datalsb);
	virtual void send_MIDI_reset(int subdevice);
	//midi control
	virtual void send_MIDI_tempo(int p_bpm) {}
	virtual void send_PANIC();

	virtual void play_notify() {}; //notify device that playing started
	virtual void stop_notify() {}; //notify device that playing stopped
	virtual void device_wait(int wait_amount); // in msecs
	virtual void device_set_tick_offset(int p_offset){} //current+offset in ms, relative to current
	virtual void device_set_tick_current(int p_offset){} //absolute

	MidiOutDevice();
	virtual ~MidiOutDevice();
};


#endif
