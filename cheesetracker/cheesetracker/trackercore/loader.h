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
                          loader.h  -  description
                             -------------------
    begin                : Sun Jan 21 2001
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

#ifndef LOADER_H
#define LOADER_H


/**Base Loader Class
  *@author Juan Linietsky
  */

#include "file_reader.h"
#include "song.h"
#include "components/audio/mixer.h"

#ifdef HAVE_GNU_BASENAME
#	include <cstring>
#else
	string basename(const char*);
#endif

/******************************
 loader.h
 ----------

Abstract base class of a lodered.
The class that loads samples/songs/
instruments/etcetera.
********************************/


class Loader {
public:
        enum Error {

		SUCCESS,
		LOADER_IN_USE,
		CANNOT_OPEN_FILE,
		FILE_ERROR,
		FILE_FORMAT_NOT_RECOGNIZED,
		HEADER_CORRUPT,
	};

protected:

	File_Reader file_read;

        Error abort_load(Error p_cause);

	Song *song;
	Mixer *mixer;
        string aux_filename;
        string format_name;
public:
	static const char * error_string[HEADER_CORRUPT+1];

	virtual Error load(const char *p_filename,bool p_load_patterns)=0;
        virtual bool test(const char *p_filename) =0;


	/* Compute Sample Info */
	virtual int get_amount_of_samples() =0;
	virtual Sample_Data *get_sample_data(int p_sample_index) =0;
	virtual string get_sample_name(int p_sample_index)=0;
	virtual void add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument)=0;
	virtual Error load_sample(const char *p_filename,int p_dest_index) =0;
	virtual Sample_Data *load_sample(const char *p_filename) {return NULL;};
	virtual Error load_samples_from_instrument(const char *p_filename) =0;

	/* Compute Instrument Info */
	virtual Error load_instrument(const char *p_filename,int p_dest_index)=0;

	virtual void transfer_data_to_song() =0;

	virtual void free_info(bool free_sampledata=false) {};

	void set_song(Song *p_song) { song=p_song; };
	void set_mixer(Mixer *p_mix) { mixer=p_mix; };
	string get_format_name() { return format_name; }

	Loader();
	virtual ~Loader();
};



#endif
