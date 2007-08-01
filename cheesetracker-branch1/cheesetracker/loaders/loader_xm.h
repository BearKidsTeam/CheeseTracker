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
                          loader_xm.h  -  description
                             -------------------
    begin                : Tue Apr 9 2002
    copyright            : (C) 2002 by red
    email                : red@server
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LOADER_XM_H
#define LOADER_XM_H

#include "loader.h"
#include "song.h"
#include "file_reader.h"
#include <list>
	using std::list;
/**
  *@author red
  */




class Loader_XM : public Loader  {


	struct XM_Header {

   		Uint8 idtext[18];
     		Uint8 songname[21];
       		Uint8 hex1a; // ?
         	Uint8 trackername[21];
          	Uint16 version;
           	Uint32 headersize; //from here
            	
            	Uint16 songlength; //pattern ordertable
             	Uint16 restart_pos;
              	Uint16 channels_used;
                Uint16 patterns_used;
                Uint16 instruments_used;
                Uint16 use_linear_freq;
                Uint16 tempo;
                Uint16 speed;
                Uint8 orderlist[256];

	} header;

 	vector<Sample> samples;
 	list<Instrument> instruments;
 	Pattern patterns[256];
  	
        File_Reader reader;
	Error load_instrument_internal(Instrument *pint,bool p_xi,int p_cpos, int p_hsize, int p_sampnumb=-1);

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

	Loader_XM();
	~Loader_XM();
};



#endif
