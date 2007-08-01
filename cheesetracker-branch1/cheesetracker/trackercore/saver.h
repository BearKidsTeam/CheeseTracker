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
                          saver.h  -  description
                             -------------------
    begin                : Sun Apr 22 2001
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

#ifndef SAVER_H
#define SAVER_H

#include "components/audio/mixer.h"
#include "file_writer.h"
#include "typedefs.h"
#include "song.h"

/**saves module file

  *@author Juan Linietsky
  */

/******************************
 saver.h
 --------

abstract class for sample saving
********************************/





class Saver {
protected:
	Song *song;
	Mixer *mixer;
        File_Writer writer;

public:

	enum Error {

		SAVE_OK,
		SAVE_ERROR = -1,
		UNKNOWN_TYPE = -2
	};

	virtual int save(const char *p_filename)=0;
	virtual int save_sample(const char *p_filename,int p_sample_index)=0;
	virtual int save_instrument(const char *p_filename,int p_instrument_index)=0;
        void set_song(Song *p_song) { song=p_song; };
        void set_mixer(Mixer *p_mixer) { mixer=p_mixer; };

	Saver();
	virtual ~Saver();
};



#endif
