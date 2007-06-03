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

#include "drivers/posix/sound_driver_oss.h"
#include "interface_binds/tracker_instance.h"
#include "components/audio/sound_driver_manager.h"


int main(int argc, char *argv[]) {

	printf("Entering main() at %s:%d\n", __FILE__, __LINE__);

	/* Audio data */
	if (argc==1) return 0;

	Tracker_Instance tracker;
	Sound_Driver_Manager driver_manager;

#ifdef OSS_ENABLED
	Sound_Driver_OSS driver_oss;
	driver_manager.register_driver(&driver_oss);
#endif

#if ((CYGWIN_ENABLED) || (WIN32_ENABLED))

	Sound_Driver_WinMM driver_winmm;
	driver_manager.register_driver(&driver_winmm);
#endif

	Resampler_Manager resampler_manager;
	Resampler_Raw resampler_raw;
	Resampler_Linear resampler_linear;
	resampler_manager.register_resampler(&resampler_raw);
	resampler_manager.register_resampler(&resampler_linear);

	driver_manager.set_active_driver(0);
	driver_manager.reset_active_driver();
	driver_manager.add_mixer(&tracker.mixer); //register this mixer



	/* Tracker data */

	if (tracker.format_manager.load_module(argv[1])) {

		//printf("error loading song!");
		return 255;
	}


	tracker.player.play_stop();
	tracker.player.play_start_song();


	int cycles=2000;

	while (cycles--) {

		if (driver_manager.external_poll_active_driver())
			usleep(1);

		////printf("cycles at %i\n",cycles);
	}

	return 0;
}
