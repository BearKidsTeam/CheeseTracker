/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://cheesetracker.sf.net/
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

#include "loader_libaudiofile.h"
#include "ns_autoptr.h"
#include <iostream>
#include <cstdio>
#include <audiofile.h>

// load_sample_func - Opens the file "p_filename" and populates the
//                    provided Sample_Data structure "SD" with data
//                    from the file.

int Loader_libaudiofile::load_sample_func(const char *p_filename, Sample_Data *SD)
{
	AFfilesetup file_setup = afNewFileSetup();

	afFreeFileSetup(file_setup);
}

Sample_Data *Loader_libaudiofile::load_sample(const char *p_filename)
{
	Sample_Data *SD = new Sample_Data;
	int status;

	status = load_sample_func(p_filename, SD);
	if(status == -1)
	{
		delete SD;
		fprintf(stderr, "libaudiofile loader failed\n");
		return NULL;
	}
	return SD;
}

Loader::Error Loader_libaudiofile::load_sample(const char *p_filename, int p_dest_index)
{
	Sample *sample = song->get_sample(p_dest_index);
	int status;

	sample->reset();
	sample->name = basename(p_filename);

	sample->in_use = true;
	status = load_sample_func(p_filename, &sample->data);

	if(status == -1)
		return FILE_ERROR;

	return SUCCESS;
}


void Loader_libaudiofile::free_info(bool free_sampledata) {

   ERROR("free_info: unexpected method invocation");
};


Loader_libaudiofile::Loader_libaudiofile() {
	format_name = "Raw";
}

Loader_libaudiofile::~Loader_libaudiofile() {
};

void Loader_libaudiofile::transfer_data_to_song() {

   ERROR("transfer_data_to_song: unexpected method invocation");
};



bool Loader_libaudiofile::test(const char *p_filename) {

   return false;
};

// the following methods shouldn't be called since this is just a sample loader
// (test returns false letting the tracker know that we don't load songs)

Loader::Error Loader_libaudiofile::load(const char *p_filename,bool p_load_patterns) {

   ERROR("load: unexpected method invocation");
   return FILE_FORMAT_NOT_RECOGNIZED; // not valid to load a song file as a sample
};

int Loader_libaudiofile::get_amount_of_samples() {

   ERROR("get_amount_of_samples: unexpected method invocation");
   return 0;
};

Sample_Data *Loader_libaudiofile::get_sample_data(int p_sample_index) {

   ERROR("get_sample_data: unexpected method invocation");
   return NULL;
};

string Loader_libaudiofile::get_sample_name(int p_sample_index) {

   ERROR("get_sample_name: unexpected method invocation");
   return "";
};

void Loader_libaudiofile::add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument) {

   ERROR("add_sample_to_song: unexpected method invocation");
};

Loader::Error Loader_libaudiofile::load_samples_from_instrument(const char *p_filename)
{

   ERROR("load_samples_from_instrument: unexpected method invocation");
   return FILE_ERROR;
};

Loader::Error Loader_libaudiofile::load_instrument(const char *p_filename,int p_dest_index) {

   ERROR("load_instrument: unexpected method invocation");
   return FILE_ERROR;
};


