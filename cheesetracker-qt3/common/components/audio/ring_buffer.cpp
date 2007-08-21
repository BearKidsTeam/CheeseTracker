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

#include "ring_buffer.h"
#include <stdio.h>

//there are not magic numbers, if they are then computing is full of'em!

void Ring_Buffer::request_buffer_size(int p_buffsize) {


	if (p_buffsize<=0) return;
	int i;
	for (i=0;i<31;i++) {
	
	 	int maskd=(1<<i)-1;
		maskd=~maskd; //invert bits!
		if ( (p_buffsize&maskd) ==0 ) { //found a buffer that contains all
		
			break;
		}
	}
	
	if (i>26) return; // we dont wanna not too big buffers
	
	buffer.resize(1<<i);
	buffer_mask=(1<<i) - 1;
	buffer_begin=0;
	
	write_zero(buffer.size()); //zero it
	
	//printf("ASKED FOR %i, I CONFIGURED TO %i\n",p_buffsize,buffer.size());
}

void Ring_Buffer::mix_from_buffer(int p_amount,const sample_t *p_src) {

	sample_t * auxbuff=&buffer[0];
	int auxidx=buffer_begin;


	if (p_amount>data_remaining) {
	
		data_remaining=p_amount;
	}		
		
 	while (p_amount--) {
	
		auxbuff[auxidx&buffer_mask]+=*(p_src++);
		auxidx++;
	}

}

void Ring_Buffer::paste_from_buffer(int p_amount,const sample_t *p_src) {
	
	sample_t * auxbuff=&buffer[0];
	int auxidx=buffer_begin;
	
	if (p_amount>data_remaining) {
	
		data_remaining=p_amount;
	}		
	
 	while (p_amount--) {
	
		auxbuff[auxidx&buffer_mask]=*(p_src++);
		auxidx++;
	}
}

void Ring_Buffer::mix_to_buffer(int p_amount,sample_t *p_dst, bool p_advance) {


	sample_t * auxbuff=&buffer[0];
	int auxidx=buffer_begin;
	int to_mix=p_amount;
 	while (to_mix--) {
	
		*p_dst++ += auxbuff[auxidx&buffer_mask];
		auxidx++;
	}
	
	if (p_advance)
		advance(p_amount);

		
		
}

void Ring_Buffer::paste_to_buffer(int p_amount,sample_t *p_dst, bool p_advance) {

	sample_t * auxbuff=&buffer[0];
	int auxidx=buffer_begin;
	int to_mix=p_amount;
	
 	while (to_mix--) {
	
		*p_dst++ = auxbuff[auxidx&buffer_mask];
		auxidx++;
	}

	if (p_advance)
		advance(p_amount);
		
		
	
}

void Ring_Buffer::write_zero(int p_amount) {
	
	//speed matters you see
	sample_t * auxbuff=&buffer[0];
	int auxidx=buffer_begin;
	
 	while (p_amount--) {
	
		auxbuff[auxidx&buffer_mask]=0;
		auxidx++;
	}
}


void Ring_Buffer::advance(int p_amount) {

	write_zero(p_amount);
	buffer_begin+=p_amount;
	buffer_begin&=buffer_mask;
	data_remaining-=p_amount;
}

bool Ring_Buffer::is_there_data_to_mix() {

	return (data_remaining>0);
}

int Ring_Buffer::get_data_remaining() {

 	return ((data_remaining>0)?data_remaining:0);
}

sample_t Ring_Buffer::get_sample(int p_index) {

 	return buffer[(buffer_begin+p_index)&buffer_mask];

}

Ring_Buffer::Ring_Buffer()

{
	buffer_begin=0;
	buffer_mask=0;
	data_remaining=0;
}

Ring_Buffer::~Ring_Buffer()
{
}
