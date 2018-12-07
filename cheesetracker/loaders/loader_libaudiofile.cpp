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
#include "Error.h"
#include <iostream>
#include <cstdio>
#include <cerrno>
#include <audiofile.h>

extern "C" {
	char _loader_audiofile_error[2048];
	long _loader_audiofile_errnum;
	void af_error_handler(long errnum, const char *what) {
		_loader_audiofile_errnum = errnum;
		strncpy(_loader_audiofile_error, what, 2047);
	}
}

class set_error_handler
{
	public:
	set_error_handler() {
		afSetErrorHandler(af_error_handler);
	}
}error_handler;

// load_sample_func - Opens the file "p_filename" and populates the
//                    provided Sample_Data structure "SD" with data
//                    from the file.

int Loader_libaudiofile::load_sample_func(const char *p_filename, Sample_Data *SD)
{
	int status;
	int num_channels;
	int frames_read;
	Uint8 *buffer;
	ns_autoptr<Uint8> ns_buffer;
	sample_int_t *one_frame;
	ns_autoptr<sample_int_t> ns_one_frame;
	AFfilehandle file_handle = afOpenFile(p_filename, "r", NULL);
	AFframecount file_size;

	if(file_handle == AF_NULL_FILEHANDLE) goto throwOpenError;


	file_size = afGetFrameCount(file_handle, AF_DEFAULT_TRACK);
	if(file_size == -1) goto throwAFError;

	SD->set_size(file_size);
	num_channels = afGetVirtualChannels(file_handle, AF_DEFAULT_TRACK);
	if(num_channels == -1) goto throwAFError;

	SD->set_num_channels(num_channels);
	status = afSetVirtualSampleFormat(file_handle, AF_DEFAULT_TRACK,
	                                      AF_SAMPFMT_TWOSCOMP,
	                                      sizeof(sample_int_t) * BITS_PER_BYTE);
	if(status == -1) goto throwAFError;

	// This is done because otherwise, I have no idea what byte order
	// libaudiofile will use. One could assume that it will always
	// convert to host byte order, but I have no documentation on
	// this, and it is not evident from reading the source code.

	status = afSetVirtualByteOrder(file_handle, AF_DEFAULT_TRACK,
	                               AF_BYTEORDER_LITTLEENDIAN);

	COMPILER_ASSERT(BITS_PER_BYTE == 8);
	buffer = new Uint8[file_size*num_channels*sizeof(sample_int_t)];
	ns_buffer.arr_new(buffer);

	frames_read = afReadFrames(file_handle, AF_DEFAULT_TRACK, buffer, file_size);
	if(frames_read == -1) goto throwAFError;

	one_frame = new sample_int_t[num_channels];
	ns_one_frame.arr_new(one_frame);

	// Copy the newly-read data into the Sample_Data buffer one frame
	// at a time.
	SD->seek(0);
	for(int ix=0; ix<file_size; ix++) {
		for(size_t chan=0; chan<num_channels; chan++) {
			// libaudiofile was explicitly told to yield little-endian
			// data, so now it must be converted to the host byte-order.
			one_frame[chan] = buffer[ix*num_channels*sizeof(sample_int_t)+chan*sizeof(sample_int_t)] |
			                  buffer[ix*num_channels*sizeof(sample_int_t)+chan*sizeof(sample_int_t)+1] << BITS_PER_BYTE;
		}
		SD->put_sample(one_frame);
	}

	afCloseFile(file_handle);

	return 0;
throwAFError:
	afCloseFile(file_handle);
	THROWF(File_Corrupt, p_filename, "%s (Audiofile error number %i)", _loader_audiofile_error, _loader_audiofile_errnum)
throwOpenError:
	{
		THROWF(File_Corrupt, p_filename, "%s (Audiofile error number %i)", _loader_audiofile_error, _loader_audiofile_errnum);
		File_Corrupt E;
		E.set_error_pfx(p_filename);
		E.report_errno(errno);
		throw E;
	}
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


