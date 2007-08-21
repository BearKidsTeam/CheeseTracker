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

#ifndef TIMER_WINTHREADS_H
#define TIMER_WINTHREADS_H

#ifdef WIN32_ENABLED

#include "timer.h"
#include <windows.h>

/**Unix sigalarm timer
  *@author Juan Sebastian Linietsky
  */

class Timer_WinThreads : public Timer  {


	volatile bool timer_continue;
	bool timer_finished;

	int timer_freq;
	int minimum_freq;
	int real_speed;

	Uint64 perf_freq;

	Uint64 count_start;

	void callback();
	static Uint32 WINAPI thread_callback(void* data);
	int get_msecs_elapsed();

	HANDLE thread_handle;
	Uint32 thread_id;
        
public:

	void start();
	void config(int milliseconds);
	void stop();

	Timer_WinThreads();
	~Timer_WinThreads();
};

#endif /* __Win32__ */
#endif
