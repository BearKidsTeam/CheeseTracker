//
// C++ Interface: midi_controls
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MIDI_CONTROLS_H
#define MIDI_CONTROLS_H

#include "typedefs.h"

/**
@author Juan Linietsky
*/
        
	
	
class Midi_Control; //geez must rename this urgently

class MIDI_Controls {
public:
	enum {
		
		MIDI_CONTROLS_AMOUNT=128,
		MAX_PITCH_BEND_VAL=0x3FFF,
		MIN_PITCH_BEND_VAL=0,
		CENTER_PITCH_BEND_VAL=0x2000,

		PRESSURE_MAX_VALUE=127,				
								
		CONTROL_MAX_VALUE=127,
		CONTROL_USE_DEFAULT_VALUE=-1
	};
private:
	
	int control_array[MIDI_CONTROLS_AMOUNT];
	
	int pitch_bend;
	int pitch_bend_range; //semitones
	
	int pressure;

protected:
	
friend class Midi_Control; //these methods are for this class

	void set_control(int p_control, int p_value);
	void set_pitch_bend(int p_value);
	void set_pressure(int p_pressure);
        void reset();

public:
	
	int get_control(int p_control); // CONTROL_USE_DEFAULT_VALUE or 0 .. 127
	int get_pitch_bend();
	int get_pressure();
	int get_pitch_bend_range();
		
	MIDI_Controls();
	~MIDI_Controls();

};

#endif
