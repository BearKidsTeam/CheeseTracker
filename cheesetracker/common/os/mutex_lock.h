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

	static Mutex_Lock *default_type;
	static Mutex_Lock *create_mutex();

	virtual void grab()=0;
	virtual bool try_grab()=0; //return true if grab failed
	virtual void release()=0;
        virtual Mutex_Lock* create_mutex_type()=0;

	Mutex_Lock();
	virtual ~Mutex_Lock();
};

class Mutex_Lock_Container {
	private:
		std::vector<Mutex_Lock *> locks;
	public:
		Mutex_Lock_Container (Mutex_Lock *lck) {
			add(lck);
		}
		void add(Mutex_Lock *lck) {
#ifdef POSIX_ENABLED
			lck->grab();
			locks.push_back(lck);
#endif
		}
		~Mutex_Lock_Container() {
#ifdef POSIX_ENABLED
			vector<Mutex_Lock*>::iterator ix;
			for(ix=locks.begin(); ix!=locks.end(); ix++) {
				(*ix)->release();
			}
#endif
		}
};

#endif
