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
                          timer_sigalarm.h  -  description
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

#ifndef TIMER_SIGALARM_H
#define TIMER_SIGALARM_H

#ifdef POSIX_ENABLED

#include "os/timer.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
/**Unix sigalarm timer
  *@author Juan Sebastian Linietsky
  */

#define TIMER_PTHREADS_MAX_THREADS 20

class Timer_SigAlarm : public Timer  {

	pthread_t thread_handler[TIMER_PTHREADS_MAX_THREADS];

	int timer_iterations_count;

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
        int threads_to_spawn;
	

public:

	void set_number_of_threads(int p_number);
	void start();
	void config(int milliseconds);
	void stop();

	Timer_SigAlarm();
	~Timer_SigAlarm();
};

#endif /*posix enabled! */
#endif

