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

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_


#include "sample_defs.h"
#include <vector>
	using std::vector;
/**
 * 
 * Juan Linietsky
 **/
class Ring_Buffer {

	vector<sample_t> buffer;

	int buffer_mask;
	int buffer_begin;
	
	void write_zero(int p_amount);
	
	int data_remaining;

public:

	sample_t get_sample(int p_index);
	//will anyway resize to a 2^x buffersize for speed
	void request_buffer_size(int p_buffsize);
	
	void mix_from_buffer(int p_amount,const sample_t *p_src);
	void paste_from_buffer(int p_amount,const sample_t *p_src);
	
	void mix_to_buffer(int p_amount,sample_t *p_dst, bool p_advance=true);
	void paste_to_buffer(int p_amount,sample_t *p_dst, bool p_advance=true);
	void advance(int p_amount);
	    
	bool is_there_data_to_mix();
	int get_data_remaining();
	
  Ring_Buffer();
  ~Ring_Buffer();
};

#endif
