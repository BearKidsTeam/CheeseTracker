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
                          midioutdevicemanager.cpp  -  description
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

#include "midioutdevicemanager.h"

int MidiOutDeviceManager::real_device(unsigned devicenum) {


        if (devicenum>=subdev_vector.size()) devicenum=subdev_vector.size();

	return subdev_vector[devicenum].device;
}

int MidiOutDeviceManager::real_subdevice(unsigned devicenum) {

        if (devicenum>=subdev_vector.size()) devicenum=subdev_vector.size();

	return subdev_vector[devicenum].subdevice;

}



void MidiOutDeviceManager::AddDevice(MidiOutDevice *newdevice) {

	if (real_devices_count<MAX_MIDIOUT_DEVICES) {

		device_array[real_devices_count]=newdevice;
		real_devices_count++;

	}

	regenerate_subdev_vector();	

}



string MidiOutDeviceManager::GetDeviceName(int devicenum) {

	return device_array[real_device(devicenum)]->get_subdevice_name(real_subdevice(devicenum));
}

string MidiOutDeviceManager::GetParentDeviceName(int devicenum) {


	return device_array[real_device(devicenum)]->get_device_name();
}

string MidiOutDeviceManager::GetParentDeviceInfo(int devicenum) {

	return device_array[real_device(devicenum)]->get_device_info();
}

int MidiOutDeviceManager::GetDevicesCount() {

        return subdev_vector.size();
}



//** MIDI **//


void MidiOutDeviceManager::send_MIDI_noteon(int abstract_device,char channel, char note, char velocity) {
	if (real_device(abstract_device)>=real_devices_count) return;
	device_array[real_device(abstract_device)]->send_MIDI_noteon(real_subdevice(abstract_device),channel,note,velocity);

}
void MidiOutDeviceManager::send_MIDI_noteoff(int abstract_device,char channel, char note, char velocity) {
	if (real_device(abstract_device)>=real_devices_count) return;

	device_array[real_device(abstract_device)]->send_MIDI_noteoff(real_subdevice(abstract_device),channel,note,velocity);
}
void MidiOutDeviceManager::send_MIDI_program(int abstract_device,char channel, char patch) {
	if (real_device(abstract_device)>=real_devices_count) return;

	device_array[real_device(abstract_device)]->send_MIDI_program(real_subdevice(abstract_device),channel,patch);
}
void MidiOutDeviceManager::send_MIDI_control(int abstract_device,char channel, char controller, char value) {
	if (real_device(abstract_device)>=real_devices_count) return;

	device_array[real_device(abstract_device)]->send_MIDI_control(real_subdevice(abstract_device),channel,controller,value);
}
void MidiOutDeviceManager::send_MIDI_pitch_bender(int abstract_device,char channel, int value) {
	if (real_device(abstract_device)>=real_devices_count) return;

	device_array[real_device(abstract_device)]->send_MIDI_pitch_bender(real_subdevice(abstract_device),channel,value);
}
void MidiOutDeviceManager::send_MIDI_note_aftertouch(int abstract_device,char channel, char note, char pressure) {
	if (real_device(abstract_device)>=real_devices_count) return;

	device_array[real_device(abstract_device)]->send_MIDI_note_aftertouch(real_subdevice(abstract_device),channel,note,pressure);
}

void MidiOutDeviceManager::send_MIDI_channel_aftertouch(int abstract_device,char channel, char pressure) {
	if (real_device(abstract_device)>=real_devices_count) return;

	device_array[real_device(abstract_device)]->send_MIDI_channel_aftertouch(real_subdevice(abstract_device),channel, pressure);
}

void MidiOutDeviceManager::send_MIDI_RPN(int abstract_device, char channel, char lsb, char msb, char datamsb, char datalsb) {
	if (real_device(abstract_device)>=real_devices_count) return;

	device_array[real_device(abstract_device)]->send_MIDI_RPN(real_subdevice(abstract_device),channel,lsb,msb,datamsb,datalsb);
}
void MidiOutDeviceManager::send_MIDI_reset(int abstract_device) {
	if (real_device(abstract_device)>=real_devices_count) return;

	device_array[real_device(abstract_device)]->send_MIDI_reset(real_subdevice(abstract_device));
}

void MidiOutDeviceManager::play_notify() {

	int i;

	for (i=0;i<real_devices_count;i++) device_array[i]->play_notify();
}

void MidiOutDeviceManager::stop_notify() {

	int i;

	for (i=0;i<real_devices_count;i++) device_array[i]->stop_notify();
}

void MidiOutDeviceManager::send_WAIT_to_all_devices(int wait_amount) {

	int i;

	for (i=0;i<real_devices_count;i++) device_array[i]->device_wait(wait_amount);
}

void MidiOutDeviceManager::send_tempo_to_all_devices(int p_tempo) {


	for (int i=0;i<real_devices_count;i++) device_array[i]->send_MIDI_tempo(p_tempo);
}

void MidiOutDeviceManager::set_tick_offset(int p_offset) {

	for (int i=0;i<real_devices_count;i++) device_array[i]->device_set_tick_offset(p_offset);
}
void MidiOutDeviceManager::set_tick_current(int p_offset) {

	for (int i=0;i<real_devices_count;i++) device_array[i]->device_set_tick_current(p_offset);

}

void MidiOutDeviceManager::send_panic() {

	int i;

	for (i=0;i<real_devices_count;i++) device_array[i]->send_PANIC();

}

	
void MidiOutDeviceManager::regenerate_subdev_vector() {

        int currentdev;


        currentdev=0;
	
	subdev_vector.clear();
	
	while ( currentdev<real_devices_count) {

		Subdevice_Pairs cur_subdev;
		int j;		
		
		for (j=0;j<device_array[currentdev]->get_subdevices_count();j++) {
		
			cur_subdev.device=currentdev;
                        cur_subdev.subdevice=j;
			subdev_vector.push_back(cur_subdev);
	
		}				
	       	currentdev++;		
	}
}



MidiOutDeviceManager::MidiOutDeviceManager(){

	real_devices_count=0;
	regenerate_subdev_vector();		
}

MidiOutDeviceManager::~MidiOutDeviceManager(){
}
