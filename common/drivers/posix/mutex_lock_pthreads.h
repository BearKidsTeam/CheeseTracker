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
                          mutex_lock_pthreads.h  -  description
                             -------------------
    begin                : Sun Mar 18 2001
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

#ifndef MUTEX_LOCK_PTHREADS_H
#define MUTEX_LOCK_PTHREADS_H

#ifdef POSIX_ENABLED

#include "os/mutex_lock.h"
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
/**
  *@author Juan Linietsky
  */

/******************************
 mutex_lock_pthread.h
 ------------------------

pthreads version of mutex locking
********************************/

class Mutex_Lock_Pthreads : public Mutex_Lock  {

	pthread_mutex_t internal_mutex;

public:

	void grab(const char *p_file, int p_line) {
#ifndef NDEBUG
		pthread_mutex_lock(&internal_mutex);
		file = strdup(p_file);
		line=p_line;
#endif
	};
	bool try_grab(const char *p_file, int p_line) {
#ifndef NDEBUG
		if(pthread_mutex_trylock(&internal_mutex)==EBUSY) {
			return true;
		} else {
			file = strdup(p_file);
			line=p_line;
			return false;
		}
#else
		return (pthread_mutex_trylock(&internal_mutex)==EBUSY);
#endif
	};

	void release() {
		pthread_mutex_unlock(&internal_mutex);
#ifndef NDEBUG
		free(file);
		file=NULL;
		line=0;
#endif
	};

	Mutex_Lock* create_mutex_type() {

		Mutex_Lock* mutex_new = new Mutex_Lock_Pthreads;		
		return mutex_new;
	}

	Mutex_Lock_Pthreads() {
	
		 pthread_mutex_init(&internal_mutex,NULL);
	}
};

#endif /*posix enabled*/
#endif
