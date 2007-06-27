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

#include "loader_raw.h"
#include "ns_autoptr.h"
#include <iostream>
#include <cstdio>


int Loader_RAW::load_sample_func(const char *p_filename, Sample_Data *SD) {
	int status;

	if( (status = file_read.open(p_filename)) ) {
		fprintf(stderr, "Loader_RAW::load_sample_func:%d ", __LINE__);
		perror(p_filename);
		return -1;
	}

	SD->set_c5_freq(8363);

	size_t size = file_read.get_size();
	Uint8 *data = new Uint8[size];
	SD->set_num_channels(1);
	SD->set_size(size);

	if(data == NULL)
	{
		fprintf(stderr, "Loader_RAW::load_sample_func: Out of memory\n");
		file_read.close();
		return -1;
	}

	// Load the data.

	file_read.get_byte_array(data, size);

	// Copy the data into the Sample_Data structure.

	Mutex_Lock_Container *lock = SD->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_lock;
	ns_lock.ptr_new(lock);
	SD->seek(0);
	sample_int_t buffer;
	for(size_t ix=0; ix<size; ix++) {
		buffer=CONVERT_FROM_TYPE(Sint8, data[ix]);
		SD->put_sample(&buffer);
	}

	delete[] data;
	SD->change_sign();

	fprintf(stderr, "Loader_RAW::load_sample_func: Success! SD = %p\n", SD);
	file_read.close();
	return 0;
}

Sample_Data *Loader_RAW::load_sample(const char *p_filename)
{
	Sample_Data *SD = new Sample_Data;
	int status;

	status = load_sample_func(p_filename, SD);
	if(status == -1)
	{
		delete SD;
		fprintf(stderr, "RAW loader failed\n");
		return NULL;
	}
	return SD;
}

Loader::Error Loader_RAW::load_sample(const char *p_filename, int p_dest_index)
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


void Loader_RAW::free_info(bool free_sampledata) {

   ERROR("free_info: unexpected method invocation");
};


Loader_RAW::Loader_RAW() {
	format_name = "Raw";
}

Loader_RAW::~Loader_RAW() {
};

void Loader_RAW::transfer_data_to_song() {

   ERROR("transfer_data_to_song: unexpected method invocation");
};



bool Loader_RAW::test(const char *p_filename) {

   return false;
};

// the following methods shouldn't be called since this is just a sample loader
// (test returns false letting the tracker know that we don't load songs)

Loader::Error Loader_RAW::load(const char *p_filename,bool p_load_patterns) {

   ERROR("load: unexpected method invocation");
   return FILE_FORMAT_NOT_RECOGNIZED; // not valid to load a song file as a sample
};

int Loader_RAW::get_amount_of_samples() {

   ERROR("get_amount_of_samples: unexpected method invocation");
   return 0;
};

Sample_Data *Loader_RAW::get_sample_data(int p_sample_index) {

   ERROR("get_sample_data: unexpected method invocation");
   return NULL;
};

string Loader_RAW::get_sample_name(int p_sample_index) {

   ERROR("get_sample_name: unexpected method invocation");
   return "";
};

void Loader_RAW::add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument) {

   ERROR("add_sample_to_song: unexpected method invocation");
};

Loader::Error Loader_RAW::load_samples_from_instrument(const char *p_filename)
{

   ERROR("load_samples_from_instrument: unexpected method invocation");
   return FILE_ERROR;
};

Loader::Error Loader_RAW::load_instrument(const char *p_filename,int p_dest_index) {

   ERROR("load_instrument: unexpected method invocation");
   return FILE_ERROR;
};


