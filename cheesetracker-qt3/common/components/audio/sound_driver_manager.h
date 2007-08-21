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
                          sound_driver_manager.h  -  description
                             -------------------
    begin                : Wed Apr 18 2001
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

#ifndef SOUND_DRIVER_MANAGER_H
#define SOUND_DRIVER_MANAGER_H

#include "os/mutex_lock.h"
#include "sound_driver.h"
#include <vector>

/**
  *@author Juan Linietsky
  */

/******************************
 sound_driver_manager.h
 --------

This manages all your sound drivers
and proovides transparent access to them
C++ is r0x0rs.
********************************/

#define DEFAULT_MIX_FREQUENCY 44100
#define DEFAULT_MIX_STEREO true
#define DEFAULT_MIX_16BITS true
#define DEFAULT_MIX_BUFFSIZE 4096

class Sound_Driver_Manager : public SigC::Object {

	static Sound_Driver_Manager * singleton_instance;

	typedef vector<Sound_Driver*> Driver_List;
	typedef vector<Mixer_Base*> Mixer_List;

	Driver_List driver_list;
	Mixer_List mixer_list;
	int active_driver_index;


	Mutex_Lock *variables_lock;
	Mutex_Lock *driver_lock; //this one is exclusively for adding/removing mixers - this way we dont have to lock variables when in "blocking" mode

       	int tmp_mix_frequency;
	bool tmp_mix_16bits;
	bool tmp_mix_stereo;
	int tmp_mix_buffsize;

	void internal_driver_init();
	bool poll_active_driver();
	void internal_poll_active_driver();
public:

	static Sound_Driver_Manager * get_singleton_instance();
	

	SigC::Signal0<void> callback;

	SigC::Signal1<void,int> sampling_rate_changed_signal;
	
	Mutex_Lock* get_variables_lock() { return variables_lock; };
	void request_mix_frequency(int p_mix_frequency);
	void request_mix_stereo(bool p_mix_stereo);
	void request_mix_16bits(bool p_mix_16bits);
	void request_buffer_size(int p_buffsize);

	int get_mix_frequency();
	bool get_mix_stereo();
	bool get_mix_16bits();
	int get_mix_buffer_size();

	void register_driver(Sound_Driver* p_new_driver);
 	void stop_active_driver(bool p_lock=true);

	int get_amount_of_drivers();

	string get_driver_name(int p_driver_index);

	int get_active_driver();
	void set_active_driver(int p_driver_index);
	bool external_poll_active_driver();


	void reset_active_driver();
	bool is_active_driver_active();

	bool does_active_driver_perform_disk_write();
	string get_active_driver_disk_write_extension();
	string get_active_driver_current_file();
	void set_active_driver_disk_writer_file(string p_file);

	void set_variables_lock(Mutex_Lock *p_variables_lock) { variables_lock=p_variables_lock; };
	void set_driver_lock(Mutex_Lock *p_driver_lock) { driver_lock=p_driver_lock; }
	void add_mixer(Mixer_Base* p_mixer);
	void remove_mixer(Mixer_Base* p_mixer);

	Sound_Driver_Manager();
	~Sound_Driver_Manager();
};

#endif
