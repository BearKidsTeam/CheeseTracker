/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://www.geocities.com/godlessinfidelus/
    copyright            : (C) 2003 by Juan Linietsky
    maintainer           : J Phelps
    email                : godless@users.sf.net
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
#ifdef POSIX_ENABLED
#   include "common/drivers/posix/mutex_lock_pthreads.h"
#endif
#include "ns_autoptr.h"
#include "sample_data.h"

#define CURRENT_FRAME current_pos*channels+chan
#ifndef NDEBUG

#define ASSERT_NOTFIXEDPOINT(function) {			\
	if(fixedpoint_mode) {					\
		Sample_Readonly_Error E;			\
		E.set_error_pfx(__FILE__);			\
		E.eprintf("%s:%i Sample is in fixed-point mode", function, __LINE__);	\
		throw E;					\
	}							\
}

#else // defined(NDEBUG)
#define ASSERT_NOTFIXEDPOINT(function)
#endif
#define SPLINE_FRACBITS 8
#define SPLINE_QUANTBITS        14
#define SPLINE_QUANTSCALE       (1L<<SPLINE_QUANTBITS)
#define SPLINE_8SHIFT           (SPLINE_QUANTBITS-8)
#define SPLINE_16SHIFT          (SPLINE_QUANTBITS)
// forces coefsset to unity gain
#define SPLINE_CLAMPFORUNITY

// sample_data_fixedpoint.h must be included AFTER all #define constants.

#define IN_SAMPLE_DATA_CPP 1
#include "sample_data_fixedpoint.h"

int frac_cosine[COSINE_LEN];
int frac_cosine_not_cosine_mode[COSINE_LEN];
sample_int_t cubic_lut[4*(1L<<SPLINE_FRACBITS)];

typedef Sint32 mix_t;
COMPILER_ASSERT(sizeof(mix_t) > sizeof(sample_int_t));

// A class to initialize the lookup tables "frac_cosine",
// "frac_cosine_not_cosine_mode", and "cubic_lut" at global scope,
// without the need for an explicit call to an initialization function.

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
		size_t length              = (1L<<SPLINE_FRACBITS);
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


void Sample_Data::release_data_ptr()
{
	if(data_ptr != NULL) printf("Generating a memory leak on purpose...\n");
	data_ptr=NULL;
	size=0;
	is_16bits=true;
}


void Sample_Data::set_data_ptr(void *p_data,int p_size,bool p_16bits)
{

	if(!p_size)
		assert(p_size == 0 && p_data == NULL);

	size=p_size;
	is_16bits=p_16bits;
	if (data_ptr!=NULL)
		free(data_ptr);
	data_ptr=(sample_16s_t*)p_data;

}

void Sample_Data::copy_sample_data(const Sample_Data *p_sdata)
{
	ASSERT_NOTFIXEDPOINT("copy_sample_data");

	*this = *p_sdata;

}

void Sample_Data::set_c5_freq(int p_c5_freq)
{
	c5_freq=p_c5_freq;
}

Sample_Data::Sample_Data()
{
#ifdef POSIX_ENABLED
	mutex = new Mutex_Lock_Pthreads;
#else
	mutex = NULL;
#endif
	reset();
}

void
Sample_Data::reset()
{

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

Sample_Data::Sample_Data(const Sample_Data &rhs)
{
	reset();
	
#ifdef POSIX_ENABLED
	mutex = new Mutex_Lock_Pthreads;
#else
	mutex = NULL;
#endif
	// Just use operator=.
	*this = rhs;
}

Sample_Data::~Sample_Data()
{
#ifdef POSIX_ENABLED
	delete mutex;
#endif
	if(data_ptr)
		delete[] data_ptr;
}


void Sample_Data::get_sample(size_t p_index, sample_t *dest)  const
{

	if(p_index >= size) {
		throw Out_Of_Bounds(__FILE__, __LINE__);
	}
	assert(is_16bits);

	for(size_t chan=0; chan<channels; chan++) {
		dest[chan] = SAMPLE_INT_T_TO_FLOAT(data_ptr[p_index*channels+chan]);
	}
	
}

void Sample_Data::set_sample(size_t p_idx, const sample_t *p_val)
{
	ASSERT_NOTFIXEDPOINT("set_sample");
	assert(is_16bits);

	if ((p_idx>=size)) return;


	for(size_t chan=0; chan<channels; chan++) {
		data_ptr[p_idx*channels+chan] = FLOAT_TO_SAMPLE_INT_T(BOUND(p_val[chan], -1.0, 1.0));
	}
}

void Sample_Data::set_loop_enabled(bool p_enabled)
{

	loop_on=p_enabled;
}

void Sample_Data::set_loop_ping_pong(bool p_enabled)
{
	pingpong_loop=p_enabled;
}


void Sample_Data::set_loop_begin(int p_idx)
{

	loop_begin=p_idx;
	if(size == 0)
		loop_begin = 0;
	else if(loop_begin >= size)
		loop_begin = size-1;
}
void Sample_Data::set_loop_end(int p_idx)
{
	loop_end=p_idx;
	if(size == 0)
		loop_begin = 0;
	else if(loop_end >= size)
		loop_end = size-1;

}

void Sample_Data::change_sign()
{
	ASSERT_NOTFIXEDPOINT("change_sign");
	assert(is_16bits);

	if (data_ptr==NULL) return;

	size_t ix;

	for (ix=0;ix<size;ix++) {
		for(size_t chan=0; chan<channels; chan++) {
			data_ptr[ix*channels+chan] ^= (sample_int_t)1 << (sizeof(sample_int_t)*BITS_PER_BYTE-1);
		}

	}
}


/* Misc internal Editing Utils */
const sample_int_t *Sample_Data::get_data_value(size_t p_pos)
{
	if (p_pos >= size || data_ptr == NULL) {
		throw Out_Of_Bounds(__FILE__, __LINE__);
	}
	assert(is_16bits);

	return &data_ptr[p_pos*channels];

}

void Sample_Data::put_data_value(size_t p_pos, const sample_int_t *p_val)
{
	ASSERT_NOTFIXEDPOINT("put_data_value");
	assert(is_16bits);

	if(data_ptr == NULL || p_pos >= size)
		return;
	for(size_t chan=0; chan<channels; chan++)
		data_ptr[p_pos*channels+chan] = p_val[chan];
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

void Sample_Data::toggle_quality()
{
	ASSERT_NOTFIXEDPOINT("toggle_quality");

	if (data_ptr==NULL) return;

	size_t ix;

	// Sun Jun  3 15:07:38 EDT 2007
	//
	// From now on, we fake this functionality by degrading the quality
	// of the data. All bits except the top 8 bits will be dropped, but
	// the data is still represented internally as 16 bits.

	for (ix=0; ix < size; ix++) {
		data_ptr[ix] &= 0xFF << (sizeof(sample_int_t)-sizeof(Sint8))*BITS_PER_BYTE;
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

const sample_int_t *
Sample_Data::get_int_sample()
{
	const sample_int_t *retval;

	assert(is_16bits);

	if(eof_reached()) {
		Sample_EOF_Error E;
		E.set_error_pfx("get_sample");
		E.set_error("End of sample reached");
		throw E;
	}
	retval=&data_ptr[current_pos*channels];

	if(!fixedpoint_mode) 
		current_pos++;
	return retval;
}

// sd_realloc - PRIVATE - Reallocate a sample buffer.


void 
Sample_Data::sd_realloc(size_t new_size)
{
        sample_int_t *new_data;
	assert(is_16bits);

	size_t multiplier=sizeof(sample_int_t)*num_channels();
	
	new_data = new sample_int_t[new_size*num_channels()];
	if(new_data == NULL) {
		throw std::bad_alloc();
	}
	if(data_ptr) {
		size_t copy_size = std::min<size_t>(new_size, size);
        	memcpy((void*)new_data, (void*)data_ptr, copy_size*multiplier);
	}

	if(new_size > size) {
		// Initialize any newly-allocated data with zero.

		memset((void*)(new_data+(size*channels)), 0, (new_size-size)*multiplier);
	}

	delete[] data_ptr;
        data_ptr=new_data;
        size=new_size;
}

// eof_reached - Tell if the position indicator for the current channel
//               points to the end of the buffer (or the beginning, if
//               Fixed Point Mode is in use and the sample is being scanned
//               backwards).

bool
Sample_Data::eof_reached()
{
	if(fixedpoint_mode && fixedpoint_backwards) {
		return !(get_current_pos() >= 0);
	} else {
		return !(get_current_pos() < get_size());
	}
}

// put_sample - Add an integer sample to the specified channel.
//
// notes      - Extends the buffer by one if necessary. Repeated
//              reallocations is inefficient.
//
// see also   - set_size, get_size

void
Sample_Data::put_sample(const sample_int_t *smp)
{
	ASSERT_NOTFIXEDPOINT("put_sample");
	assert(is_16bits);

	if(eof_reached()) 
		sd_realloc(size+1);

	for(size_t chan=0; chan<num_channels(); chan++)
		data_ptr[CURRENT_FRAME] = smp[chan];

	current_pos++;
}

// seek - Set the position.
//
//          - If the size of the sample is less than {new_pos}, then
//            the position indicator will be set to the size of the
//            sample.
//
//          - If not in fixed-point mode, the fixed-point incrementation
//            cache is left alone.
//
// return   - Returns the final value of the position indicator.
//
// see also - get_size

ptrdiff_t
Sample_Data::seek(ptrdiff_t new_pos)
{
	if(fixedpoint_mode) {
		fixedpoint_offset=0;
	}
	else if(new_pos > get_size()) {
		new_pos = get_size();
	}
	current_pos = new_pos;
	return current_pos;
}

// get_current_pos - Get the position indicator of the specified channel.

ptrdiff_t
Sample_Data::get_current_pos() const
{
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
Sample_Data::get_sample_array(sample_int_t *dest, size_t len)
{
	assert(is_16bits);

        if(get_current_pos()+len > get_size()) {
                len -= (get_current_pos()+len)-get_size();
                if(len == 0)
                        return 0;
        }

	memcpy((void*)dest, (void*)(data_ptr+current_pos*channels),
	       len*sizeof(sample_int_t)*channels);
	current_pos += len;
        return len;
}

// put_sample_array - Put an array of integers into the specified channel
//
//                  - Extends the buffer as necessary.

void
Sample_Data::put_sample_array(const sample_int_t *src, size_t len)
{
	ASSERT_NOTFIXEDPOINT("put_sample_array");
	assert(is_16bits);

        if(len > size-current_pos) {
                sd_realloc(len-get_size()-get_current_pos());
        }

	memcpy((void*)(data_ptr+current_pos*channels), src,
	       len*sizeof(sample_int_t)*channels);
	current_pos+=len;

}

// truncate - Truncate the specified channel
//
//          - Everything after channel {chan}'s position indicator
//            is freed. 
//
//          - Reallocates memory.

void
Sample_Data::truncate()
{
	ASSERT_NOTFIXEDPOINT("truncate");
	assert(is_16bits);
	if(current_pos==0) {
		delete[] data_ptr;
		data_ptr=NULL;
		size=0;
		return;
	}
	if(eof_reached()) {
		return;
	}
	sample_int_t *new_data;
	size_t multiplier=sizeof(sample_int_t)*channels;

	new_data = new sample_int_t[current_pos*channels];
	memcpy((void*)new_data, (void*)data_ptr, current_pos*multiplier);
	size=current_pos;
	delete[] data_ptr;
	data_ptr=new_data;
	correct_loop_pointers();
}

// get_f_sample - Get a sample from the specified channel
//
// notes	- Handles end-of-sample differently in
//                fixedpoint_mode than in normal mode.
//
//                In normal mode, 0.0 is 
//                returned continuously after eof_reached(),

void Sample_Data::get_f_sample(sample_t *dest)
{
	assert(is_16bits);
	if(eof_reached()) {
		for(size_t chan=0; chan<channels; chan++)
			dest[chan] = 0.0f;
	}

	for(size_t chan=0; chan<channels; chan++) {
		dest[chan] = SAMPLE_INT_T_TO_FLOAT(data_ptr[CURRENT_FRAME]);
	}

	if(!fixedpoint_mode) {
		current_pos++; 
		if(current_pos>size)
			current_pos=size;
	}
}

// put_f_sample - Add a floating-point sample to the specified channel.
//
// notes        - Reallocating one frame at a time is inefficient.
//
// see also     - set_size

void Sample_Data::put_f_sample(const sample_t *p_val)
{
	ASSERT_NOTFIXEDPOINT("put_f_sample");
	assert(is_16bits);

	if(eof_reached())
		sd_realloc(size+1);

	for(size_t chan=0; chan<channels; chan++) {
		data_ptr[CURRENT_FRAME] = FLOAT_TO_SAMPLE_INT_T(BOUND(p_val[chan], -1.0, 1.0));
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
Sample_Data::set_size(size_t new_size)
{
	ASSERT_NOTFIXEDPOINT("set_size");
	if(size == new_size)
		return;
	if(new_size == 0) {
		seek(0);
		truncate();
	} else {
		sd_realloc(new_size);
	}
	seek(current_pos);
	correct_loop_pointers();
}

// num_channels - Return the number of channels contained in the sample.

size_t
Sample_Data::num_channels() const
{
	return channels;
}

// set_num_channels - Set the number of channels for this sample.
// 
//                  - Interleaves existing data with zeroes if increasing the
//                    number of channels.
//
//                  - Disinterleaves existing data in the event of a decrease
//                    in the number of channels;
//
// note             - This function involves reallocation and copying of data buffers.
//                    Avoid using this function in performance-critical loops.
//
// see also	  - set_size

void
Sample_Data::set_num_channels(size_t num)
{
	ASSERT_NOTFIXEDPOINT("set_num_channels");
	sample_int_t *new_data;

	if(size == 0) {
		channels=num;
	}

	new_data = new sample_int_t[num*get_size()];

	for(size_t ix=0; ix<get_size(); ix++) {
		for(size_t chan=0; chan<num; chan++) {
			if(chan >= channels) {
				new_data[ix*num+chan] = 0;
			} else {
				new_data[ix*num+chan] = data_ptr[ix*channels+chan];
			}
		}
	}

	delete[] data_ptr;
	data_ptr = new_data;
	channels=num;

	return;
}

bool
Sample_Data::is_empty()
{
	return(data_ptr == NULL || size == 0);
}

const Sample_Data &
Sample_Data::operator=(const Sample_Data &r_data)
{
	ASSERT_NOTFIXEDPOINT("operator=");
	set_num_channels(r_data.num_channels());
	assert(r_data.is_16bit());

	// Copy sample buffer

	fixedpoint_offset	= r_data.fixedpoint_offset;

	seek(0);
	set_size(r_data.get_size());
	for(size_t jx=0; jx<r_data.get_size(); jx++) {
		put_sample(&(r_data.data_ptr[jx*r_data.num_channels()]));
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


void
Sample_Data::correct_loop_pointers()
{
	if(loop_begin > size)
		loop_begin = size-1;
	if(sustain_loop_begin > size)
		sustain_loop_begin = size-1;

	if(loop_end > size)
		loop_end=size;
	if(sustain_loop_end > size)
		sustain_loop_end = size;
}


template<class TYPE> void
blank_sample(TYPE *dest, TYPE zero, size_t channels)
{
	for(size_t chan=0; chan<channels; chan++) {
		dest[chan]=zero;
	}
}

void
Sample_Data::get_sample_for_cosine_mixer(sample_t *dest, bool use_cosine_mode)
{
	if(eof_reached()) {
		blank_sample<sample_t>(dest, 0.0, channels);
		return;
	}
	const sample_int_t *current_frame = get_data_value(get_current_pos());
	const sample_int_t *next_frame;
	ns_autoptr<sample_int_t> ns_temp;

	// The next_frame may be past the end of the sample.

	if(get_current_pos()+1 >= get_size()) {
		// 'temp' is needed because 'next_frame' is const.
		sample_int_t *temp = new sample_int_t[channels];
		ns_temp.arr_new(temp);
		next_frame = temp;

		blank_sample<sample_int_t>(temp, 0, channels);
	} else {
		next_frame = get_data_value(get_current_pos()+1);
	}

	for(size_t chan=0; chan<channels; chan++) {
		sample_int_t final_data = current_frame[chan];
		mix_t magic_number = next_frame[chan]-final_data;

		// magic_number must be of a wider type than sample_int_t, or
		// the calculations in this function will overflow, resulting
		// in a buzzing noise in the output. 

		COMPILER_ASSERT(sizeof(magic_number) > sizeof(final_data));

		if(use_cosine_mode) {
			magic_number *= frac_cosine[fixedpoint_offset & FRACTIONAL_MASK];
			IP_FIXED_TO_INT(magic_number);
			final_data += magic_number;
		}
		else {
			magic_number *= frac_cosine_not_cosine_mode[fixedpoint_offset & FRACTIONAL_MASK];
			IP_FIXED_TO_INT(magic_number);
			final_data += magic_number;
		}
		dest[chan] = SAMPLE_INT_T_TO_FLOAT(final_data);
	}
}

#define SPLINE_FRACSHIFT ((FIXEDPOINT_INT_PART_BITS-SPLINE_FRACBITS)-2)
#define SPLINE_FRACMASK (((1L<<(FIXEDPOINT_INT_PART_BITS-SPLINE_FRACSHIFT))-1)&~3)


void
Sample_Data::do_cubic_mixer_voodoo(sample_t *dest)
{
	if(eof_reached()) {
		blank_sample<sample_t>(dest, 0.0, channels);
		return;
	}

	size_t poshi = get_current_pos();
	size_t poslo_fixed = INT_TO_FIXED(get_current_pos()) + fixedpoint_offset;
	const sample_int_t *prev_frame = get_data_value(poshi ? poshi-1 : 0);
	const sample_int_t *current_frame = get_data_value(poshi);

	poslo_fixed >>= SPLINE_FRACSHIFT;
	poslo_fixed &= SPLINE_FRACMASK;

	for (size_t chan=0; chan<channels; chan++) {
		dest[chan] = SAMPLE_INT_T_TO_FLOAT((cubic_lut[poslo_fixed]*(prev_frame[chan]) +
	       			cubic_lut[poslo_fixed+1]*current_frame[chan] +
	       			cubic_lut[poslo_fixed+3]*current_frame[chan+channels*2] +
	       			cubic_lut[poslo_fixed+2]*current_frame[chan+channels]) >> SPLINE_16SHIFT);
	}

}

void
Sample_Data::get_sample_for_linear_mixer(sample_t *dest)
{

	if(eof_reached()) {
		blank_sample<sample_t>(dest, 0.0, channels);
		return;
	}

	size_t pos = get_current_pos();
	const sample_int_t *current_frame = get_int_sample();
	const sample_int_t *next_frame;
	ns_autoptr<sample_int_t> ns_temp;

	if(get_current_pos() + 1 >= get_size()) {
		sample_int_t *temp = new sample_int_t[num_channels()];
		ns_temp.arr_new(temp);
		next_frame=temp;
		blank_sample<sample_int_t>(temp, 0, channels);
	} else {
		next_frame = get_data_value(pos+1);
	}
	// Tue May 29 04:41:45 EDT 2007
	// BUG: The "mix_t" below is essential to obtaining a correct return value.
	// The multiplication deliberately generates an overflow and then makes up for
	// it by using a larger-than-normal integer. This means that the linear mixer
	// cannot use the largest available integer type as sample_int_t: There simply
	// _must_ be a larger integer type to catch the overflow.

	COMPILER_ASSERT(sizeof(mix_t) > sizeof(sample_int_t));

	for(size_t chan=0; chan<channels; chan++) {
		sample_int_t final_data = current_frame[chan];
		dest[chan] = SAMPLE_INT_T_TO_FLOAT(
			final_data+FIXED_TO_INT((next_frame[chan]-final_data)*mix_t(fixedpoint_offset&FRACTIONAL_MASK))
		); 
	}
}


// lock         - Lock the sample's multireader_lock.
//
// return value - Pointer to multireader_lock_container, which unlocks
//                the multireader_lock when deleted.
//

Mutex_Lock_Container *
Sample_Data::lock(const char *file, int line)
{
	return new Mutex_Lock_Container(mutex, file, line);
}


