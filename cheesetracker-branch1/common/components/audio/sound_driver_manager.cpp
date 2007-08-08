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
                          sound_driver_manager.cpp  -  description
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

#include <new> // bad_alloc
#include "Error.h"
#include "sound_driver_manager.h"
#include "message_box.h"

Sound_Driver_Manager * Sound_Driver_Manager::singleton_instance=NULL;

Sound_Driver_Manager * Sound_Driver_Manager::get_singleton_instance() {

	return singleton_instance;
}


void Sound_Driver_Manager::add_mixer(Mixer_Base* p_mixer) {

	if (variables_lock) variables_lock->grab(__FILE__, __LINE__);
	if (driver_lock) driver_lock->grab(__FILE__, __LINE__); //we need an extra lock for this
	mixer_list.push_back(p_mixer);
	if (driver_lock) driver_lock->release();
	if (variables_lock) variables_lock->release();
	p_mixer->set_mix_frequency(get_mix_frequency());

}

void Sound_Driver_Manager::remove_mixer(Mixer_Base* p_mixer) {

	if (variables_lock) variables_lock->grab(__FILE__, __LINE__);
	if (driver_lock) driver_lock->grab(__FILE__, __LINE__); //we need an extra lock for this

	for (int i=(mixer_list.size()-1);i>=0;i--) {

		if (mixer_list[i]==p_mixer) {

			mixer_list.erase(mixer_list.begin()+i);
		}
	}
	if (driver_lock) driver_lock->release();

	if (variables_lock) variables_lock->release();
}


void Sound_Driver_Manager::internal_poll_active_driver() {

	if (driver_list[active_driver_index]->get_process_mode()==Sound_Driver::POLL) {

		ERROR("No callbacks allowed for a polling driver!!");
		return; //no callbacks allowed!
	}

	poll_active_driver();
}

bool Sound_Driver_Manager::external_poll_active_driver() {

	if (driver_list[active_driver_index]->get_process_mode()==Sound_Driver::CALLBACK) {

		return true; //no callbacks allowed!
	}

	return poll_active_driver();
}

void Sound_Driver_Manager::register_driver(Sound_Driver* p_new_driver) {

	driver_list.push_back(p_new_driver);


	p_new_driver->callback.connect(SigC::slot(*this,&Sound_Driver_Manager::internal_poll_active_driver));
//	if (driver_list.size()==1) internal_driver_init();
	//if (driver_list.size()==1) active_driver_index=0;
}

int Sound_Driver_Manager::get_amount_of_drivers() {

	return driver_list.size();

}

string Sound_Driver_Manager::get_driver_name(int p_driver_index) {

	if ((p_driver_index<0) || ((unsigned)p_driver_index>=driver_list.size()))
		return "Invalid driver request";

	return driver_list[p_driver_index]->get_name();

}

int Sound_Driver_Manager::get_active_driver() {

	return active_driver_index;

}

void Sound_Driver_Manager::set_active_driver(int p_driver_index) {

	if ((p_driver_index<0) || ((unsigned)p_driver_index>=driver_list.size())) return;
	if (p_driver_index==active_driver_index) return;
	bool driver_was_active=false;

	if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

	if (active_driver_index>=0) {

		if (driver_list[active_driver_index]->is_active()) {

				driver_list[active_driver_index]->finish();
				driver_was_active=true;

		}
	}

	active_driver_index=p_driver_index;
	if (driver_was_active) internal_driver_init();

	if (variables_lock) variables_lock->release();

}

void Sound_Driver_Manager::stop_active_driver(bool p_lock) {

	if (active_driver_index<0) return;

	if (p_lock)
		if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

	if (driver_list[active_driver_index]->is_active())
		driver_list[active_driver_index]->finish();

	if (p_lock)
		if (variables_lock) variables_lock->release();

}

bool Sound_Driver_Manager::poll_active_driver() {

	if (active_driver_index<0) return true;

	bool result = true;
        Sound_Driver::Status driver_status=Sound_Driver::IDLE;

	if (driver_list[active_driver_index]->accepts_blocking()) {
		if (variables_lock) variables_lock->grab(__FILE__, __LINE__); //commenting this out, since the mutex takes the whole thing
        } else {
		if (variables_lock) {
			if (variables_lock->try_grab(__FILE__, __LINE__))
				return false;
		}
	}


	if (is_active_driver_active()) {

		callback.emit(); //process midi stuff!
		driver_list[active_driver_index]->set_mixer_list(&mixer_list);
		driver_status=driver_list[active_driver_index]->process();
	}

	if (variables_lock) variables_lock->release();


	switch (driver_status) {

		case Sound_Driver::DISABLED: {

			//driver became disabled at some point we lost track of
			stop_active_driver(true);
			result=true;
		} break;
		case Sound_Driver::IDLE: {
			//driver is doing nothing, ask the OS to sleep instead of the driver
			result=true;
		} break;
		case Sound_Driver::ACTIVE: {
			//all fine, driver is mixing stuff, write/block to the soundcard happily
			//@TODO I will _REALLY_ need a second lock for this
			driver_list[active_driver_index]->flush();
			result=false;
		} break;
	}

	return result;

}

void Sound_Driver_Manager::reset_active_driver() {

	if (active_driver_index<0) return;

	if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

	try {
		if (driver_list[active_driver_index]->is_active()) {

			driver_list[active_driver_index]->finish();

		}
	} catch (Error E) {
		if(variables_lock)
			variables_lock->release();
		throw E;
	}

	internal_driver_init();

	if (variables_lock) variables_lock->release();
}

bool Sound_Driver_Manager::is_active_driver_active() {

	if (active_driver_index<0) return false;

	return driver_list[active_driver_index]->is_active();
}


void Sound_Driver_Manager::request_mix_frequency(int p_mix_frequency) {

	tmp_mix_frequency=p_mix_frequency;

	if (active_driver_index<0) return;

        bool need_reinit=false;

	if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

	if (driver_list[active_driver_index]->is_active()) {

		driver_list[active_driver_index]->finish();
  		need_reinit=true;

	}


	driver_list[active_driver_index]->request_mix_frequency(p_mix_frequency);

 	if (need_reinit)
		driver_list[active_driver_index]->init();

	if (variables_lock) variables_lock->release();
}

void Sound_Driver_Manager::request_mix_stereo(bool p_mix_stereo) {

	tmp_mix_stereo=p_mix_stereo;

	if (active_driver_index<0) return;
	
        bool need_reinit=false;

	if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

	if (driver_list[active_driver_index]->is_active()) {

		driver_list[active_driver_index]->finish();
  		need_reinit=true;

	}


	driver_list[active_driver_index]->request_mix_stereo(p_mix_stereo);

 	if (need_reinit)
		driver_list[active_driver_index]->init();

	if (variables_lock) variables_lock->release();

}


void Sound_Driver_Manager::request_mix_16bits(bool p_mix_16bits) {

	tmp_mix_16bits=p_mix_16bits;

	if (active_driver_index<0) return;
	
        bool need_reinit=false;

	if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

	if (driver_list[active_driver_index]->is_active()) {

			driver_list[active_driver_index]->finish();
	  		need_reinit=true;

	}

	driver_list[active_driver_index]->request_mix_16bits(p_mix_16bits);

	if (need_reinit)
		driver_list[active_driver_index]->init();

	if (variables_lock) variables_lock->release();

}

void Sound_Driver_Manager::request_buffer_size(int p_buffsize) {

	///// FIXME: The start() and finish() methods in this program
	///// can throw exceptions, which will cause variables_lock->release()
	///// to be skipped over, which will lead to a deadlock.

	tmp_mix_buffsize=p_buffsize;

	if (active_driver_index<0) return;

        bool need_reinit=false;

	if (variables_lock) variables_lock->grab(__FILE__, __LINE__);

	if (driver_list[active_driver_index]->is_active()) {
			driver_list[active_driver_index]->finish();
	  		need_reinit=true;
	}

	if (driver_lock) driver_lock->grab(__FILE__, __LINE__); //we need an extra lock for this
	driver_list[active_driver_index]->request_buffer_size(p_buffsize);
	if (driver_lock) driver_lock->release();

 	if (need_reinit)
		driver_list[active_driver_index]->init();

	if (variables_lock) variables_lock->release();

}

bool Sound_Driver_Manager::does_active_driver_perform_disk_write() {
	if (active_driver_index<0)
		return false;

	return driver_list[active_driver_index]->does_perform_disk_write();

}
string Sound_Driver_Manager::get_active_driver_disk_write_extension() {
	if (active_driver_index<0)
		return "";

	return driver_list[active_driver_index]->get_disk_writer_extension();

}
string Sound_Driver_Manager::get_active_driver_current_file() {
	if (active_driver_index<0)
		return "";

	return driver_list[active_driver_index]->get_disk_writer_file();
}

void Sound_Driver_Manager::set_active_driver_disk_writer_file(string p_file) {
	if (active_driver_index<0)
		return;

	driver_list[active_driver_index]->set_disk_writer_file(p_file);
}

void Sound_Driver_Manager::internal_driver_init() {

	if (active_driver_index<0) return;

	try {
		driver_list[active_driver_index]->request_mix_frequency(tmp_mix_frequency);
		driver_list[active_driver_index]->request_mix_stereo(tmp_mix_stereo);
		driver_list[active_driver_index]->request_mix_16bits(tmp_mix_16bits);
		driver_list[active_driver_index]->request_buffer_size(tmp_mix_buffsize);
		driver_list[active_driver_index]->init();
		sampling_rate_changed_signal.emit( driver_list[active_driver_index]->get_mix_frequency() );
	} catch (Error E) {
		message_box("Error", E.what(), "OK");
	} catch (std::bad_alloc B) {
		message_box("SENILE.SYS loaded", "Out of memory", "¡Chinga!");
	} 
}
int Sound_Driver_Manager::get_mix_frequency() {
	if (active_driver_index<0)
		return tmp_mix_frequency;

	return driver_list[active_driver_index]->get_mix_frequency();
}
bool Sound_Driver_Manager::get_mix_stereo() {
	if (active_driver_index<0)
		return tmp_mix_stereo;

	return driver_list[active_driver_index]->get_mix_stereo();
}
bool Sound_Driver_Manager::get_mix_16bits() {
	if (active_driver_index<0)
		return tmp_mix_16bits;

        return driver_list[active_driver_index]->get_mix_16bits();
}
int Sound_Driver_Manager::get_mix_buffer_size() {

	if (active_driver_index<0)
		return tmp_mix_buffsize;
        return driver_list[active_driver_index]->get_mix_buffer_size();
}

Sound_Driver_Manager::Sound_Driver_Manager(){

	if (singleton_instance) {
		ERROR(" You cant instance the sound driver manager more than once!!!");
		return;
	}

	singleton_instance=this;

	tmp_mix_frequency=DEFAULT_MIX_FREQUENCY;
	tmp_mix_stereo=DEFAULT_MIX_STEREO;
	tmp_mix_16bits=DEFAULT_MIX_16BITS;
	tmp_mix_buffsize=DEFAULT_MIX_BUFFSIZE;
	variables_lock=NULL;

	active_driver_index=-1;
	driver_lock=NULL;
}
Sound_Driver_Manager::~Sound_Driver_Manager(){
}
