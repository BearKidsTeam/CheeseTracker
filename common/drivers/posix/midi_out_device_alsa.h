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



#ifndef MIDIOUTDEVICE_ALSA_H
#define MIDIOUTDEVICE_ALSA_H

#ifdef ALSA_ENABLED

#include <alsa/asoundlib.h>
#include "components/midi/midi_out_device.h"
#include "typedefs.h"
#include <vector>
	using std::vector;




class MidiOutDevice_ALSA : public MidiOutDevice  {

	enum {

		TICKS_PER_QUARTER = 128,
	};

        struct Tick {

        	int current;
         	int offset;
	} tick;
	
	struct seq_context {

        	snd_seq_t *handle; /* The snd_seq handle to /dev/snd/seq */
	        int queue; /* The queue to use for all operations */
	        snd_seq_addr_t  address; /* Source for events */
	        int port_count;               /* Ports allocated */
       	};

        struct  AlsaDevice {

		snd_seq_addr_t address;
  		string name;		
        };

	vector<AlsaDevice> alsa_devices;

        seq_context ctxp;
       	int queue;

        void setup_event(snd_seq_event_t *p_event,int p_devindex);

        	


public:
        int get_subdevice_by_name(string p_name);
	
	string get_subdevice_name(int subdevicenum);

	bool enable_subdevice(int subdevicenum); // true if error enabling
	bool disable_subdevice(int subdevicenum); // true if error disabling
	bool is_subdevice_enabled(int subdevice);

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
	void send_PANIC();

	void send_MIDI_tempo(int p_bpm);
        void device_wait(int wait_amount);
	
	void play_notify(); //notify device that playing started
	void stop_notify(); //notify device that playing stopped
	
	void device_set_tick_offset(int p_offset); //current+offset in ms, relative to current
	void device_set_tick_current(int p_offset); //absolute

 	MidiOutDevice_ALSA();
 	~MidiOutDevice_ALSA();
};

#endif

#endif


