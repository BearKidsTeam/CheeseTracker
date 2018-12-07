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
                          timer_sigalarm.cpp  -  description
                             -------------------
    begin                : Thu Feb 15 2001
    copyright            : (C) 2001 by Juan Sebastian Linietsky
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

#include "Error.h"
#include "timer_sigalarm.h"

#ifdef POSIX_ENABLED

int Timer_SigAlarm::get_msecs_elapsed() {

	struct timeval tv_current;
	long aux_1,aux_2;


	gettimeofday(&tv_current,&tz);

	aux_1=tv_current.tv_sec-tv_begin.tv_sec;
	aux_2=(tv_current.tv_usec+aux_1*1000000)-tv_begin.tv_usec;
	aux_2=(aux_2/1000);

	return aux_2;
}


void Timer_SigAlarm::callback() {

	

	while (timer_continue) {

		timer_callback.emit(get_msecs_elapsed());
		usleep(1);
	}

	timer_finished=true;

}
void* Timer_SigAlarm::thread_callback(void* data) {

	try {
		Timer_SigAlarm *dangit;

		dangit=(Timer_SigAlarm*)data;

		dangit->callback();

		return NULL;
	} catch (Error E) {
		E.fatal_error();
	}
}

void Timer_SigAlarm::start() {

        int rc,i;

	if (timer_active) {

		ERROR("Timer active!");
		return;	
	}
        //printf("rf %i/fm %i = \n",requested_frequency,frequency_multiplier);
	real_speed=requested_frequency*frequency_multiplier;
	minimum_freq=requested_frequency;
	gettimeofday(&tv_begin,&tz);
	timer_iterations_count=0;
	timer_active=true;
	timer_continue=true;

	for (i=0;i<threads_to_spawn;i++) {

		rc = pthread_create(&thread_handler[i],NULL,&thread_callback,this);
	}

}

void Timer_SigAlarm::stop() {

	timer_continue=false;
	timer_active=false;
	for(size_t ix=0; ix<threads_to_spawn; ix++) {
		pthread_join(thread_handler[ix], NULL);
	}

}

void Timer_SigAlarm::config(int milliseconds) {

	if ((milliseconds==0) || ((milliseconds % minimum_frequency)==0)) {

		requested_frequency=milliseconds;

	} else ERROR("Requested timer frequency cant be assigned! -" << milliseconds);


}

void Timer_SigAlarm::set_number_of_threads(int p_number) {

	if ((threads_to_spawn<0) || (threads_to_spawn>=TIMER_PTHREADS_MAX_THREADS)) return;
	threads_to_spawn=p_number;
}

Timer_SigAlarm::Timer_SigAlarm(){
	threads_to_spawn=1;
//        player=NULL;
	minimum_frequency=10; // x86 linux - [[TODO]] fix for some other unixes
	frequency_multiplier=1000; // x86 linux - [[TODO]] fix for some other unixes
}

Timer_SigAlarm::~Timer_SigAlarm(){

}
#endif

