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

#ifndef SOUND_DRIVER_WINMM_H
#define SOUND_DRIVER_WINMM_H


#include "components/audio/sound_driver.h"
#include <vector>
	using std::vector;

/**sound driver base class
  *@author Juan Linietsky
  */

/******************************
 Sound_Driver_WinMM.h
 --------

dummy sound driver
********************************/

#if ((CYGWIN_ENABLED) || (WIN32_ENABLED))

class WinMMPrivate;

class Sound_Driver_WinMM : public Sound_Driver {

	bool active;
	WinMMPrivate *p;
	int block_count;
	int block_to_process;
	Uint8 *audiobuffer;
	
	void process_data(int p_bytes);


public:
	void notify_buffer_done();

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


	Sound_Driver_WinMM();
	virtual ~Sound_Driver_WinMM();

};


#endif
#endif
