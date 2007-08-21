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
                          saver_wav.cpp  -
                             -------------------
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "saver_wav.h"
#include "ns_autoptr.h"




int Saver_Wav::save(const char *p_filename) {


	return UNKNOWN_TYPE;
}

#define SAVE_INT(a) { int I = a; writer.store_byte_array( (Uint8*) &I, sizeof(int) ); }
#define SAVE_SHORT(a) { short I = a; writer.store_byte_array( (Uint8*) &I, sizeof(short) ); }


int Saver_Wav::save_sample(const char *p_filename,int p_sample_index) {
	
	string fname=p_filename,ext;
	Sample *smp;
	ext=fname.substr(fname.length()-4,4);
	
	if ( (fname.length()<4) || (fname.substr(fname.length()-4,4)!=".wav") )
		return UNKNOWN_TYPE;
	
	smp=song->get_sample(p_sample_index);
	
	if (!smp->in_use) return SAVE_ERROR;
		
	if (writer.open(p_filename)) return SAVE_ERROR;



   int DataLen = smp->data.get_size() * sizeof(sample_int_t) * smp->data.num_channels();

   int SampleRate = smp->data.get_c5_freq();

   writer.store_byte_array( (Uint8*) "RIFF", 4 );

   writer.store_dword(DataLen + 24 + 4);


   writer.store_byte_array( (Uint8*) "WAVE", 4 );



   writer.store_byte_array( (Uint8*) "fmt ", 4 );
   writer.store_dword(0x10);
   writer.store_word(0x01); // always 0x01 - Uncompressed
   writer.store_word(smp->data.num_channels()); // number of channels
   writer.store_dword(SampleRate); // sample rate
   writer.store_dword(SampleRate*sizeof(sample_int_t)); // avg bytes per second for streaming
   writer.store_word(sizeof(sample_int_t)*smp->data.num_channels()); // bytes per samp
   writer.store_word(sizeof(sample_int_t) * BITS_PER_BYTE); // bits per sample

   writer.store_byte_array( (Uint8*) "data", 4 );
   writer.store_dword(DataLen);



	Mutex_Lock_Container *lock = smp->data.lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_lock;
	ns_lock.ptr_new(lock);
	smp->data.seek(0);

	while(!smp->data.eof_reached()) {
		const sample_int_t *buffer = smp->data.get_int_sample();

		for(size_t chan=0; chan < smp->data.num_channels(); chan++) {
			writer.store_sample(buffer[chan]);
		}
   	}

	writer.close();

   return SAVE_OK;
}

int Saver_Wav::save_instrument(const char *p_filename,int p_instrument_index) {


	return UNKNOWN_TYPE;
}


Saver_Wav::Saver_Wav(){
}
Saver_Wav::~Saver_Wav(){
}

