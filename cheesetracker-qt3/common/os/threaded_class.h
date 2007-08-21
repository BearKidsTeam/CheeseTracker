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
#ifndef THREADED_CLASS_H
#define THREADED_CLASS_H

#include <sigc++/sigc++.h>
#include <typedefs.h>

class ThreadedClass : public SigC::Object {

public:

	static ThreadedClass *default_type; ///< check mutex_lock.h. long story
	static ThreadedClass *create_thread();

	virtual ThreadedClass* create_thread_type()=0;

	SigC::Signal0<void> thread_callback;
	virtual void start_thread() = 0;
	virtual void cleanup_thread() = 0;
};



#endif
