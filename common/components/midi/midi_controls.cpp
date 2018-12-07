//
// C++ Implementation: midi_controls
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "midi_controls.h"


int MIDI_Controls::get_control(int p_control) {

	if (p_control<0 || p_control>=MIDI_CONTROLS_AMOUNT) {
		ERROR("Invalid CC request, returning default: " << p_control);
		return CONTROL_USE_DEFAULT_VALUE;
	}
	
	return control_array[p_control];
		
}

void MIDI_Controls::set_control(int p_control, int p_value) {
	
	if (p_control<0 || p_control>=MIDI_CONTROLS_AMOUNT) {
		ERROR("Invalid CC set: " << p_control);
		return;
	}

	if ((p_value<0 || p_value>=CONTROL_MAX_VALUE) && p_value!=CONTROL_MAX_VALUE) {
		ERROR("Invalid CC value set: " << p_value);
		return;
	}
		
	control_array[p_control]=p_value;
	
}

int MIDI_Controls::get_pitch_bend() {
	
	return pitch_bend;	
}

void MIDI_Controls::set_pitch_bend(int p_value) {
	
	if (p_value<MIN_PITCH_BEND_VAL || p_value>MAX_PITCH_BEND_VAL) {
		ERROR("Invalid pitch bend val " << p_value);
		return;
	}
	
	pitch_bend=p_value;
			
}


void MIDI_Controls::set_pressure(int p_pressure) {
	
	if (p_pressure<0 || p_pressure>PRESSURE_MAX_VALUE) {
		ERROR("Invalid pressure value! " << p_pressure);
		return;
	}
	
	pressure=p_pressure;
		
}

int MIDI_Controls::get_pressure() {
	
	return pressure;	
}


int MIDI_Controls::get_pitch_bend_range() {
	
	return pitch_bend_range;
}

void MIDI_Controls::reset() {
	
	for (int i=0;i<MIDI_CONTROLS_AMOUNT;i++) {
		
		control_array[i]=CONTROL_USE_DEFAULT_VALUE;
	}	
	
	pitch_bend=CENTER_PITCH_BEND_VAL;
	pitch_bend_range=2;
	pressure=0;
}


MIDI_Controls::MIDI_Controls() {
	
	reset();
}


MIDI_Controls::~MIDI_Controls()
{
}


