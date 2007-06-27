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
                          mutex_lock.h  -  description
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

#ifndef MUTEX_LOCK_H
#define MUTEX_LOCK_H

#include <vector>
#include "typedefs.h"

/**
  *@author Juan Linietsky
  */
/******************************
 mutex_lock.h
 ------------------------

class to abstract mutex locking
********************************/


class Mutex_Lock {
public:
#ifndef NDEBUG
	char *file;	// The source file where this was grabbed,
			// for debugging deadlocks.
	int line;	// The line in the source file.
#endif

	static Mutex_Lock *default_type;
	static Mutex_Lock *create_mutex();

	virtual void grab(const char *p_file, int p_line)=0;
	virtual bool try_grab(const char *p_file, int p_line)=0; //return true if grab failed
	virtual void release()=0;
        virtual Mutex_Lock* create_mutex_type()=0;

	Mutex_Lock();
	virtual ~Mutex_Lock();
};

// Mutex_Lock_Container does not own the Mutex_Locks it contains.
// Therefore, a Mutex_Lock_Container must not outlive the object that
// returned it.

class Mutex_Lock_Container {
	private:
		Mutex_Lock *lock;
	public:
		Mutex_Lock_Container (Mutex_Lock *lck, const char *file, int line) {
#ifdef POSIX_ENABLED
			lck->grab(file, line);
#endif
			lock = lck;
		}
		
		~Mutex_Lock_Container() {
#ifdef POSIX_ENABLED
			lock->release();
#endif
		}
};

#endif
