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
                          sound_driver_jack.h  -  description
                             -------------------
    begin                : October 2003
    copyright            : (C) Juan Linietsky / Kasper Souren
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

#ifdef JACK_ENABLED

#ifndef SOUND_DRIVER_JACK_H
#define SOUND_DRIVER_JACK_H

#include <unistd.h>

#include "components/audio/sound_driver.h"
//#include "sound_driver.h"
#include <jack/jack.h>


/**Sound driver for JACK
  *@author Juan Linietsky & Kasper Souren
  */


/******************************
 sound_driver_jack.h
 --------

This is a JACK sound driver!
********************************/

#define BUFFERSIZE 16384

class Sound_Driver_JACK : public Sound_Driver  {

        jack_port_t *output_port_l;
        jack_port_t *output_port_r;
	bool active;
	bool jack_active;
	jack_client_t *client;

	int real_mixfreq;
	int real_buffersize;
	int process_amount;

	Mixer_Base *mixer;

	int jack_callback_nframes;

	Sound_Driver::Status process();

        static int process_jack_callback(jack_nframes_t nframes,void *arg);
        static int jack_set_mixfreq(jack_nframes_t nframes,void *arg);
        static int jack_set_buffsize(jack_nframes_t nframes,void *arg);
        static void jack_shutdown(void *arg);

        jack_default_audio_sample_t *buffer_l;
        jack_default_audio_sample_t *buffer_r;

        int useless;
	
	string client_name;

public:

	void set_client_name(string p_name);
        bool accepts_blocking() { return false; }
        void set_ext_mixfreq(int p_mixfreq);
        void set_ext_mixbuffsize(int p_mixbuffsize);
        void set_ext_process(int p_amount);
        void set_ext_jackdied();

        int mix_samples(int p_amount);

	int get_mix_frequency();
	bool get_mix_stereo();
	bool get_mix_16bits();
	int get_mix_buffer_size();

	void flush();


 	int init();
	bool finish();
	bool update();
	bool is_active() {return active; };

       	string get_name() { return "JACK Sound Driver"; };

	Sound_Driver_JACK();
	~Sound_Driver_JACK();
};

#endif
#endif

