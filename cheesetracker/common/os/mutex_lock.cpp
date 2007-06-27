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
                          mutex_lock.cpp  -  description
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

#include <cstdlib>
#include "mutex_lock.h"

//void * Mutex_Lock::mutex_creation_function=NULL;
Mutex_Lock* Mutex_Lock::default_type=NULL;

Mutex_Lock* Mutex_Lock::create_mutex() {

        if (default_type==NULL) {
		ERROR("No default mutex type specified!!");
		return NULL;
	}

	return default_type->create_mutex_type();
}

Mutex_Lock::Mutex_Lock(){
	file=NULL;
	line=0;
}
Mutex_Lock::~Mutex_Lock(){
	free(file);
}
