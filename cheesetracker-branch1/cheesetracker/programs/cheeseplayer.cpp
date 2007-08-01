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
#include "loaders/loader_it.h"
#include "trackercore/player_data.h"
#include "components/audio/mixer.h"
#include "components/audio/sound_driver_manager.h"
#include "drivers/posix/sound_driver_oss.h"
#include "trackercore/file_format_manager.h"

#include "components/audio/resampler_manager.h"
#include "plugins/resamplers/resampler_raw.h"
#include "plugins/resamplers/resampler_linear.h"

#include <gtkmm/main.h>
#include "gui_gtk/interface.h"


int main(int argc, char *argv[]) {

	printf("Entering main() at %s:%d\n", __FILE__, __LINE__);

	/* Audio data */
	if (argc==1) return 0;

	Gtk::Main kit(argc, argv);

	Interface interface;

	//resmanager.register_resampler(&resampler_raw);
	resmanager.register_resampler(&resampler_linear);

	Mixer mixer;
	
	Sound_Driver_Manager driver_manager;
	Sound_Driver_OSS driver_oss;

	driver_oss.link_to_mixer(&mixer);
	driver_manager.register_driver(&driver_oss);
	driver_manager.set_active_driver(0);

	driver_manager.reset_active_driver();


	/* Tracker data */
	
	Song song;
	Loader_IT loader_it;
	
	File_Format_Manager format_manager;
	format_manager.link_to_song(&song);
	format_manager.register_loader(&loader_it);
	
	/* Player Data */
	
	Player_Data player;
	player.link_to_song(&song);
	player.link_to_mixer(&mixer);
	
	if (format_manager.load_module(argv[1])) {
	
		//printf("error loading song!");
		return 255;
	}
	

//	player.play_start_song();


	interface.show();
	Gtk::Main::run(interface);

//	while (1) {
	
//		driver_manager.external_poll_active_driver();
//		usleep(1);
//	}

	return 0;
}
