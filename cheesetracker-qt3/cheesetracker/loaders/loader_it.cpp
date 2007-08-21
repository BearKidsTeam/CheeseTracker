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
                          loader_it.cpp  -  description
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

#include "loader_it.h"




const char * Loader_IT::IT_Version[]={

	"ImpulseTracker  .  ",
	"Compressed ImpulseTracker  .  ",
	"ImpulseTracker 2.14p3",
	"Compressed ImpulseTracker 2.14p3",
	"ImpulseTracker 2.14p4",
	"Compressed ImpulseTracker 2.14p4",
};



Loader_IT::Loader_IT(){


	// char arrays to cstrings

	header.identifier[4]=0;
	header.songname[26]=0;
	format_name="Impulse Tracker";

}

Loader_IT::~Loader_IT(){
}



void Loader_IT::clear_structs(bool p_clear_shared_data) {

	int i;

	for (i=0;i<sample_count;i++) {

		delete sample[i];
	}

	free(sample);

	for (i=0;i<instrument_count;i++) {

		delete instrument[i];
	}

	free(instrument);
	
	for (i=0;i<pattern_count;i++) {
	
		delete pattern[i];
	}

	free(pattern);

        message="";
	sample_count=0;
	instrument_count=0;
	pattern_count=0;
}


Loader::Error Loader_IT::give_up_and_clear_structs(Error p_reason) {

	clear_structs(true);

	return p_reason;
}



Loader::Error Loader_IT::load_body() {

	Error aux_result;

	sample_count=0;
	instrument_count=0;
	pattern_count=0;
	printf("Loading samples..\n");
	if ( (aux_result=load_samples()) )
		return give_up_and_clear_structs(aux_result);
	printf("Loading orders..\n");
	if ( (aux_result=load_orders()) ) return
		give_up_and_clear_structs(aux_result);
	printf("Loading instruments..\n");
	if ( (aux_result=load_instruments()) )
		return give_up_and_clear_structs(aux_result);
	printf("Loading sample_data..\n");
	if ( (aux_result=load_sampledata()) )
		return give_up_and_clear_structs(aux_result);
	printf("Loading message..\n");
	if ( (aux_result=load_message()) )
		return give_up_and_clear_structs(aux_result);
	printf("Loading patterns..\n");
	if ( (aux_result=load_patterns()) )
		return give_up_and_clear_structs(aux_result);
	printf("success!..\n");

	return SUCCESS;
}

/*************************************************************
   O V E R R I D E N    F U N C T I O N S
**************************************************************/

void Loader_IT::transfer_data_to_song() {

	int i;

	// Header

       	// static variables

	if (song==NULL) {

		ERROR("Transfer data failed, Song is not set!");
		return;
	}

	transfer_info();


	for (i=0;i<sample_count;i++) {

		transfer_sample_to_song(sample[i],i);
	}

	if (!(header.flags & 4)) // doesnt use instruments

		for (i=0;i<sample_count;i++) {

        		song->make_instrument_out_of_sample_at(i,i);
		}

	 else
		for (i=0;i<instrument_count;i++) {

		transfer_instrument_to_song(instrument[i],i);
		}

	//patterndata


	for (i=0;i<pattern_count;i++) {

		int j,k;

		song->get_pattern(i)->set_length(pattern[i]->get_length());

		for (j=0;j<64;j++) {

			for (k=0;k<pattern[i]->get_length();k++) {

				song->get_pattern(i)->get_note_ref(j,k)=pattern[i]->get_note(j,k);
			}
		}
	}

	song->variables.filename=aux_filename;
}

Loader::Error Loader_IT::load(const char *p_filename,bool p_load_patterns) {

	Loader::Error aux_result;

	if ( file_read.is_open() ) return LOADER_IN_USE;
	if ( file_read.open(p_filename) ) return CANNOT_OPEN_FILE;

	if ( (aux_result=load_header()) ) return abort_load(aux_result);

	if ( (aux_result=load_body()) ) return abort_load(aux_result);

	file_read.close();

	aux_filename=p_filename;

	return SUCCESS;
}

bool Loader_IT::test(const char *p_filename) {

	int aux_err;

	if ( file_read.is_open() ) return LOADER_IN_USE;
	if ( file_read.open(p_filename) ) return CANNOT_OPEN_FILE;

	aux_err=load_header();

	file_read.close();

	return aux_err==SUCCESS;

}

void Loader_IT::free_info(bool free_sampledata) {

	clear_structs(free_sampledata);
}


