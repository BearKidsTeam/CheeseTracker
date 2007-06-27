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
                          saver_raw.cpp  -  description
                             -------------------
    begin                : Thu Jan 24 2002
    copyright            : (C) 2002 by calvin french
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

#include "ns_autoptr.h"
#include "saver_raw.h"





int Saver_Raw::save(const char *p_filename) {


	return UNKNOWN_TYPE;
}
int Saver_Raw::save_sample(const char *p_filename,int p_sample_index) {
	
	string fname=p_filename,ext;
	Sample *smp;
	ext=fname.substr(fname.length()-4,4);
	
	if ((fname.length()<5) || ((fname.substr(fname.length()-4,4)!=".raw") && (fname.substr(fname.length()-4,4)!=".RAW")) ) return UNKNOWN_TYPE;
	
	smp=song->get_sample(p_sample_index);
	
	if (!smp->in_use) return SAVE_ERROR;
	
	
	if (writer.open(p_filename)) return SAVE_ERROR;

	// NOTE: RAW SAMPLES ARE 8 BITS AND MONO
	//
	// Samples that don't match this data type will
	// be silently converted.

	Mutex_Lock_Container *lock = smp->data.lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_lock;
	ns_lock.ptr_new(lock);
	smp->data.seek(0);
	for(size_t ix=0; ix<smp->data.get_size(); ix++) {
		writer.store_byte(CONVERT_TO_TYPE(Sint8, smp->data.get_int_sample()[0]));
	}
	
	writer.close();

	return SAVE_OK;
}

int Saver_Raw::save_instrument(const char *p_filename,int p_instrument_index) {

	return UNKNOWN_TYPE;
}


Saver_Raw::Saver_Raw(){
}
Saver_Raw::~Saver_Raw(){
}

