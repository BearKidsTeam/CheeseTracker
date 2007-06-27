#include "drivers/posix/mutex_lock_pthreads.h"
#include "multireader_lock.h"

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
                          multireader_lock.cpp
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

// Maximum number of operations: 2

multireader_lock::multireader_lock()
{
	touchers=0;
#ifdef POSIX_ENABLED
	mutex = new Mutex_Lock_Pthreads;
#endif
}

#ifdef POSIX_ENABLED
multireader_lock::~multireader_lock() {
	delete mutex;
}
#endif

void
multireader_lock::touch()
{
	queue.enter();
#ifdef POSIX_ENABLED
	mutex->grab(__FILE__, __LINE__);
#endif
	queue.leave();
	touchers++;
#ifdef POSIX_ENABLED
	mutex->release();
#endif
}

void
multireader_lock::let_go()
{
#ifdef POSIX_ENABLED
	mutex->grab(__FILE__, __LINE__);
#endif
	touchers--;
#ifdef POSIX_ENABLED
	mutex->release();
#endif
}

void
multireader_lock::lock()
{
	queue.enter();
#ifdef POSIX_ENABLED
	mutex->grab(__FILE__, __LINE__);
	while(touchers) {
		mutex->release();
		mutex->grab(__FILE__, __LINE__);
	}
#endif
	queue.leave();
}

void
multireader_lock::unlock()
{
#ifdef POSIX_ENABLED
	mutex->release();
#endif
}
