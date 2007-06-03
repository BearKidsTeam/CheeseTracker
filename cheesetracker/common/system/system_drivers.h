//
// C++ Interface: system_drivers
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SYSTEM_DRIVERS_H
#define SYSTEM_DRIVERS_H

#include "system/system.h"
#include "drivers/posix/sound_driver_oss.h"
#include "drivers/posix/sound_driver_jack.h"
#include "drivers/posix/midi_client_alsa.h"
#include "drivers/posix/midi_out_device_oss.h"
#include "drivers/posix/midi_out_device_alsa.h"
#include "plugins/effects/ladspa/ladspa_effect_source.h"

/* The "Dirty" Class */

/**
@author Juan Linietsky
*/
class System_Drivers : public System {
public:


#ifdef ALSA_ENABLED

	MIDI_Client_ALSA midi_client_alsa;
	MidiOutDevice_ALSA midi_out_alsa;
#endif

#ifdef OSS_ENABLED

#	ifndef ALSA_ENABLED

	MidiOutDevice_OSS midi_out_oss;
#	endif

	Sound_Driver_OSS sound_driver_oss;
#endif

#ifdef JACK_ENABLED

	Sound_Driver_JACK sound_driver_jack;
#endif

#ifdef LADSPA_ENABLED
	LADSPA_Effect_Source ladspa_effect_source;
#endif

	System_Drivers(int p_init_flags);
	~System_Drivers();

};

#endif
