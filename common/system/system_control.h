//
// C++ Interface: system_control
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SYSTEM_CONTROL_H
#define SYSTEM_CONTROL_H


#include "system/system.h"
/**
@author Juan Linietsky
*/
class System_Control : public SigC::Object {

	System *system;
	

	pthread_t player_thread;
	void audio_process_callback();

	static bool audio_thread_quit_requested;
	void audio_thread_callback_internal();
	static void * audio_thread_callback(void *p_instance);
	
	
	static System_Control * singleton_instance;
public:

	static System_Control * get_singleton_instance();
	void set_system(System *p_system);
	
	bool init_audio_thread(); //return true on failure
	void stop_audio_thread();

	void passive_poll(); //polling called by a non-rt/audio-thread a few times per second
	
	System_Control();
	~System_Control();

};

#endif
