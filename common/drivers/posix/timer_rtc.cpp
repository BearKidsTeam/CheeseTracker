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
                          timer_rtc.cpp  -  description
                             -------------------
    begin                : Mon Oct 8 2001
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
#include "timer_rtc.h"

#ifdef LINUX_ENABLED

int Timer_RTC::get_msecs_elapsed() {

	struct timeval tv_current;
	long aux_1,aux_2;


	gettimeofday(&tv_current,&tz);

	aux_1=tv_current.tv_sec-tv_begin.tv_sec;
	aux_2=(tv_current.tv_usec+aux_1*1000000)-tv_begin.tv_usec;
	aux_2=(aux_2/1000);

	return aux_2;
}


void Timer_RTC::callback() {
        int someval;
        unsigned long data;
	while (timer_continue) {

		someval=read(fd, &data, sizeof(unsigned long));
//		//printf("I'm at %i\n",get_msecs_elapsed());
//		if (player!=NULL) player->update_timer(get_msecs_elapsed());
		timer_callback.emit(get_msecs_elapsed());
//		usleep(1);
	}

	timer_finished=true;

}
void* Timer_RTC::thread_callback(void* data) {

	try {

		Timer_RTC *dangit;

		(void*)dangit=data;

		dangit->callback();

		return NULL;
	} catch (Error E) {
		E.fatal_error();
	}
}

void Timer_RTC::start() {

        int rc;

	if (timer_active) {

		ERROR("Timer active!");
		return;	
	}

	real_speed=requested_frequency*frequency_multiplier;
	minimum_freq=requested_frequency;
	gettimeofday(&tv_begin,&tz);
	timer_active=false;
	timer_continue=true;


	fd = open (RTC_DEVICE, O_RDONLY);

        if (fd==-1) {

		ERROR("Real time clock NOT aviable.");
		return;

	}



	rc = ioctl(fd, RTC_IRQP_SET,ROOT_ACCURACY);

	if (rc == -1) {

		ERROR("Cant get a " << ROOT_ACCURACY << "hz timing, are you root? -  falling back to " << USER_ACCURACY << "hz");

		rc = ioctl(fd, RTC_IRQP_SET,USER_ACCURACY);

		if (rc == -1) {

			ERROR("Unable to configure real time clock.");

			close(fd);
			return;
		}
	}

	rc = ioctl(fd, RTC_PIE_ON, 0);

	if (rc == -1) {

		close(fd);
		return;
	}

	rc = pthread_create(&thread_handler,NULL,&thread_callback,this);
	timer_active=true;

}

void Timer_RTC::stop() {

	timer_continue=false;
	timer_active=false;
	pthread_join(thread_handler);

}

void Timer_RTC::config(int milliseconds) {

	if ((milliseconds==0) || ((milliseconds % minimum_frequency)==0)) {

		requested_frequency=milliseconds;

	} else ERROR("Requested timer frequency cant be assigned! -" << milliseconds);


}

Timer_RTC::Timer_RTC(){

//        player=NULL;
	minimum_frequency=10; // x86 linux - [[TODO]] fix for some other unixes
	frequency_multiplier=1000; // x86 linux - [[TODO]] fix for some other unixes
}

Timer_RTC::~Timer_RTC(){

}
#endif

