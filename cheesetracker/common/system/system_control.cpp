//
// C++ Implementation: system_control
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "Error.h"
#include "system_control.h"
#include <unistd.h>


bool System_Control::audio_thread_quit_requested=false;

System_Control * System_Control::singleton_instance=NULL;


System_Control * System_Control::get_singleton_instance() {

	return singleton_instance;
}


void System_Control::audio_process_callback() {

	system->midi.control.poll_events();
}



void System_Control::set_system(System *p_system) {

	system=p_system;
	system->audio.mixer.process_callback.connect(SigC::slot(*this,&System_Control::audio_process_callback));
	system->audio.mixer.set_process_callback_interval(200); //200 for now, will change later
}

// Returns true on failure -- No evidence that this ever gets called.

bool System_Control::init_audio_thread() {

	// NOTE: Not even remotely portable to Windows,
	// but who cares, it ports to everything else.

	printf("init_audio_thread:%d: Called, but from where???\n", __LINE__);

	pthread_attr_t thread_attr;
	
	pthread_attr_init(&thread_attr);
	sched_param thread_sched_param;
	memset(&thread_sched_param,0,sizeof(thread_sched_param));
	int priority=sched_get_priority_max(SCHED_RR);
	thread_sched_param.sched_priority=priority;
        pthread_attr_setschedparam (&thread_attr,&thread_sched_param);
	
	audio_thread_quit_requested=false;
	
	pthread_create(&player_thread,&thread_attr,&audio_thread_callback,this);

	return false;
}


void  System_Control::audio_thread_callback_internal() {

	while(!audio_thread_quit_requested) {
	
		if (system->audio.driver_manager.external_poll_active_driver())
			usleep(1);
	
	}
}

void * System_Control::audio_thread_callback(void *p_instance) {

	try {
		System_Control * instance = (System_Control*) p_instance;
		instance->audio_thread_callback_internal();
		
		return NULL;
	} catch (Error E) {
		E.fatal_error();
	}
}


void System_Control::stop_audio_thread() {

	audio_thread_quit_requested=true;
	pthread_join(player_thread, NULL);
}


void System_Control::passive_poll() {

	system->midi.engine_manager.passive_poll();
}

System_Control::System_Control() {

	singleton_instance=this;
}

System_Control::~System_Control() {
}


