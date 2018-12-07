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
                          saver_it.h  -  description
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

#ifndef SAVER_IT_H
#define SAVER_IT_H

#include "saver.h"

/**
  *@author Juan Linietsky
  */
/******************************
 saver_it.h
 --------

Impulse Tracker Module Saving..
it seems to work, but modules
saved with this crash xmp...
********************************/





class Saver_IT : public Saver {

	Uint32 sample_offsets[MAX_SAMPLES];
	Uint32 pattern_offsets[MAX_PATTERNS];
	Uint32 instrument_offsets[MAX_PATTERNS];

	int sample_count;
	int pattern_count;
	int instrument_count;
	int orders_count;
        int message_length;

	int message_offset;

	void calculate_values();
	void write_header();
	void write_sample(int p_sample_index);
	void write_sample_internal(int p_sample_index,bool p_write_data);
	void write_instrument(int p_instrument_index);
 	void write_instrument_internal(Instrument *p_aux_instr);
	void write_orders();
	void write_message();
        void write_pattern(int p_pattern);
        void write_song();
public:

	int save(const char *p_filename);
	int save_sample(const char *p_filename,int p_sample_index);
	int save_instrument(const char *p_filename,int p_instrument_index);

	Saver_IT();
	~Saver_IT();
};



#endif
