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
                          timer.h  -  description
                             -------------------
    begin                : Sun Nov 19 2000
    copyright            : (C) 2000 by Juan Sebastian Linietsky
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

#ifndef TIMER_H
#define TIMER_H

#include "typedefs.h"
#include <sigc++/signal.h>


/**Timing functions
  *@author Juan Sebastian Linietsky
  */

class Timer {
protected:

	int minimum_frequency;
	int requested_frequency;

	bool timer_active;

public:

	SigC::Signal1<void,float> timer_callback;

	virtual void start() {};
	virtual void config(int milliseconds) {};
	virtual void stop() {};

	bool is_active() { return timer_active; }

	int get_minimum_frequency();

	Timer();
	virtual ~Timer();
};

#endif
