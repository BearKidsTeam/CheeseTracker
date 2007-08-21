//
// C++ Interface: midi_event
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MIDI_EVENT_H
#define MIDI_EVENT_H

#include "typedefs.h"
/**
@author Juan Linietsky
*/
struct MIDI_Event {

	enum Type {
		NOTE_ON,
		NOTE_OFF,
		NOTE_PRESSURE,
		CONTROLLER,
		PATCH_SELECT,
		AFTERTOUCH, //channel pressure
		PITCH_BEND,
		SYSEX //this will not be used here anyway
	};



        Type type;
	Uint8 channel; /* not necesarily 0-15, it can be handling more than one device */

	union Data {

		struct Raw {
			Sint8 param1;
			Sint8 param2;
		} raw;

		struct Note { /* Note On / Note Off / Note Pressure */

			Sint8 note;
			Sint8 velocity;
		} note;

		struct Control { /* Controller */

			Sint8 index;
			Sint8 parameter;
		} control;

		struct Aftertouch { /* Channel Pressure */

			Sint8 pressure;
		} aftertouch;

		struct Patch { /* Patch */

			Sint8 index;
		} patch;

		struct Pitch_Bend { /* Pitch Bend */

			Sint16 bend; /* 0 - 0x3999 */
		} pitch_bend;


	} data;

	Uint32 timestamp; //Time when event was received in msecs. should be enough precision.

};

#endif
