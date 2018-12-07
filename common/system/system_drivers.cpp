//
// C++ Implementation: system_drivers
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "system_drivers.h"



System_Drivers::System_Drivers(int p_init_flags) : System(p_init_flags) {

	if (init_flags&INITIALIZE_AUDIO_OUT) {

#ifdef OSS_ENABLED
		audio.driver_manager.register_driver(&sound_driver_oss);
#endif
#ifdef JACK_ENABLED

		audio.driver_manager.register_driver(&sound_driver_jack);
#endif

	}

	if (p_init_flags&INITIALIZE_MIDI_IN) {

#ifdef ALSA_ENABLED
		midi.client_manager.add_midi_client(&midi_client_alsa);
#endif

	}

	#ifdef LADSPA_ENABLED
	effect_source_manager.register_effect_source(&ladspa_effect_source);
	#endif
}

System_Drivers::~System_Drivers()
{
}


