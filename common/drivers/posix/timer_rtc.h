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
                          timer_rtc.h  -  description
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

#ifndef TIMER_RTC_H
#define TIMER_RTC_H


/**
  *@author Juan Sebastian Linietsky
  */
#ifdef LINUX_ENABLED

#include "os/timer.h"
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/**Unix sigalarm timer
  *@author Juan Sebastian Linietsky
  */

#define TIMER_PTHREADS_MAX_THREADS 20
#define RTC_DEVICE "/dev/rtc"

#define ROOT_ACCURACY 256
#define USER_ACCURACY 64

class Timer_RTC : public Timer  {

	pthread_t thread_handler;
        int fd;
	struct timeval tv_begin;
	struct timezone tz;
	volatile bool timer_continue;
	bool timer_finished;
	int frequency_multiplier; // unixes need this!
	int timer_freq;
	int minimum_freq;
        int real_speed;

        void callback();
	static void* thread_callback(void* data);
	int get_msecs_elapsed();



public:

	void start();
	void config(int milliseconds);
	void stop();

	Timer_RTC();
	~Timer_RTC();
};


#endif /*posix enabled */
#endif
