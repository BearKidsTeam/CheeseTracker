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
}

void
multireader_lock::touch()
{
	queue.enter();
	mutex.grab();
	queue.leave();
	touchers++;
	mutex.release();
}

void
multireader_lock::let_go()
{
	mutex.grab();
	touchers--;
	mutex.release();
}

void
multireader_lock::lock()
{
	queue.enter();
	mutex.grab();
	while(touchers) {
		mutex.release();
		mutex.grab();
	}
	queue.leave();
}

void
multireader_lock::unlock()
{
	mutex.release();
}
