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
                          Timer_WinThreads.cpp  -  description
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

#include "timer_winthreads.h"

#ifdef WIN32_ENABLED

int Timer_WinThreads::get_msecs_elapsed() {

        Uint64 new_time,elapsed;

        QueryPerformanceCounter((LARGE_INTEGER*)&new_time);

        elapsed=new_time-count_start;
        elapsed/=(perf_freq/1000);
        
        return elapsed;
}


void Timer_WinThreads::callback() {

	while (timer_continue) {

	
		if (player!=NULL) player->update_timer(get_msecs_elapsed());

		Sleep(10);
	}

	timer_finished=true;

}

Uint32 WINAPI Timer_WinThreads::thread_callback(void* data) {

	Timer_WinThreads *dangit;


	(void*)dangit=data;

	dangit->callback();

	return 0;
}

void Timer_WinThreads::start() {

	int rc;
	
	if (timer_active) {
	
	//ERROR("Timer active!");
		return;	
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&count_start);
	
	timer_continue=true;

	thread_handle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Timer_WinThreads::thread_callback,this,HIGH_PRIORITY_CLASS,(LPDWORD)&thread_id);

}

void Timer_WinThreads::stop() {

	timer_continue=false;
	timer_active=false;

}

void Timer_WinThreads::config(int milliseconds) {


}


Timer_WinThreads::Timer_WinThreads(){
        player=NULL;
	minimum_frequency=10; // x86 linux - [[TODO]] fix for some other unixes
        QueryPerformanceFrequency((LARGE_INTEGER*)&perf_freq);

//        //printf("freq is %i\n",perf_freq);
}

Timer_WinThreads::~Timer_WinThreads(){

        
}

#endif /* __WIN32__ */
