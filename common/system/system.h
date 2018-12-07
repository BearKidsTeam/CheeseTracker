//
// C++ Interface: component_bundle
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SYSTEM__H
#define SYSTEM__H

#include "components/audio/mixer.h"
#include "components/audio/sound_driver_manager.h"
#include "components/midi/engine_manager.h"
#include "components/midi/midi_client_manager.h"
#include "components/midi/midi_control.h"
#include "components/audio/effect_source_manager.h"
#include "components/audio/effect_source_internal.h"

/**
@author Juan Linietsky
*/
class System {

	System();

protected:


	static System *singleton_instance;

	int init_flags;

	System(int p_init_flags);
public:

	static System* get_singleton_instance();

	enum InitFlags {

		INITIALIZE_AUDIO_OUT = (1<<0),
		INITIALIZE_AUDIO_IN = (1<<1),
		INITIALIZE_AUDIO = INITIALIZE_AUDIO_IN|INITIALIZE_AUDIO_OUT,

		INITIALIZE_MIDI_OUT = (1<<2),
		INITIALIZE_MIDI_IN = (1<<3),
		INITIALIZE_MIDI = INITIALIZE_MIDI_IN|INITIALIZE_MIDI_OUT
	};


	/* Standard Components */

	struct Midi {

		Midi_Control control; /* Receives midi from client manager, asks a voice to Engine Manager and passes it to Mixer */
		Engine_Manager engine_manager;
		MIDI_Client_Manager client_manager;

	} midi;

	struct Audio {

		Mixer mixer;
		Sound_Driver_Manager driver_manager;

	} audio;

	Effect_Source_Manager effect_source_manager;
	Effect_Source_Internal effect_source_internal;
	

	/* Driver specific stuff */


	~System();

};

#endif
