#ifndef MUTEX_QUEUE_H
#define MUTEX_QUEUE_H

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
                          mutex_queue.h -  
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

#include "mutex_lock.h"

#define QUEUE_LEN 2

class Mutex_Queue {
		Mutex_Lock *waiting[QUEUE_LEN];
	public:
		Mutex_Queue();
		~Mutex_Queue();
		void enter();
		void leave();
};

#endif
