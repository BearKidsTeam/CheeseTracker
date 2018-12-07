#ifndef MULTIREADER_LOCK_H
#define MULTIREADER_LOCK_H

/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://www.geocities.com/godlessinfidelus
    copyright            : (C) 2003 by Juan Linietsky
    email                : coding@reduz.com.ar
    maintainer email     : godless@users.sf.net
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
                          multireader_lock.h -  
                             -------------------
    begin                : Tue 11 Jun 2007
    copyright            : © 2007 by Jeremy Phelps
    email                : godless@users.sf.net
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
#include "mutex_queue.h"

// The multireader_lock allows an infinite number of 
// processes to access a memory region for read-only
// access, and one process to lock all others out for
// read-write access. 

// The multireader_lock has two pairs of operations: touch()
// and let_go() are for read-only access, while lock() and
// unlock() are for write access.
//
// touch() blocks while the multireader_lock is locked, and
// lock() blocks while there are any threads which
// have called touch() but have not called let_go().

class multireader_lock {
		Mutex_Lock *mutex;
		Mutex_Queue queue;
		size_t touchers;
	public:
		multireader_lock();
#ifdef POSIX_ENABLED
		~multireader_lock();
#endif
		void touch();
		void let_go();
		void lock();
		void unlock();
};

#define LOCK false
#define TOUCH true

class multireader_lock_container {
		multireader_lock *asem;
		bool do_touch;
	public:
		multireader_lock_container(multireader_lock *sem, bool touch_only) {
			asem=sem;
			do_touch=touch_only;
			if(touch_only)	asem->touch();
			else		asem->lock();
		}
		~multireader_lock_container() {
			if(do_touch)	asem->let_go();
			else		asem->unlock();
		}
};

#endif
