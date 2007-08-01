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
                          MidiOut_Device_Win32.cpp  -  description
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

//#include "midi_out_device_win32.h" ???

#ifdef WIN32_ENABLED

MidiOut_Device_Win32::MidiOut_Device_Win32(){

        int i,count;

        subdevices_count=midiOutGetNumDevs();
        subdevice_enabled[0]=false;
}



bool MidiOut_Device_Win32::enable_subdevice(int subdevicenum) { // true if error enabling

        subdevice_enabled[subdevicenum]=!midiOutOpen(&device_handler[subdevicenum],subdevicenum,0,0, CALLBACK_NULL );

	return false;

}
bool MidiOut_Device_Win32::disable_subdevice(int subdevice) { // true if error disabling

        subdevice_enabled[subdevice]=false;
        midiOutClose(device_handler[subdevice]);

	return true;
}

string MidiOut_Device_Win32::get_subdevice_name(int subdevice) {

         
        MIDIOUTCAPS caps;
        midiOutGetDevCaps(subdevice,&caps,sizeof(MIDIOUTCAPS));

        return caps.szPname;
}

void MidiOut_Device_Win32::send_MIDI_noteon(int subdevice,char channel, char note, char velocity) {

        midiOutShortMsg(device_handler[subdevice],(velocity*0x10000) | (note*0x100) | 0x90 | channel);
}
void MidiOut_Device_Win32::send_MIDI_noteoff(int subdevice,char channel, char note, char velocity) {

        midiOutShortMsg(device_handler[subdevice],(velocity*0x10000) | (note*0x100) | 0x80 | channel);

}
void MidiOut_Device_Win32::send_MIDI_program(int subdevice,char channel, char patch) {

        midiOutShortMsg(device_handler[subdevice],(patch*0x100) | 0xC0 | channel);
}
void MidiOut_Device_Win32::send_MIDI_control(int subdevice,char channel, char controller, char value) {

        midiOutShortMsg(device_handler[subdevice],(value*0x10000) | (controller*0x100) | 0xB0 | channel);
}
void MidiOut_Device_Win32::send_MIDI_pitch_bender(int subdevice,char channel, int value) {

        midiOutShortMsg(device_handler[subdevice],((value >> 7) *0x10000) | ((value&63)*0x100) | 0xE0 | channel);
}

void MidiOut_Device_Win32::send_MIDI_note_aftertouch(int subdevice,char channel, char note, char pressure) {

        midiOutShortMsg(device_handler[subdevice],(pressure*0x100) | 0xD0 | channel);

}
void MidiOut_Device_Win32::send_MIDI_channel_aftertouch(int subdevice,char channel, char pressure) {

        midiOutShortMsg(device_handler[subdevice],(pressure*0x100) | 0xA0 | channel);
}


void MidiOut_Device_Win32::send_MIDI_RPN(int subdevice, char channel, char lsb, char msb, char datamsb, char datalsb) {

        send_MIDI_control(subdevice,channel,0x65,msb);
        send_MIDI_control(subdevice,channel,0x64,lsb);
        send_MIDI_control(subdevice,channel,0x06,datamsb);
        send_MIDI_control(subdevice,channel,0x26,datalsb);

}
void MidiOut_Device_Win32::send_MIDI_reset(int subdevice) {

        midiOutShortMsg(device_handler[subdevice],255);
}

void MidiOut_Device_Win32::play_notify() {

}

void MidiOut_Device_Win32::stop_notify() {

}


void MidiOut_Device_Win32::device_wait(int wait_amount) {

}

MidiOut_Device_Win32::~MidiOut_Device_Win32(){

}

#endif