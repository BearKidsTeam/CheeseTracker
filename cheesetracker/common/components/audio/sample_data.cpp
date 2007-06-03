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
                          sample_data.cpp  -  description
                             -------------------
    begin                : Mon Feb 5 2001
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

#include <algorithm>
#include <cassert>
#include <new>
#include <cmath>
#include "sample_data.h"

#define FIXEDPOINT_INT_PART_BITS 11
#define FRACTIONAL_MASK ((1 << FIXEDPOINT_INT_PART_BITS)-1)
#define COSINE_LEN (1 << FIXEDPOINT_INT_PART_BITS)
#define FIXEDPOINT_FLOOR(number) (number & (~0 << FIXEDPOINT_INT_PART_BITS))
#define ASSERT_NOTFIXEDPOINT(function) {			\
	if(fixedpoint_mode) {					\
		Sample_Readonly_Error E;			\
		E.set_error_pfx(function);			\
		E.set_error("Sample is in fixed-point mode");	\
		throw E;					\
	}							\
}
#define SPLINE_FRACBITS 8
#define SPLINE_QUANTBITS        14
#define SPLINE_QUANTSCALE       (1L<<SPLINE_QUANTBITS)
#define SPLINE_8SHIFT           (SPLINE_QUANTBITS-8)
#define SPLINE_16SHIFT          (SPLINE_QUANTBITS)
// forces coefsset to unity gain
#define SPLINE_CLAMPFORUNITY

int frac_cosine[COSINE_LEN];
int frac_cosine_not_cosine_mode[COSINE_LEN];
sample_int_t cubic_lut[4*(1L<<SPLINE_FRACBITS)];

typedef Sint32 mix_t;
COMPILER_ASSERT(sizeof(mix_t) > sizeof(sample_int_t));

class init_lookup_tables {
	public:
	init_lookup_tables() {
		for (size_t i=0;i<COSINE_LEN;i++) {

			float x=(float)i*M_PI/(float)COSINE_LEN;

			float cosval=cos(x);
			frac_cosine[i]=int( (1.0-cosval) * 0.5 * (float)COSINE_LEN);
			cosval=cos(cos(x)*x);
			frac_cosine_not_cosine_mode[i]=int( (1.0-cosval) * 0.5 * (float)COSINE_LEN);
		}
		int length              = (1L<<SPLINE_FRACBITS);
		float length_f  = 1.0f / (float)length;
		float scale     = (float)SPLINE_QUANTSCALE;
		for(size_t i=0;i<length;i++)
		{       float cm1, c0, c1, c2;
			float x         = ((float)i)*length_f;
			int sum,idx     = i<<2;
			cm1                     = (float)floor( 0.5 + scale * (-0.5*x*x*x + 1.0 * x*x - 0.5 * x       ) );
			c0                      = (float)floor( 0.5 + scale * ( 1.5*x*x*x - 2.5 * x*x             + 1.0 ) );
			c1                      = (float)floor( 0.5 + scale * (-1.5*x*x*x + 2.0 * x*x + 0.5 * x       ) );
			c2                      = (float)floor( 0.5 + scale * ( 0.5*x*x*x - 0.5 * x*x                   ) );
			cubic_lut[idx+0]      = (signed short)( (cm1 < -scale) ? -scale : ((cm1 > scale) ? scale : cm1) );
			cubic_lut[idx+1]      = (signed short)( (c0  < -scale) ? -scale : ((c0  > scale) ? scale : c0 ) );
			cubic_lut[idx+2]      = (signed short)( (c1  < -scale) ? -scale : ((c1  > scale) ? scale : c1 ) );
			cubic_lut[idx+3]      = (signed short)( (c2  < -scale) ? -scale : ((c2  > scale) ? scale : c2 ) );
#ifdef SPLINE_CLAMPFORUNITY
			sum                     = cubic_lut[idx+0]+cubic_lut[idx+1]+cubic_lut[idx+2]+cubic_lut[idx+3];
			if( sum != SPLINE_QUANTSCALE )
			{       int max = idx;
				if( cubic_lut[idx+1]>cubic_lut[max] ) max = idx+1;
				if( cubic_lut[idx+2]>cubic_lut[max] ) max = idx+2;
				if( cubic_lut[idx+3]>cubic_lut[max] ) max = idx+3;
				cubic_lut[max] += (SPLINE_QUANTSCALE-sum);
			}
#endif
		}


	}
} init_lut_object;


void Sample_Data::release_data_ptr() {

	data_ptr=NULL;
	size=0;
	is_16bits=true;
}


void Sample_Data::set_data_ptr(void *p_data,int p_size,bool p_16bits) {

	if(!p_size)
		assert(p_size == 0 && p_data == NULL);

	size=p_size;
	is_16bits=p_16bits;
	if (data_ptr!=NULL)
		free(data_ptr);
	data_ptr=(sample_16s_t*)p_data;

}

void Sample_Data::copy_sample_data(const Sample_Data *p_sdata) {

	ASSERT_NOTFIXEDPOINT("copy_sample_data");

	*this = *p_sdata;

}

void Sample_Data::set_c5_freq(int p_c5_freq) {

	c5_freq=p_c5_freq;
}

Sample_Data::Sample_Data(){

	loop_on=false;
	sustain_loop_on=false;

	loop_begin=0;
	loop_end=0;
	pingpong_loop=false;

	/* sustain loop data */
	sustain_loop_begin=0;
	sustain_loop_end=0;
	sustain_pingpong_loop=false;

	c5_freq=44100;

	/* format Data */
	is_16bits=true;

//       	finetune=0;
//	note_offset=0;

	current_pos=0;
	data_ptr=NULL;
	fixedpoint_mode=false;
	fixedpoint_inc=0;

	fixedpoint_offset=0;
	fixedpoint_backwards=false;

	// FIXME: Change default value to 0 when
	// the rest of the code is ready.
	channels=1;
 	size=0;
}

Sample_Data::~Sample_Data(){
	if(data_ptr)
		free(data_ptr);
}


float Sample_Data::get_sample(size_t p_index)  const{

	if ( (p_index<0) || (p_index>=size)) return 0.0f;

        if (is_16bits)
		return (float)get_data_16()[p_index]/32768.0;
        else
		return (float)get_data_8()[p_index]/128.0;
}

void Sample_Data::set_sample(size_t p_idx,float p_val) {
	ASSERT_NOTFIXEDPOINT("set_sample");

	if ( (p_idx<0) || (p_idx>=size)) return;

	if (p_val>1.0) p_val=1.0;
	if (p_val<(-1.0)) p_val=(-1.0);

        if (is_16bits) {

		p_val*=32768.0;
		if (p_val>32767.0) p_val=32767.0;
		get_data_16()[p_idx]=(int)p_val;
        } else {

		p_val*=128.0;
		if (p_val>127.0) p_val=127.0;
		get_data_8()[p_idx]=(int)p_val;
	}
}

void Sample_Data::set_loop_enabled(bool p_enabled) {

	loop_on=p_enabled;
}

void Sample_Data::set_loop_ping_pong(bool p_enabled) {

	pingpong_loop=p_enabled;

}


void Sample_Data::set_loop_begin(int p_idx) {

	loop_begin=p_idx;
}
void Sample_Data::set_loop_end(int p_idx) {

	loop_end=p_idx;

}

void Sample_Data::change_sign() {
	ASSERT_NOTFIXEDPOINT("change_sign");

	if (data_ptr==NULL) return;

	size_t ix;

	for (ix=0;ix<size;ix++) {


		if (is_16bits) {
			data_ptr[ix] ^= (sample_int_t)1 << (sizeof(sample_int_t)*BITS_PER_BYTE-1);

		} else {
			// FIXME: This assumes certain behavior in the
			// event of an integer overflow.

      			Sint8 *sample_8bits_ptr=(Sint8*)data_ptr;
	                sample_8bits_ptr[ix]+=128;
		}
	}
}


/* Misc internal Editing Utils */
sample_int_t Sample_Data::
get_data_value(size_t chan, size_t p_pos) {

	if (data_ptr==NULL) return 0;

	if (p_pos>size) return 0;

        if (is_16bits) {

		return data_ptr[p_pos];
	} else {

		Sint8 *sample_8bits_ptr=(Sint8*)data_ptr;
		return sample_8bits_ptr[p_pos] << ((sizeof(sample_int_t)-1)*BITS_PER_BYTE);

	}

}
void Sample_Data::put_data_value(size_t chan, size_t p_pos, sample_int_t p_val) {
	ASSERT_NOTFIXEDPOINT("put_data_value");

	if (data_ptr==NULL) return;

	if ((p_pos<0) || (p_pos>size)) return;

        if (is_16bits) {

		data_ptr[p_pos]=p_val;

	} else {

		p_val=p_val >> (BITS_PER_BYTE*sizeof(sample_int_t)-1);

		Sint8 *sample_8bits_ptr=(Sint8*)data_ptr;
		sample_8bits_ptr[p_pos]=p_val;

	}

}

/*
void Sample_Data::reverse() {
	ASSERT_NOTFIXEDPOINT("reverse");

	if (data_ptr==NULL) return;

	size_t i
	sample_int_t aux_val;
	
	for(chan=0; chan<num_channels(); chan++)
	for (i=0;i<size/2;i++) {

		aux_val=get_data_value(chan, i);
		put_data_value(i,get_data_value(chan, (size-i)-1));
		put_data_value(chan, (size-i)-1,aux_val);		
	}

	aux_val=loop_end;	
	loop_end=(size-loop_begin);
	loop_begin=(size-aux_val);

	aux_val=sustain_loop_end;	
	sustain_loop_end=(size-sustain_loop_begin);
	sustain_loop_begin=(size-aux_val);
}

void Sample_Data::post_loop_cut() {
	ASSERT_NOTFIXEDPOINT("post_loop_cut");

	if (data_ptr==NULL) return;


	Uint32 new_size;

	if (!loop_on && !sustain_loop_on) 
		return;
	else if (loop_on && !sustain_loop_on)
		set_size(loop_end);
	else if (!loop_on && sustain_loop_on)
		set_size(sustain_loop_end);
	else 
		set_size((loop_end>sustain_loop_end)?loop_end:sustain_loop_end);

}

void Sample_Data::pre_loop_cut() {
	ASSERT_NOTFIXEDPOINT("pre_loop_cut");

	if (data_ptr==NULL) return;

	reverse();
	post_loop_cut();
	reverse();

}

void Sample_Data::amplify(int p_percent) {
	ASSERT_NOTFIXEDPOINT("amplify");

	if (data_ptr==NULL) return;

	size_t i;

	for (i=0;i<size;i++)
	for(chan=0; chan<num_channels(); chan++)
		put_data_value(chan, i,(get_data_value(i)*p_percent)/100);

}

*/

void Sample_Data::toggle_quality() {
	ASSERT_NOTFIXEDPOINT("toggle_quality");

	if (data_ptr==NULL) return;

	Sint8 *new_sample_8bits;
	Sint16 *new_sample_16bits;
	size_t i;

	if (is_16bits) {

		new_sample_8bits=(Sint8*)malloc(size);
		if (new_sample_8bits==NULL) {

			ERROR("Cannot convert sample! not enough memory?");
			return;
		}
		for (i=0;i<size;i++) new_sample_8bits[i]=data_ptr[i] >> 8;
		free(data_ptr);
		data_ptr=(Sint16*)new_sample_8bits;
		is_16bits=false;

	} else {

		Sint8 *sample_8bits_ptr=(Sint8*)data_ptr;
		new_sample_16bits=(Sint16*)malloc(size*2);
		if (new_sample_16bits==NULL) {

			ERROR("Cannot convert sample! not enough memory?");
			return;
		}

		for (i=0;i<size;i++) new_sample_16bits[i]=(Sint16)sample_8bits_ptr[i] << 8;
		free(data_ptr);
		data_ptr=new_sample_16bits;
		is_16bits=true;
	}


}

// get_int_sample 	- Gets a single sample from the specified channel.
//
// notes		- In fixed-point mode, the position indicator does not move.
//
//                      - In normal mode, the position indicator advances by one sample.
//
// see also		- use_fixedpoint, fixedpoint_set_resample_rate, fixedpoint_aboutface,
//                        seek, fixedpoint_move_cursor

sample_int_t
Sample_Data::get_int_sample(size_t chan) {
	sample_int_t retval;

	if(eof_reached(chan)) {
		Sample_EOF_Error E;
		E.set_error_pfx("get_sample");
		E.set_error("End of sample reached");
		throw E;
	}
	if(is_16bits) {
		retval=data_ptr[current_pos];
	} else {
		sample_8s_t temp;
		temp = ((sample_8s_t*)data_ptr)[current_pos];
		retval=temp;
		retval <<= 8;
	}
	if(!fixedpoint_mode) 
		current_pos++;
	return retval;
}

// sd_realloc - PRIVATE - Reallocate a sample buffer.


void 
Sample_Data::sd_realloc(size_t chan, size_t new_size) {
        sample_int_t *new_data;
	size_t multiplier=sizeof(sample_int_t);

	if(!is_16bits)
		multiplier=sizeof(sample_8s_t);
	
	new_data = (sample_int_t*)malloc(new_size*multiplier);
	if(new_data == NULL) {
		throw std::bad_alloc();
	}
	if(data_ptr) {
		size_t copy_size = std::min<size_t>(new_size, size);
        	memcpy((void*)new_data, (void*)data_ptr, copy_size*multiplier);
	}

	if(new_size > size) {
		// Initialize any newly-allocated data with zero.

		if(is_16bits) {
			memset((void*)(new_data+size), 0,
			       (new_size-size)*sizeof(sample_int_t));
		} else {
			sample_8s_t *ptr = (sample_8s_t*)new_data;
			memset((void*)(ptr+size), 0,
			       (new_size-size)*sizeof(sample_8s_t));
		}
	}

        free(data_ptr);
        data_ptr=new_data;
        size=new_size;
}

// eof_reached - Tell if the position indicator for the current channel
//               points to the end of the buffer (or the beginning, if
//               Fixed Point Mode is in use and the sample is being scanned
//               backwards).

bool
Sample_Data::eof_reached(size_t chan) {
	if(fixedpoint_mode && fixedpoint_backwards) {
			return (get_current_pos(chan) > 0);
	} else {
		return !(get_current_pos(chan) < get_size(chan));
	}
}

// put_sample - Add an integer sample to the specified channel.
//
// notes      - Extends the buffer by one if necessary. Repeated
//              reallocations is inefficient.
//
// see also   - set_size, get_size

void
Sample_Data::put_sample(size_t chan, sample_int_t smp) {
	ASSERT_NOTFIXEDPOINT("put_sample");
	if(chan != 0)
		return;

	if(eof_reached(chan)) 
		sd_realloc(chan, size+1);

	if(is_16bits)
		data_ptr[current_pos] = smp;
	else
		((sample_8s_t*)data_ptr)[current_pos] = (smp >> BITS_PER_BYTE);

	current_pos++;
}

// seek - Set the position indicator of a channel
//
//          - If the size of the channel is less than {new_pos}, then
//            the position indicator will be set to the size of the
//            channel.
//
//          - If not in fixed-point mode, the fixed-point incrementation
//            cache is left alone.
//
// see also - get_size

void
Sample_Data::seek(size_t chan, size_t new_pos) {
	if(chan != 0)
		return;
	if(new_pos > get_size(chan))
		new_pos = get_size(chan);
	current_pos = new_pos;
	if(fixedpoint_mode)
		fixedpoint_offset=0;
}

// get_current_pos - Get the position indicator of the specified channel.

size_t
Sample_Data::get_current_pos(size_t chan) const {
	return current_pos;
}

// get_sample_array - Get an array of integer samples from a channel
//
// notes            - Returns the number of samples actually retrieved,
//                    which may be less than the number requested if
//                    the channel is not long enough.
//
//                  - Ignores fixedpoint_mode. The sample is always scanned
//                    forward, one sample at a time.
//
// see also         - get_size, get_int_sample

size_t
Sample_Data::get_sample_array(size_t chan, sample_int_t *dest, size_t len) {
	size_t ix;


        if(get_current_pos(chan)+len > get_size(chan)) {
                len -= (get_current_pos(chan)+len)-get_size(chan);
                if(len == 0)
                        return 0;
        }

	if(is_16bits) {
        	memcpy((void*)dest, (void*)(data_ptr+current_pos),
               	       len*sizeof(sample_int_t));
		current_pos += len;
	} else {
		for(ix=0; ix<len; ix++, current_pos++) {
			dest[ix] = ((sample_8s_t*)data_ptr)[current_pos];
			dest[ix] <<= BITS_PER_BYTE;
		}
	}
        return len;
}

// put_sample_array - Put an array of integers into the specified channel
//
//                  - Extends the buffer as necessary.

void
Sample_Data::put_sample_array(size_t chan, const sample_int_t *src,
                              size_t len) {
	ASSERT_NOTFIXEDPOINT("put_sample_array");

	if(chan != 0)
		return;

        size_t ix;

        if(len > size-current_pos) {
                sd_realloc(chan, len-get_size(chan)-get_current_pos(chan));
        }

	if(is_16bits) {
		memcpy((void*)(data_ptr+current_pos), src,
		       len*sizeof(sample_int_t));
		current_pos+=len;
	} else {
		sample_8s_t *ptr = (sample_8s_t*)data_ptr;

		for(ix=0; ix<len; ix++,current_pos++) {
			ptr[current_pos] = (src[ix] >> BITS_PER_BYTE);
		}

	}

}

// truncate - Truncate the specified channel
//
//          - Everything after channel {chan}'s position indicator
//            is freed. 
//
//          - Reallocates memory.

void
Sample_Data::truncate(size_t chan) {
	ASSERT_NOTFIXEDPOINT("truncate");
	if(current_pos==0) {
		free(data_ptr);
		data_ptr=NULL;
		size=0;
		return;
	}
	if(eof_reached(chan)) {
		return;
	}
	sample_int_t *new_data;
	size_t multiplier=sizeof(sample_int_t);

	if(!is_16bits) {
		multiplier=sizeof(sample_8s_t);
	}

	new_data=(sample_int_t*)malloc(current_pos*multiplier);
	memcpy((void*)new_data, (void*)data_ptr, current_pos*multiplier);
	size=current_pos;
	free(data_ptr);
	data_ptr=new_data;
	correct_loop_pointers();
}

// get_f_sample - Get a sample from the specified channel
//
// notes	- Handles end-of-sample differently in
//                fixedpoint_mode than in normal mode.
//
//                In normal mode, the last sample is
//                returned continuously after eof_reached(),
//                while in fixedpoint_mode, an exception is
//                thrown.

float Sample_Data::get_f_sample(size_t chan) {
	if(eof_reached(chan)) {
		return 0.0f;
	}

	float retval;

        if (is_16bits)
		retval=(float)data_ptr[current_pos]/32768.0;
        else
		retval=(float)((sample_8s_t*)data_ptr)[current_pos]/128.0;
	if(!fixedpoint_mode) {
		current_pos++; 
		if(current_pos>size)
			current_pos=size;
	}
	return retval;
}

// put_f_sample - Add a floating-point sample to the specified channel.
//
// notes        - Reallocating one byte at a time is inefficient.
//
// see also     - set_size

void Sample_Data::put_f_sample(size_t chan, float p_val) {
	ASSERT_NOTFIXEDPOINT("put_f_sample");

	if(eof_reached(chan))
		sd_realloc(chan, size+1);

	if (p_val>1.0) p_val=1.0;
	if (p_val<(-1.0)) p_val=(-1.0);

        if (is_16bits) {

		p_val*=32768.0;
		if (p_val>32767.0) p_val=32767.0;
		data_ptr[current_pos]=(sample_int_t)p_val;
        } else {

		p_val*=128.0;
		if (p_val>127.0) p_val=127.0;
		((sample_8s_t*)data_ptr)[current_pos]=(int)p_val;
	}
	current_pos++;
}

// set_size - Grow or shrink the sample data buffer for the specified
//            channel.
//
//          - Reallocates memory - Should not be used in combination
//            with set_data_ptr() or get_data_8/16().
//
//          - Moves the position indicator to the new end of the
//            sample if it is greater than {new_size} samples into
//            the buffer.
//
//          - set_size(0) deletes the buffer.

void
Sample_Data::set_size(size_t chan, size_t new_size) {
	ASSERT_NOTFIXEDPOINT("set_size");
	if(chan != 0)
		return;
	if(size == new_size)
		return;
	if(new_size == 0) {
		seek(chan, 0);
		truncate(chan);
	} else {
		sd_realloc(chan, new_size);
	}
	seek(chan, current_pos);
	correct_loop_pointers();
}

// num_channels - Return the number of channels contained in the sample.

size_t
Sample_Data::num_channels() const {
	return channels;
}

// alloc_channels - Set the number of channels for this sample.
// 
//                - Allocates or frees buffers to make the sample
//                  have the desired number of samples.
//
//                - Newly allocated channels will have zero-length
//                  buffers.
//
// see also	  - set_size

void
Sample_Data::alloc_channels(size_t num) {
	ASSERT_NOTFIXEDPOINT("alloc_channels");
	// Not implemented.
	return;
}

bool
Sample_Data::is_empty() {
	return(data_ptr == NULL || size == 0);
}

const Sample_Data &
Sample_Data::operator=(const Sample_Data &r_data) {
	ASSERT_NOTFIXEDPOINT("operator=");
	size_t chan_ix;
	alloc_channels(r_data.num_channels());

	// Copy all sample buffers

	for(chan_ix = 0; chan_ix < r_data.num_channels(); chan_ix++) {
		seek(chan_ix, 0);
		set_size(chan_ix, r_data.get_size(chan_ix));

		// FIXME: When multiple channels are actually
		// implemented, this will need to be changed.

		fixedpoint_offset	= r_data.fixedpoint_offset;

		for(size_t jx=0; jx<r_data.get_size(chan_ix); jx++) {
			if(r_data.is_16bit())
				put_sample(chan_ix,
				           ((Uint16*)r_data.data_ptr)[jx]);
			else
				put_sample(chan_ix,
				           ((Uint8*)r_data.data_ptr)[jx]);
		}
		seek(chan_ix, 0);
	}

	// Copy miscellaneous information
	
	set_c5_freq(r_data.get_c5_freq());
	set_loop_enabled(r_data.is_loop_enabled());
	set_loop_ping_pong(r_data.is_loop_ping_pong());
	set_loop_begin(r_data.get_loop_begin());
	set_loop_end(r_data.get_loop_end());

	// Sustain loop data (no interface implemented)

	sustain_loop_on		= r_data.sustain_loop_on;
	sustain_loop_begin	= r_data.sustain_loop_begin;
	sustain_loop_end	= r_data.sustain_loop_end;
	sustain_pingpong_loop	= r_data.sustain_pingpong_loop;

	fixedpoint_mode		= r_data.fixedpoint_mode;
	fixedpoint_backwards	= r_data.fixedpoint_backwards;
	fixedpoint_inc		= r_data.fixedpoint_inc;

	return *this;
}

// use_fixedpoint - Set whether or not to use the special fixed-point processing mode.
// 
// notes          - Fixed-point mode is a special mode required by the resamplers
//                  found in common/plugins/resamplers/. In fixed-point mode, a
//                  channel's position indicator doesn't move when the read/write
//                  interface is used. This behavior is required because the mixer
//                  may need to mix the same sample into multiple channels.
//
//                  Instead, the position indicator is moved explicitly with
//                  fixedpoint_move_cursor() when all channels at the current position
//                  have been mixed. 
//
//                - When the indicator does move, it moves according to the resample
//                  ratio calculated by fixedpoint_set_resample_rate().
//
//                - Fixed-point mode supports forward and backward cursor movement, whereas
//                  normal mode only supports forward movement.
//
// see also       - fixedpoint_set_resample_rate, fixedpoint_move_cursor, fixedpoint_aboutface,
//                  fixedpoint_is_backwards

void
Sample_Data::use_fixedpoint(bool yes_or_no)
{
	fixedpoint_mode=yes_or_no;
}

void
Sample_Data::fixedpoint_set_resample_rate(size_t current_freq, size_t mix_freq, bool backwards)
{
	fixedpoint_inc = (current_freq << FIXEDPOINT_INT_PART_BITS)/mix_freq;
	fixedpoint_backwards = backwards;
}

// fixedpoint_move_cursor - Move the cursor in fixed-point mode.
//
// warning                - Does not make sure that the sample is actually in fixed-point mode.
//                          If the sample is in fixed-point mode, the read/write interface
//                          will advance the cursor by one for each sample read or written.
//
// see also               - use_fixedpoint

void
Sample_Data::fixedpoint_move_cursor() {
	for(size_t chan = 0; chan<num_channels(); chan++) {
		fixedpoint_offset += fixedpoint_inc;

		size_t int_batch = (fixedpoint_offset >> FIXEDPOINT_INT_PART_BITS);	// Convert fixed-point to int.

		if(int_batch >= 1) {
			if(fixedpoint_backwards) {
				if(current_pos < int_batch) {
					Sample_EOF_Error E;
					E.set_error_pfx("fixedpoint_move_cursor");
					E.set_error("Beginning of sample reached while incrementing backwards");
					throw E;
				}
				current_pos -= int_batch;
			}
			else
				current_pos += int_batch;

			fixedpoint_offset-= int_batch << FIXEDPOINT_INT_PART_BITS;
		}
	}
}

void
Sample_Data::fixedpoint_aboutface() {
	fixedpoint_backwards = !fixedpoint_backwards;
}

void
Sample_Data::correct_loop_pointers() {
	if(loop_begin > size)
		loop_begin = size-1;
	if(sustain_loop_begin > size)
		sustain_loop_begin = size-1;

	if(loop_end > size)
		loop_end=size;
	if(sustain_loop_end > size)
		sustain_loop_end = size;
}

bool
Sample_Data::fixedpoint_is_backwards() {
	return fixedpoint_backwards;
}


float
Sample_Data::get_sample_for_cosine_mixer(size_t chan, bool use_cosine_mode) {
	sample_int_t final_data = get_data_value(chan, get_current_pos(chan));
	mix_t magic_number = (get_data_value(chan, get_current_pos(chan)+1)-final_data);

	// magic_number must be of a wider type than sample_int_t, or
	// the calculations in this function will overflow, resulting
	// in a buzzing noise in the output. 

	COMPILER_ASSERT(sizeof(magic_number) > sizeof(final_data));

	if(use_cosine_mode) {
		magic_number *= frac_cosine[fixedpoint_offset & FRACTIONAL_MASK];
		magic_number >>= FIXEDPOINT_INT_PART_BITS;
		final_data += magic_number;
	}
	else {
		magic_number *= frac_cosine_not_cosine_mode[fixedpoint_offset & FRACTIONAL_MASK];
		magic_number >>= FIXEDPOINT_INT_PART_BITS;
		final_data += magic_number;
	}

	return SAMPLE_INT_T_TO_FLOAT(final_data);
}

#define SPLINE_FRACSHIFT ((FIXEDPOINT_INT_PART_BITS-SPLINE_FRACBITS)-2)
#define SPLINE_FRACMASK (((1L<<(FIXEDPOINT_INT_PART_BITS-SPLINE_FRACSHIFT))-1)&~3)


float
Sample_Data::do_cubic_mixer_voodoo(size_t chan) {

	size_t poshi = get_current_pos(chan);
	size_t poslo = (get_current_pos(chan) << FIXEDPOINT_INT_PART_BITS) + fixedpoint_offset;


	poslo >>= SPLINE_FRACSHIFT;
	poslo &= SPLINE_FRACMASK;

	return SAMPLE_INT_T_TO_FLOAT((cubic_lut[poslo]*get_data_value(chan, poshi-1) +
	       cubic_lut[poslo+1]*get_data_value(chan, poshi  ) +
	       cubic_lut[poslo+3]*get_data_value(chan, poshi+2) +
	       cubic_lut[poslo+2]*get_data_value(chan, poshi+1)) >> SPLINE_16SHIFT);

}

float
Sample_Data::get_sample_for_linear_mixer(size_t chan) {
	sample_int_t final_data = get_int_sample(chan);
	size_t pos = get_current_pos(chan);

	// Tue May 29 04:41:45 EDT 2007
	// BUG: The "mix_t" below is essential to obtaining a correct return value.
	// The multiplication deliberately generates an overflow and then makes up for
	// it by using a larger-than-normal integer. This means that the linear mixer
	// cannot use the largest available integer type as sample_int_t: There simply
	// _must_ be a larger integer type to catch the overflow.

	return SAMPLE_INT_T_TO_FLOAT(
		final_data+((get_data_value(chan, pos+1)-final_data)*mix_t(fixedpoint_offset&FRACTIONAL_MASK) >>
			FIXEDPOINT_INT_PART_BITS)
	); 
}
