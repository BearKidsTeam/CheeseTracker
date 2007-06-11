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
#include "tracker_voice.h"
#include "ns_autoptr.h"


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



void Tracker_Voice::mix(size_t p_amount,sample_t* p_where) {

	if ( info.current_frequency==0 ) info.active=false;
	if ( (info.sample_data_ptr==NULL) || (info.sample_data_ptr->is_empty()) ) info.active=false;

	if ( !info.active ) {

		return;
	}

	float pan, vol;
	ns_autoptr<Mutex_Lock_Container> ns_sample_lock;
	// Lock the sample's mutex, to prevent illegal attempts
	// to modify the sample while we're in use_fixedpoint()
	// mode.
	Mutex_Lock_Container *sample_lock = info.sample_data_ptr->lock();  
	// Mutex will be automatically unlocked at
	// the end of this scope.
	ns_sample_lock.ptr_new(sample_lock); 

	info.sample_data_ptr->use_fixedpoint(true);
	info.sample_data_ptr->fixedpoint_set_resample_rate(info.current_frequency, mixfreq, info.playing_backwards);

	vol = info.volume;
	pan = info.panning/(float)PAN_RIGHT;

	info.oldlvol=info.lvolsel;
	info.oldrvol=info.rvolsel;

	// TODO: Determine if this is still needed. {

	if(info.panning != PAN_SURROUND) {

		info.lvolsel=vol*(1.0-pan);
		info.rvolsel=vol*pan;

	} else	{
		info.lvolsel=info.rvolsel=vol/2.0;
	}
	// }

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

void Tracker_Voice::add_to_mix_buffer(size_t p_amount,sample_t *p_buffer) {

	sample_t *mixing_buffer_index=p_buffer; 
	size_t todo=p_amount;
	float ramp_tangent_l;
	float ramp_tangent_r;

        if ( info.sample_data_ptr == NULL ) {
		info.current_index=0;
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


	info.current_index=info.sample_data_ptr->get_current_pos();

	// This while-loop is here because the sample might
	// not have enough data in it to fill the p_buffer 
	// with p_amount stereo samples. But, if the sample is looping,
	// we can still fill the buffer by making multiple
	// passes until {todo} samples have been put into the
	// mixing buffer.

	while(todo>0) {
		// Implement looping behavior and check boundaries. 

		if ( info.playing_backwards ) {
			/* The sample is playing in reverse */
			if( ( loop_active )&&(info.current_index<=loop_begin) ) {
				/* the sample is looping and has reached loop_begin */
				if ( info.sample_data_ptr->is_loop_ping_pong() ) {
					/* Ping-pong loop: Reverse loop direction */
					info.playing_backwards=false;
					info.sample_data_ptr->fixedpoint_aboutface();
				} else
					/* normal backwards looping, so set the current
					 * position to loopend index */
					info.current_index=loop_end;
			} else {
				/* the sample is not looping, so check if it reached index 0 */
				if(info.current_index < 1) {
					/* playing index reached 0, so stop playing this sample */
					info.current_index=0;
					info.active=false;
					break;
				}
			}
		} else {
			/* The sample is playing forward */
			if ( (loop_active) && (info.current_index >= loop_end)) {
				/* the sample is looping, check the loopend index */
				if( info.sample_data_ptr->is_loop_ping_pong() ) {
					info.playing_backwards=true;
					info.sample_data_ptr->fixedpoint_aboutface();
				} else
					/* normal backwards looping, so set the current position
					   to loopend index */
					info.current_index=loop_begin;
			} else {
				/* sample is not looping, so check if it reached the last
				   position */
				if(info.current_index >= idxsize) {
					/* yes, so stop playing this sample */
					info.current_index=0;
					info.active=false;
					break;
				}
			}
		}

		// Make sure that the changes to current_index above
		// are reflected in the sample's position indicator.
		// Furthermore, verify the actual position within the
		// sample in case we're off.

		info.sample_data_ptr->use_fixedpoint(false);
		info.current_index=info.sample_data_ptr->seek(info.current_index);
		info.sample_data_ptr->use_fixedpoint(true);

		// Time to calculate how much of p_buffer we'll
		// use during this iteration of the loop.

		size_t total_samples=0;
		size_t end,done;

		// FIXME: Invalid if the sample is looping and either
		// loop index is not equal to the beginning or end
		// of the sample.

		end=info.playing_backwards?left_end:right_end;

		// Let total_samples be the number of samples
		// between the current position and the
		// end (or beginning) of the sample.

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

		// Since we're using integer math, we also use
		// the equivalent expression total_samples
		// / (info.current_frequency/mixfreq) if
		// (mixfreq/info.current_frequency) would be less
		// than one.

		// A third condition is used for the case where
		// (mixfreq/info.current_frequency) would equal one,
		// in which case multiplication would be pointless.

		// Let "done" be the lesser of either the number of
		// virtual samples that would be extracted from the
		// sample, or the number of samples remaining before we
		// have filled p_buffer.

		if(info.current_frequency > mixfreq) {
			done=std::min<size_t>(total_samples/(info.current_frequency/mixfreq), todo);
		} else if (info.current_frequency < mixfreq) {
			done=std::min<size_t>(total_samples * (mixfreq/info.current_frequency), todo);
		} else {
			// Mixing frequency and sample frequency are equal
			//
			done = std::min<size_t>(total_samples, todo);
		}


		if ( done==0 ) {
			info.active = 0;
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

		mixdata.increment_fraction.num = info.current_frequency;
		mixdata.increment_fraction.den = mixfreq;
		mixdata.sample=info.sample_data_ptr;
		mixdata.dst_buff=mixing_buffer_index;

		mixdata.sample_offset=info.current_index;
		mixdata.samples_to_mix=done;
		mixdata.l_volume_prev=l_vol_old;
		mixdata.r_volume_prev=r_vol_old;
		mixdata.l_volume=l_vol_new;
		mixdata.r_volume=r_vol_new;

		mixdata.panning = info.panning;

		resampler.set_filter(info.filter.enabled,info.filter.coeffs);

		resampler.mix();


		// FIXME: In the event that info.current_frequency > mixfreq, it
		// is possible that the calculation used to produce "done"
		// cannot be reversed to get total_samples, due to loss of
		// precision. In these cases, the reverse operation will
		// always be less than total_samples, meaning that the
		// actual sample position after mixing will be less than
		// info.current_index + total_samples.

		// In all other cases, we expect that the sample position
		// after mixing will always be exactly equal to
		// info.current_index + total_samples.

#ifndef NDEBUG
		if(info.current_frequency <= mixfreq && done != todo) {
			assert(info.sample_data_ptr->get_current_pos()
			       == info.current_index + total_samples);
		}
#endif

		// Update our local copy of the current position
		// in the sample.

                info.current_index=info.sample_data_ptr->get_current_pos();
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
