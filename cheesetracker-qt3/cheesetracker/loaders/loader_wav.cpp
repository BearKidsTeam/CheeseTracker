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

#include "loader_wav.h"
#include "ns_autoptr.h"

#include <iostream>

struct format_chunk
{

   Uint16 channels;
   Uint32 sample_rate;
   Uint16 bytes_per_sample;
   Uint16 bits_per_sample;
};

struct data_chunk
{

   Uint32 length;
};

struct smpl_chunk
{

   Uint32 rate;
};

struct multi_chunk
{

   multi_chunk() : has_riff(0), has_format(0), has_data(0), has_smpl(0) {};

   bool has_riff, has_format, has_data, has_smpl;

   format_chunk format;
   data_chunk data;
   smpl_chunk smpl; // not implemented yet***
};

// a little mac to make the code a bit cleaner <-- I don't know what the fuck this comment means.
static int load_chunk( char chunk_id[4],File_Reader& file_read,
                       multi_chunk& Dest, Sample_Data *SD )
{

   Mutex_Lock_Container *SD_lock = SD->lock(__FILE__, __LINE__);
   ns_autoptr<Mutex_Lock_Container> ns_SD_lock;
   ns_SD_lock.ptr_new(SD_lock);

   // check for the types of chunks we know
   if( !memcmp( chunk_id, "RIFF", 4 ) ) {

	//printf("riff at %i\n",file_read.get_file_pos());

      if( Dest.has_riff ) {

	// Ignore all but the first RIFF header.
         return 0;
      };

      /*int length=*/file_read.get_dword();

      char WAVE[4];

      file_read.get_byte_array((Uint8*)WAVE,4);

      if( memcmp( WAVE, "WAVE", 4 ) )
         THROWF(File_Corrupt, file_read.get_filename(), "load chunk ERRORing: WAVE != WAVE, this sample might be from another universe, please zip it and send 10 times to godless@users.sf.net", NULL);


      Dest.has_riff = 1;
   }
   else if( !memcmp( chunk_id, "fmt ", 4 ) ) {

	//printf("format at %i\n",file_read.get_file_pos());

      if( Dest.has_format ) {

         ERROR("load chunk ERRORing: file has already a format chunk, ignoring");
         return 0;
      };

      int length;
      length=file_read.get_dword();

      if( length != 0x10 ) THROWF(File_Corrupt, file_read.get_filename(), "load chunk fatal: format chunk length ain't right", NULL);

      Uint16 compression_code;
      compression_code=file_read.get_word();

      if(compression_code != 1) {
	  THROWF(File_Corrupt, file_read.get_filename(), "load chunk ERROR: Compressed samples are not supported", NULL);
          return 0;
      }

      Dest.format.channels=file_read.get_word();
      Dest.format.sample_rate=file_read.get_dword();

      int avg_bytes_per_second; // ignored
      avg_bytes_per_second=file_read.get_dword();
      Dest.format.bytes_per_sample=file_read.get_word();

      // bytes_per_sample will actually contain
      // bytes per sample per channel.

      if( Dest.format.channels > 1 )
	      Dest.format.bytes_per_sample/=Dest.format.channels;

      Dest.format.bits_per_sample=file_read.get_word();

      Dest.has_format = 1;
   }
   else if( !memcmp( chunk_id, "data", 4 ) ) {


      if( Dest.has_data ) {

         ERROR("load chunk ERRORing: file has more than one data chunks, ignoring secondary ones.");
         return 0;
      };

      if( !Dest.has_format ) {
         WARN("load chunk fatal: format chunk not defined before data chunk, assuming 1 byte per sample");
	Dest.data.length=1;
	Dest.has_format=true;
      }

      Dest.has_data = 1;
      Dest.data.length=file_read.get_dword();

      // Allocate the channels.

      SD->set_num_channels(Dest.format.channels);
      SD->set_size(Dest.data.length/Dest.format.channels/Dest.format.bytes_per_sample);
      SD->seek(0);

      size_t jx_start=0;
      if(Dest.format.bytes_per_sample > sizeof(sample_int_t)) {
	jx_start = Dest.format.bytes_per_sample - sizeof(sample_int_t);
      }

      // Read little-endian, two's complement samples of whatever size, from
      // whatever number of channels. Internally, all samples in CheeseTracker
      // are of type sample_int_t.
      //

      sample_int_t *buffer;
      ns_autoptr<sample_int_t> ns_buffer;
      buffer = new sample_int_t[Dest.format.channels];
      ns_buffer.arr_new(buffer);	// Ensure buffer's deletion at end of scope

      for(size_t ix=0; ix<SD->get_size(); ix++) {
	      for(size_t chan=0; chan < Dest.format.channels; chan++)
	      {

		  // FIXME: Lots of two's complement assumptions here.
		  // Absolutely necessary to be able to load signed samples
		  // of any size into a buffer designed to hold samples
		  // of only one size. Also required if we're gonna be
		  // able to put 8-bit unsigned data into the same buffer.
		  //
		  // One good solution would be to define sample_int_t as
		  // unsigned. This would currently break lots of code
		  // that depends on this data type being signed. It would
		  // be easy, portable bitwork to get the signed, two's
		  // complement data of a WAV file to fit properly in an
		  // unsigned register. In fact, the code already does the
		  // reverse operation when handling 8-bit data.

		  // Zero-initialize before ORing in the data. {tmp}
		  // represents one channel of a sample. Each byte from
		  // the WAV will be ORed and bit-shifted into the appropriate
		  // location in {tmp}, so we do not have to consider the
		  // byte-order in which {tmp} is stored.

		  sample_int_t tmp = 0;


		  // Sun Apr  8 05:07:02 EDT 2007
		  // Gracefully degrade samples of higher quality than 
		  // the number of bits in sample_int_t by dropping the
		  // less significant bytes. 

		  // In prior versions of CheeseTracker, attempting to
		  // load WAV samples with sample sizes that weren't exactly
		  // 1 or 2 bytes caused an error.

		  size_t jx;
		  for(jx=0; jx<jx_start; jx++) {
			file_read.get_byte();
		  }

		  // The cast to sample_int_t is necessary because otherwise the bit-shift is
		  // performed in a 32-bit register on some systems. It was once believed that
		  // sample_int_t might one day be a 64-bit number, but it has since been
		  // discovered that the resampler code (in sample_data.cpp) requires that
		  // there be a type mix_t that is wider than sample_int_t. 

		  for(jx=jx_start; jx < Dest.format.bytes_per_sample; jx++) {
		     tmp |= (sample_int_t)file_read.get_byte() << jx*BITS_PER_BYTE;
		  }
		  // Bit-shift smaller samples into the top of the register.
		  if(Dest.format.bytes_per_sample < sizeof(sample_int_t) ) {
		     tmp <<= (((sizeof(sample_int_t)-Dest.format.bytes_per_sample)-1)*BITS_PER_BYTE);
		  }
		  // 8-bit hack: the WAV standard requires that, while
		  // all other sample sizes be interpreted as signed,
		  // 8-bit (1 byte) samples are considered UNsigned.
		  // Since we're using a signed buffer and shifting small
		  // samples into the top of that buffer, we have to do
		  // something to correct the 8-bit data. XORing the top
		  // bit with 1 will make the sample values that were
		  // originally the smallest still be the smallest in a larger
		  // register.

		  if(Dest.format.bytes_per_sample == 1) {
		     tmp ^= (sample_int_t) 1 << ((sizeof(sample_int_t)*BITS_PER_BYTE)-1);
		  }
	          buffer[chan] = tmp;
	      }
	  SD->put_sample(buffer);
      }

      Dest.has_data = 1;
   }

   else {

      // unknonw chunk type, skip over it
      int skip;

        skip=file_read.get_dword();
        file_read.seek( file_read.get_file_pos() + skip );
   };

   return 0;
};

// Once upon a time there were TWO (very slightly different) versions of this
// function, but now there is only one. The differences are in separate
// functions that call this one.

Loader::Error Loader_WAV::load_sample_func(const char *p_filename, Sample_Data *SD)
{
	// FIXME: try and open the file. If it fails, stupidly
	// discard the system-level error message, which will
	// make it difficult to determine the cause of the problem.
	// But this doesn't really matter, because we don't do anything
	// with the ERROR message or the FILE_ERROR return value.

	if (file_read.open(p_filename)) {
		ERROR("Cannot open file");
		return FILE_ERROR;
	}


	// our detection scheme is simple, we just need to see a riff
	// chunk first thing.

	bool first_chunk_detection_flag = 1;

	// gobble the file up while we still can; putting the chunks into our
	// multichunk collection

	multi_chunk Header;
	do {

		char chunk_id[4];
		file_read.get_byte_array((Uint8*)chunk_id, 4);

		// if this is the first chunk, we'll "detect" the file
		// format by checking if the first chunk id is a RIFF
		// header.

		if( first_chunk_detection_flag ) {

			if( memcmp( chunk_id, "RIFF", 4 ) ) {

				// no worries, it just means we don't know
				// what the heck this is.
				file_read.close();
				ERROR("Not a WAV file");
				return FILE_FORMAT_NOT_RECOGNIZED;
			}

			first_chunk_detection_flag = 0;
		}

		// Sun Apr  8 08:06:22 EDT 2007
		//
		// load_chunk now takes care of populating the SD
		// pointer with data. No more intermediate step of
		// storing the sample data in another buffer first.

		if( load_chunk( chunk_id, file_read,Header, SD ) ) {

			// oop!
			file_read.close();

			ERROR("load sample fatal: RIFF header corrupt.");
			return HEADER_CORRUPT; // probably.. ok well
		};


	}while( ! (Header.has_riff && Header.has_format && Header.has_data) );

	file_read.close();

	SD->set_c5_freq(Header.format.sample_rate);

	return SUCCESS;
}


Sample_Data* Loader_WAV::load_sample(const char *p_filename)
{
	Sample_Data *SD = new Sample_Data;
	Error status;

	status = load_sample_func(p_filename, SD);
	if(status != SUCCESS) {
		delete SD;
		return NULL;
	}

	return SD;
}


Loader::Error Loader_WAV::load_sample(const char *p_filename, int p_dest_index)
{
	Sample *sample = song->get_sample(p_dest_index);
	Loader::Error status;

	fprintf(stderr, "At %s:%d\n", __FILE__, __LINE__);

	sample->reset();
	status = load_sample_func(p_filename, &sample->data);
	
	if(status == SUCCESS) {
		sample->name = basename(p_filename);
		sample->filename = sample->name;
		sample->in_use = true;
	} else {
		fprintf(stderr, "%s:%d: Status is %d\n", __FILE__, __LINE__, 
		        status);
	}

	return status;

}

bool Loader_WAV::test(const char *p_filename)
{

   return false;
};

// the following methods shouldn't be called since this is just a sample loader
// (test returns false letting the tracker know that we don't load songs)

Loader::Error Loader_WAV::load(const char *p_filename,bool p_load_patterns)
{

   ERROR("load: unexpected method invocation");
   return FILE_FORMAT_NOT_RECOGNIZED; // not valid to load a song file as a sample
};

int Loader_WAV::get_amount_of_samples()
{
   ERROR("get_amount_of_samples: unexpected method invocation");
   return 0;
};

Sample_Data *Loader_WAV::get_sample_data(int p_sample_index)
{
   ERROR("get_sample_data: unexpected method invocation");
   return NULL;
};

string Loader_WAV::get_sample_name(int p_sample_index)
{
   ERROR("get_sample_name: unexpected method invocation");
   return "";
};

void Loader_WAV::add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument)
{
   ERROR("add_sample_to_song: unexpected method invocation");
};

Loader::Error Loader_WAV::load_samples_from_instrument(const char *p_filename)
{
   ERROR("load_samples_from_instrument: unexpected method invocation");
   return FILE_ERROR;
};

Loader::Error Loader_WAV::load_instrument(const char *p_filename,int p_dest_index)
{
   ERROR("load_instrument: unexpected method invocation");
   return FILE_ERROR;
};

void Loader_WAV::transfer_data_to_song()
{
   ERROR("transfer_data_to_song: unexpected method invocation");
};

void Loader_WAV::free_info(bool free_sampledata)
{
   ERROR("free_info: unexpected method invocation");
};

Loader_WAV::Loader_WAV()
{
	format_name="Microsoft WAV";

}
Loader_WAV::~Loader_WAV()
{

}

