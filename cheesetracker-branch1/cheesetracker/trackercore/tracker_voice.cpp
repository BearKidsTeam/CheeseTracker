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

#include <algorithm>
#include <cassert>
#ifdef NEED_GMP
#  include <gmp.h>
#endif
#include "tracker_voice.h"
#include "ns_autoptr.h"
#include "common/defines/fixedpoint_defs.h"


#define VOICE_VOLUME_ADJUST 0.008f


int Tracker_Voice::get_current_freq() {

	return info.current_frequency;
}
int Tracker_Voice::get_current_sample_pos() {

	return info.current_index;
}

void Tracker_Voice::set_priority(int p_prio) {

	current_prio=p_prio;
}

//virtuals
Voice::Priority Tracker_Voice::get_priority() {


	return (Voice::Priority)current_prio;
}

int Tracker_Voice::set_channel(int p_channel) {

	channel_index=p_channel;
	return channel_index;
}

int Tracker_Voice::get_channel() {

	return channel_index;
}

void Tracker_Voice::set_mix_frequency(float p_mixfreq) {

	mixfreq=(int)p_mixfreq;
}
void Tracker_Voice::set_preamp(float p_preamp) {

	preamp=p_preamp;
}

bool Tracker_Voice::can_mix() {

	if ( info.current_frequency==0 )
		return false;
	if ( (info.sample_data_ptr==NULL) || (info.sample_data_ptr->is_empty()) )
		return false;
	if (!info.volume && (!info.oldlvol) && (!info.oldrvol))
		return false;

	return true;

}



void Tracker_Voice::mix(size_t p_amount,sample_t* p_where, bool use_mutex) {

	if ( info.current_frequency==0 ) info.active=false;
	if ( (info.sample_data_ptr==NULL) || (info.sample_data_ptr->is_empty()) ) info.active=false;

	if ( !info.active ) {

		return;
	}

	float pan, vol;
	ns_autoptr<Mutex_Lock_Container> ns_sample_lock;
	// Lock the sample, to prevent illegal attempts
	// to modify or read the sample while we're in
	// use_fixedpoint() mode.
	Mutex_Lock_Container *sample_lock;

	if(use_mutex) {
		sample_lock = info.sample_data_ptr->lock(__FILE__, __LINE__);  
		// The sample will be automatically unlocked at
		// the end of this scope.
		ns_sample_lock.ptr_new(sample_lock); 
	}

	info.sample_data_ptr->use_fixedpoint(true);
	info.sample_data_ptr->fixedpoint_set_resample_rate(info.current_frequency, mixfreq, info.playing_backwards);

	vol = info.volume;
	pan = info.panning/(float)PAN_RIGHT;

	info.oldlvol=info.lvolsel;
	info.oldrvol=info.rvolsel;

	info.lvolsel=vol/2.0;

	if (info.first_mix) {

		if (info.sample_needs_ramp_up) {
			info.oldlvol=0;
			info.oldrvol=0;
		} else {
			info.oldlvol=info.lvolsel;
			info.oldrvol=info.rvolsel;
		}

		info.first_mix=false;
	}


	idxsize = info.sample_data_ptr->get_size();
	loop_end = info.sample_data_ptr->get_loop_end();
	loop_begin = info.sample_data_ptr->get_loop_begin();
	add_to_mix_buffer(p_amount,p_where);
	info.sample_data_ptr->use_fixedpoint(false);
}

void Tracker_Voice::add_to_mix_buffer(size_t p_amount,sample_t *p_buffer)
{

	sample_t *mixing_buffer_index=p_buffer; 
	size_t todo=p_amount;
	float ramp_tangent_l;
	float ramp_tangent_r;

        if ( info.sample_data_ptr == NULL ) {
		info.current_index=0;
		info.fixedpoint_offset=0;
		info.active=false;
		return;
	}

	/* PRECALCULATE RAMP */

	bool loop_active=info.sample_data_ptr->is_loop_enabled() && (loop_end>loop_begin);

	if (p_amount==0) return;
	ramp_tangent_l=(float)(info.lvolsel-info.oldlvol)/(float)p_amount;
	ramp_tangent_r=(float)(info.rvolsel-info.oldrvol)/(float)p_amount;

	Resampler::Mix_Data &mixdata=resampler.get_mixdata();

	size_t left_end=loop_active?loop_begin:0;
	size_t right_end=loop_active?loop_end:idxsize;

	// We keep a local copy of the position indicator
	// in sample_data_ptr. This is because the
	// same sample's position indicator might be
	// iterated by other voices, or by the editor.
	//
	// When first entering this function,
	// the local copy is considered authoritative.
	//
	// Since the mutex for this sample is locked
	// right now, no other threads will iterate
	// the sample until after this function returns.

	info.sample_data_ptr->seek(info.current_index);
	info.sample_data_ptr->set_fixedpoint_offset(info.fixedpoint_offset);

	// This while-loop is here because the sample might
	// not have enough data in it to fill the p_buffer 
	// with p_amount stereo samples. But, if the sample is looping,
	// we can still fill the buffer by making multiple
	// passes until {todo} samples have been put into the
	// mixing buffer.

	while(todo>0){
		// Implement looping behavior and check boundaries. 

		info.active = info.sample_data_ptr->fixedpoint_loop(false);
		info.current_index=info.sample_data_ptr->get_current_pos();
		info.playing_backwards = info.sample_data_ptr->fixedpoint_is_backwards();
		if(!info.active)
			break;

		// Time to calculate how much of p_buffer we'll
		// use during this iteration of the loop.

		size_t total_samples=0;
		size_t end,done;

		// Let "end" point to either the end, beginning,
		// or next loop point in the sample, or in
		// other words, let "end" point to the spot
		// where we must stop playing the sample.

		end=info.playing_backwards?left_end:right_end;

		// Let total_samples be the number of samples
		// between the current position and the
		// end (or beginning) of the sample (or loop
		// segment).

		if(end > info.current_index) {
			total_samples = end-info.current_index;
		} else {
			total_samples = info.current_index - end;
		}

		// total_samples * (mixfreq/info.current_frequency)
		// is the number of samples that would actually get
		// stored in the p_buffer as a result of iterating
		// at the resampling rate instead of one sample at a
		// time. We'll call this the number of "virtual samples"
		// that can be extracted from the sample_data_ptr.

		// In integer math, this relationship gets distorted
		// by the lack of fractional precision. To counteract
		// this, it is necessary to multiply a certain factor
		// into the expression so that the integers behave
		// like fixed-point numbers. (see fixedpoint.h for
		// a complete explanation...). 

		// If total_samples is large enough, the extra factor
		// would result in an integer overflow. On 64-bit
		// systems, there is no larger integer that can
		// be used. Therefore, we use GMP on 64-bit platforms.

		// Let "done" be the lesser of either the number of
		// virtual samples that would be extracted from the
		// sample, or the number of samples remaining before we
		// have filled p_buffer. Since only whole numbers of
		// samples can be stored in an array, fractional
		// amounts in "done" are dropped.
		//
		// This means that it is not possible to recalculate
		// "total_samples" from "done", mixfreq, and
		// info.current_frequency:
		//
		//   total_samples != done/(mixfreq/current_freq);
		//
		// "total_samples" may exactly equal the expression to
		// the right of the "!=", or it may be 1 greater.

		// And now we calculate the value of "done":

		if (info.current_frequency != mixfreq) {
#ifdef NEED_GMP
			mpz_t gmp_mixfreq;
			mpz_t gmp_current_freq;
			mpz_t gmp_total_samples;
			mpz_t F;

			mpz_init(gmp_mixfreq);
			mpz_init(gmp_current_freq);
			mpz_init(gmp_total_samples);
			mpz_init(F);
			mpz_set_ui(gmp_mixfreq, mixfreq);
			mpz_set_ui(gmp_current_freq, info.current_frequency);
			mpz_set_ui(gmp_total_samples, total_samples);
			mpz_set_ui(F, 2048);

			// The following is equivalent to:
			//
			// (total_samples * (mixfreq*2048)/info.current_frequency)/2048
			//
			// Multiplying by 2048 prevents the division from dropping
			// less-significant digits.

			mpz_mul(gmp_mixfreq, gmp_mixfreq, F);
			mpz_div(gmp_mixfreq, gmp_mixfreq, gmp_current_freq);
			mpz_mul(gmp_total_samples, gmp_total_samples, gmp_mixfreq);
			mpz_div(gmp_total_samples, gmp_total_samples, F);

			size_t virtual_samples = mpz_get_ui(gmp_total_samples);
			
			done=std::min<size_t>(virtual_samples+1, todo);

			// CLEANUP
			mpz_clear(gmp_mixfreq);
			mpz_clear(gmp_current_freq);
			mpz_clear(gmp_total_samples);
#else
			done=std::min((size_t)FIXED_TO_INT((Uint64)total_samples * (Uint64)(INT_TO_FIXED((Uint64)mixfreq)/(Uint64)info.current_frequency))+1, todo);
#endif
		} else {
			// Mixing frequency and sample frequency are equal
			done = std::min<size_t>(total_samples+1, todo);
		}

		if ( done==0 ) {
			// p_buffer is full.
			break;
		}

		float l_vol_old,l_vol_new;
		float r_vol_old,r_vol_new;
		int written=p_amount-todo;

		l_vol_old=info.oldlvol+(float)written*ramp_tangent_l;
		r_vol_old=info.oldrvol+(float)written*ramp_tangent_r;
		l_vol_new=info.oldlvol+(float)(written+done)*ramp_tangent_l;
		r_vol_new=info.oldrvol+(float)(written+done)*ramp_tangent_r;

		// Copy a bunch of data from {info} into another
		// slightly different data structure that is used
		// by the resampler modules.

		mixdata.sample=info.sample_data_ptr;
		mixdata.dst_buff=mixing_buffer_index;

		mixdata.samples_to_mix=done;
		mixdata.l_volume_prev=l_vol_old;
		mixdata.l_volume=l_vol_new;

		mixdata.panning = info.panning;

		resampler.set_filter(info.filter.enabled,info.filter.coeffs);
		resampler.mix();

		// Update our local copy of the current position
		// in the sample.

                info.current_index=info.sample_data_ptr->get_current_pos();
		info.fixedpoint_offset = info.sample_data_ptr->get_fixedpoint_offset();
		todo-=done;

		// Incrementing by done*2 because mixing_buffer_index
		// contains stereo data and {done} is expressed in
		// samples.

		mixing_buffer_index += done*2;
	}

}


/*********************************/


void Tracker_Voice::setup_sample(Sample_Data *p_sample_data,size_t p_offset) {


	if (p_sample_data==NULL) {

		ERROR("p_sample_data is null!!");
		return;
	}

	if (p_offset>=p_sample_data->get_size()) {

		if (p_sample_data->is_loop_enabled()) {

			ERROR("Sample offset in loop not implemented!");
		} else {

			return;
		}
	}

	ns_autoptr<float> ns_buffer;
	float *buffer = new float[p_sample_data->num_channels()];
	ns_buffer.arr_new(buffer);
	Mutex_Lock_Container *lock = p_sample_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_lock;
	ns_lock.ptr_new(lock);
	
	p_sample_data->get_sample(p_offset, buffer);

	info.sample_needs_ramp_up=false;
	for(size_t chan=0; chan<p_sample_data->num_channels(); chan++) {
		if (fabs(buffer[chan])>0.1) { //this will click!
			info.sample_needs_ramp_up=true;
		} 
	}

	info.sample_data_ptr=p_sample_data;
	info.start_index=p_offset;
	info.restart();

}
bool Tracker_Voice::is_active() {

	return info.active;
}

void Tracker_Voice::set_frequency(Uint32 p_freq) {

	info.current_frequency=p_freq;
}
void Tracker_Voice::set_panning(int p_pan) {

	info.panning=p_pan;
}
void Tracker_Voice::set_volume(int p_vol)
{

	info.volume=(float)p_vol/512.0;
	info.volume*=VOICE_VOLUME_ADJUST;
}

void Tracker_Voice::set_filter(const IIR_SVF::Coeffs& p_coeffs,bool p_enabled)
{


	info.filter.coeffs=p_coeffs;
	info.filter.enabled=p_enabled;

	if (!p_enabled) {

		info.filter.hist1=0;
		info.filter.hist2=0;
		info.filter.prev=0;

	}

}


bool Tracker_Voice::has_stopped()
{
 	return was_removed_from_mixer();
}

void Tracker_Voice::reset()
{

	preamp=1;
	mixfreq=1;
	current_prio=0;
	info.clear();
	clear_status();
	channel_index=0;
	// Ensure forward playback.
	if(info.sample_data_ptr &&
	   info.sample_data_ptr->fixedpoint_is_backwards()) {
		info.sample_data_ptr->fixedpoint_aboutface();
		info.playing_backwards = false;
	}
}


Tracker_Voice::Tracker_Voice()
  : Voice()
{

	preamp=1;
	mixfreq=1;
	current_prio=0;
	channel_index=0;
}


Tracker_Voice::~Tracker_Voice()
{
}
