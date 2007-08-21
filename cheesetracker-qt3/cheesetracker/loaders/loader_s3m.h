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
                          loader_s3m.h  -  description
                             -------------------
    begin                : Tue Apr 24 2001
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

#ifndef LOADER_S3M_H
#define LOADER_S3M_H

#include "loader.h"

/**
  *@author Juan Linietsky
  */
/******************************
 loader_s3m.h
 ----------
Scream Tracker Module Loader!
It lacks support for
individual sample loading
and reorganizing the columns.
********************************/




class Loader_S3M : public Loader  {

	struct S3M_Header {
	        char  songname[28];
	        Uint8 t1a;
	        Uint8 type;
	        Uint8 unused1[2];
	        Uint16 ordnum;
	        Uint16 insnum;
	        Uint16 patnum;
	        Uint16 flags;
	        Uint16 tracker;
	        Uint16 fileformat;
	        char  scrm[5];
	        Uint8 mastervol;
	        Uint8 initspeed;
	        Uint8 inittempo;
	        Uint8 mastermult;
	        Uint8 ultraclick;
	        Uint8 pantable;
	        Uint8 unused2[8];
	        Uint16 special;
	        Uint8 channels[32];
		Uint8 pannings[32];
	};

	struct S3M_Sample {
	        Uint8 type;
	        char  filename[13];
	        Uint8 memsegh;
	        Uint16 memsegl;
	        Uint8 volume;
	        Uint8 dsk;
	        Uint8 pack;
	        Uint8 flags;
	        Uint32 c2spd;
	        Uint8 unused[12];
	        char  sampname[29];
	        char  scrs[5];
		Sample_Data data;

	};

        int sample_parapointers[MAX_SAMPLES];
        int pattern_parapointers[MAX_PATTERNS];

	int load_header();
	int load_sample(S3M_Sample *p_sample);
	int load_pattern(Pattern *p_pattern);
        int load_patterns();

	void transfer_sample_to_song(S3M_Sample *p_sample,int p_dest_index,bool create_instrument);

        int load_sample_data(S3M_Sample *p_sample);
	int load_samples();
	
	S3M_Header header;
	S3M_Sample **sample;
	Pattern **pattern;
        Uint8 orderlist[500];
        int sample_count;
	int pattern_count;
public:

	Error load(const char *p_filename,bool p_load_patterns);
	bool test(const char *p_filename);


	/* Compute Sample Info */
	int get_amount_of_samples();
	Sample_Data *get_sample_data(int p_sample_index);
	string get_sample_name(int p_sample_index);
	void add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument);
	Error load_sample(const char *p_filename,int p_dest_index);
        Error load_samples_from_instrument(const char *p_filename);

	/* Compute Instrument Info */
	Error load_instrument(const char *p_filename,int p_dest_index);

	void transfer_data_to_song();

	void free_info(bool free_sampledata=false);
	Loader_S3M();
	~Loader_S3M();
};



#endif
