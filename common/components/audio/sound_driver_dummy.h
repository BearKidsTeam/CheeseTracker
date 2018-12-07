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
                          sound_driver.h  -  description
                             -------------------
    begin                : Sun Jan 28 2001
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

#ifndef SOUND_DRIVER_DUMMY_H
#define SOUND_DRIVER_DUMMY_H


#include "components/audio/sound_driver.h"
#include <vector>
	using std::vector;

/**sound driver base class
  *@author Juan Linietsky
  */

/******************************
 sound_driver_dummy.h
 --------

dummy sound driver
********************************/

class Sound_Driver_Dummy : public Sound_Driver {

    bool active;
public:


      	virtual int get_mix_frequency() ;
        virtual bool get_mix_stereo() ;
        virtual bool get_mix_16bits() ;
        virtual int get_mix_buffer_size() ;

	virtual string get_name() ;

	virtual int init() ;
	virtual bool finish() ;
	virtual bool is_active() ;
        virtual Status process() ;
        virtual void flush() ;


	Sound_Driver_Dummy();
	virtual ~Sound_Driver_Dummy();

};

#endif
