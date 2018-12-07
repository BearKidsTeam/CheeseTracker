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
                          sample_data.h  -  description
                             -------------------
    begin                : Mon Feb 5 2001
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

#ifndef SAMPLE_DATA_H
#define SAMPLE_DATA_H

#include "typedefs.h"
#include "tables.h"

/**Sample Data.
All data needed to modify a Sample

  *@author Juan Linietsky
  */


/******************************
 sample_data.h
 --------

This contains the real "data"
of a sample. It is used both by the
song and the player.

Also contains some useful editing functions.
that will make your life easier
********************************/





class Sample_Data {

	int get_data_value(int p_pos);
	void put_data_value(int p_pos,int p_val);
public:

	/* loop flags */

	bool loop_on;
	bool sustain_loop_on;

	/* loop data */
	Uint32 loop_begin;
	Uint32 loop_end;
	bool pingpong_loop;

	/* sustain loop data */
	Uint32 sustain_loop_begin;
	Uint32 sustain_loop_end;
	bool sustain_pingpong_loop;

	/* format Data */
	bool is_16bits;

       	int finetune;
	int note_offset;

	Sint32 size; // in samples, not bytes.
	Sint16 *data_ptr;

        void reverse();
        void post_loop_cut();
        void pre_loop_cut();
	void amplify(int p_amount);
	int get_value_to_normalize();
	void change_sign();
	void toggle_quality();
	void resize(int p_new_size);
	void resize_interpolated(int p_new_size);
	void centralize();

        void import_frequency(int p_freq);
	int export_frequency();
	
	Sample_Data();
	~Sample_Data();
};

#endif
