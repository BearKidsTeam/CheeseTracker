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
                          sound_driver_oss.h  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Juan Linietsky
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



#ifndef SOUND_DRIVER_OSS_H
#define SOUND_DRIVER_OSS_H


#ifdef OSS_ENABLED


#include <unistd.h>

#include "components/audio/sound_driver.h"
#include <fcntl.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>




/**Sound driver for OSS
  *@author Juan Linietsky
  */


/******************************
 sound_driver_OSS.h
 --------

This is an OSS sound driver!
********************************/

class Sound_Driver_OSS : public Sound_Driver  {


	string device_file;
	int sound_fd;

	Sint8 *audiobuffer;
	int buffersize;
	int play_precision;

	int fragsize;
	int numfrags;

	bool active;

	void process_data(int p_samples);

	int current_buffer_size;

	bool blocking_mode;

	bool does_perform_disk_write();
	string get_disk_writer_extension();
	string get_disk_writer_file();
	void set_disk_writer_file(string p_file);


public:

	int get_mix_frequency();
	bool get_mix_stereo();
	bool get_mix_16bits();
	int get_mix_buffer_size();

 	int init();
	bool finish();
	Status process();
	void flush();
	bool is_active() {return active; };

       	string get_name() { return "OSS Sound Driver"; };

	Sound_Driver_OSS();
	~Sound_Driver_OSS();
};

#endif

#endif
