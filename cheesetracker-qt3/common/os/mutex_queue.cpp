/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://www.geocities.com/godlessinfidelus
    copyright            : (C) 2003 by Juan Linietsky
    maintainer           : J Phelps
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
                          mutex_queue.cpp -  
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

#include "drivers/posix/mutex_lock_pthreads.h"
#include "mutex_queue.h"

// Mutex_Queue solves the problem of the multireader_lock where if one
// thread wants to write, it must wait until there are no other threads
// holding the lock. In order for other threads to release their lock, the
// mutex must be released, which also allows new threads to acquire locks.
//
// The Mutex_Queue forces threads to acquire multireader_locks in a specific
// order, one by one, so that the acquisition of a write lock can be
// atomic.

// Maximum number of operations: 19*QUEUE_LEN - 3

void
Mutex_Queue::enter() {
#ifdef POSIX_ENABLED
	// Try to grab the mutex closest to the
	// front of the queue. We simply use try_grab(__FILE__, __LINE__)
	// on each position until it succeeds. However,
	// we reserve the last position in the line.
	size_t ix;
	for(ix=0; ix < QUEUE_LEN-1; ix++) {
		if(!waiting[ix]->try_grab(__FILE__, __LINE__))
			break;
	}
	if(ix==(QUEUE_LEN-1)) {
		// ix now points to end of waiting[],
		// which means we have reached the end
		// of the queue. Since we can't go any
		// further back, we must wait for this
		// spot to become available.
		waiting[ix]->grab(__FILE__, __LINE__);
	}
	// We are now in line. Start moving to
	// the front. We do this by obtaining
	// the mutex for the position in front of
	// us, and then releasing the mutex for the
	// current position.

	for(; ix; ix--) {
		waiting[ix-1]->grab(__FILE__, __LINE__);
		waiting[ix]->release();
	}
	// We are now at the front of the line.
	// Good bye!
#endif
}

// Maximum number of operations: 5
void
Mutex_Queue::leave()
{
#ifdef POSIX_ENABLED
	waiting[0]->release();
#endif
}

Mutex_Queue::Mutex_Queue() {
#ifdef POSIX_ENABLED
	for(size_t ix=0; ix<QUEUE_LEN; ix++) {
		waiting[ix] = new Mutex_Lock_Pthreads;
	}
#endif
}

Mutex_Queue::~Mutex_Queue() {
#ifdef POSIX_ENABLED
	for(size_t ix=0; ix<QUEUE_LEN; ix++) {
		delete waiting[ix];
	}
#endif
}
