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
                          midioutdevicemanager.h  -  description
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

#ifndef MIDIOUTDEVICEMANAGER_H
#define MIDIOUTDEVICEMANAGER_H
#include "midi_out_device.h"
#include <vector>
	using std::vector;

/**Manages MIDIOUT devices
  *@author Juan Sebastian Linietsky
  */
#define MAX_MIDIOUT_DEVICES 50

class MidiOutDeviceManager {
private:

	struct Subdevice_Pairs {
		
		int device;
		int subdevice;
	};	

	vector<Subdevice_Pairs> subdev_vector;
	
	void regenerate_subdev_vector();
	
	MidiOutDevice *device_array[MAX_MIDIOUT_DEVICES];
	int real_devices_count;

	int real_device(unsigned devicenum);
	int real_subdevice(unsigned devicenum);
public:

	string GetDeviceName(int devicenum);
	
	string GetParentDeviceName(int devicenum);
	string GetParentDeviceInfo(int devicenum);
	
	int GetDevicesCount();
	void AddDevice(MidiOutDevice *newdevice);
	bool is_device_enabled(int devicenum) { return device_array[real_device(devicenum)]->is_subdevice_enabled(real_subdevice(devicenum)); };
	bool enable_device(int devicenum) { return device_array[real_device(devicenum)]->enable_subdevice(real_subdevice(devicenum)); };
	void disable_device(int devicenum) { device_array[real_device(devicenum)]->disable_subdevice(real_subdevice(devicenum)); };
	// midi abstraction
	
	void send_MIDI_noteon(int abstract_device,char channel, char note, char velocity);
	void send_MIDI_noteoff(int abstract_device,char channel, char note, char velocity);
	void send_MIDI_program(int abstract_device,char channel, char patch);
	void send_MIDI_control(int abstract_device,char channel, char controller, char value);
	void send_MIDI_pitch_bender(int abstract_device,char channel, int value);
	void send_MIDI_note_aftertouch(int abstract_device,char channel, char note, char pressure);
	void send_MIDI_channel_aftertouch(int abstract_device,char channel, char pressure);
	void send_MIDI_RPN(int abstract_device, char channel, char lsb, char msb, char datamsb, char datalsb);
	void send_MIDI_reset(int abstract_device);

	void send_WAIT_to_all_devices(int wait_amount); //ms to wait
	void send_tempo_to_all_devices(int p_tempo); //ms to wait
	void set_tick_offset(int p_offset); //current+offset in ms, relative to current
	void set_tick_current(int p_offset); //absolute

	void send_panic();

	void play_notify();
	void stop_notify();

	MidiOutDeviceManager();
	~MidiOutDeviceManager();
};

#endif
