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


//	int i;
	 
	float pan, vol;
		// Calculate increment index depending on frequency difference
	info.increment_index=((Sint64)(info.current_frequency<<FRACTIONAL))/mixfreq;
	info.sample_data_ptr->use_fixedpoint(true);
	info.sample_data_ptr->fixedpoint_set_resample_rate(info.current_frequency, mixfreq, info.playing_backwards);

	if (info.playing_backwards) info.increment_index=-info.increment_index;

	vol = info.volume;
	pan = info.panning/(float)PAN_RIGHT;

	info.oldlvol=info.lvolsel;
	info.oldrvol=info.rvolsel;


	if(info.panning != PAN_SURROUND) {

		info.lvolsel=vol*(1.0-pan);
		info.rvolsel=vol*pan;

	} else	{
		info.lvolsel=info.rvolsel=vol/2.0;
	}

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
	idxlend = info.sample_data_ptr->get_loop_end();
	idxlpos = info.sample_data_ptr->get_loop_begin();

	if (info.increment_index==0) {

		info.active=false;
		return;
	}
	add_to_mix_buffer(p_amount,p_where);
	info.sample_data_ptr->use_fixedpoint(false);
}

void Tracker_Voice::add_to_mix_buffer(size_t p_amount,sample_t *p_buffer) {


	size_t end,done;
	sample_t *mixing_buffer_index;

	mixing_buffer_index=p_buffer;

	size_t total=p_amount;
	size_t todo=total;

	float ramp_tangent_l;
	float ramp_tangent_r;

        if ( info.sample_data_ptr == NULL ) {

		info.current_index=0;
		info.active=false;
		return;
	}


	/* precalculate ramp */


	/* update the 'current' index so the sample loops, or stops playing if it
	   reached the end of the sample */

	bool loop_active=info.sample_data_ptr->is_loop_enabled() && (idxlend>idxlpos);

	if (total==0) return;

	ramp_tangent_l=(float)(info.lvolsel-info.oldlvol)/(float)total;
	ramp_tangent_r=(float)(info.rvolsel-info.oldrvol)/(float)total;

	//                  2

	Resampler::Mix_Data &mixdata=resampler.get_mixdata();

	size_t lend=loop_active?idxlpos:0;
	size_t rend=loop_active?idxlend:idxsize;

	while(todo>0) {

		if ( info.playing_backwards ) {
			/* The sample is playing in reverse */
			if( ( loop_active )&&(info.current_index<idxlpos) ) {
				/* the sample is looping and has reached the loopstart index */
				if ( info.sample_data_ptr->is_loop_ping_pong() ) {
					/* sample is doing bidirectional loops, so 'bounce' the
					   current index against the idxlpos */
					info.current_index = idxlpos+(idxlpos-info.current_index);
					info.playing_backwards=false;
					info.increment_index = -info.increment_index;
					info.sample_data_ptr->fixedpoint_aboutface();
				} else
					/* normal backwards looping, so set the current position to
					   loopend index */
					info.current_index=idxlend-(idxlpos-info.current_index);
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
			if ( (loop_active) && (info.current_index >= idxlend)) {
				/* the sample is looping, check the loopend index */
				if( info.sample_data_ptr->is_loop_ping_pong() ) {
					/* sample is doing bidirectional loops, so 'bounce' the
					   current index against the idxlend */
					info.playing_backwards=true;
					info.sample_data_ptr->fixedpoint_aboutface();
					info.increment_index = -info.increment_index;
					info.current_index = idxlend-(info.current_index-idxlend);
				} else
					/* normal backwards looping, so set the current position
					   to loopend index */
					info.current_index=idxlpos+(info.current_index-idxlend);
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

		info.sample_data_ptr->use_fixedpoint(false);

		info.sample_data_ptr->seek(info.current_index);

		info.sample_data_ptr->use_fixedpoint(true);

		end=info.playing_backwards?lend:rend;

		//  Sun Apr 22 16:54:40 EDT 2007
		//
		//  My goal is to eliminate the address-range limitations created by
		//  the use of 32-bit fixed-point numbers. Juan Linietsky addressed
		//  this problem on 32-bit systems by making some of the indices
		//  (such as lend and rend) 64-bit (and signed), but this is broken
		//  on 64-bit systems, because the underlying assumption is that half
		//  the bits in a 64-bit number are going to be unused in a (32-bit)
		//  index.
		//
		//  The original code only used 64-bit numbers for SOME of the
		//  fixed-point numbers: Others were only 32 bits, with 11 of
		//  those bits being devoted to fractional data and an additional
		//  bit lost because those numbers were all signed. This left a
		//  20-bit address range for indexing samples, whether your
		//  native address space is 32 bits, 64 bits, or a million bits.
		//
		//  This is now addressed by keeping a temporary index counter in
		//  the Sample_Data in fixed-point (unsigned) format. When this temporary counter's
		//  value exceeds 1 (that is, 1 << 11), the number is converted to integer
		//  format and the resulting integer is added to the real position-counter
		//  (which is of type size_t) and then this integer is converted back
		//  to fixed-point format to be subtracted from the temporary counter.
		//
		//  This allows me to appear to be adding a fractional value to an integer,
		//  without losing any range (not even on 64-bit systems, or on systems with
		//  even bigger address spaces that haven't been built yet), but it doesn't
		//  allow me to divide the integer value by a fixed-point value and get the
		//  number of actual samples that will end up going to the mixing buffer.
		//
		//  This can be overcome with special-case handling, going back to the way
		//  the fixed-point incrementation number is calculated in the first place:
		//
		//  fixedpoint_inc = (current_freq << 11)/mixfreq;
		//
		//  that is, the incrementation is a fraction whose numerator is current_freq and whose
		//  denomonator is mixfreq (<< 11 converts current_freq to the fixed-point form). When
		//  current_freq is greater than mixfreq, the total number of samples will be decreased
		//  to (total_samples/(current_freq/mixfreq)).
		//  
		//  When current_freq is LESS than mixfreq, then the total number of samples will be
		//  INCREASED to (total_samples * (mixfreq/current_freq))
		//
		//  This can be done with 100% fidelity in unsigned integer math, but is slower
		//  than the original signed fixed-point math that was once here because it requires many
		//  more instructions (such as tests to make sure that subtraction will not result in overflow
		//  and to make sure that division will not result in less-than-one). But it's also more portable,
		//  being as correct on 64-bit chips as it is on 32-bit chips, and it creates no arbitrary
		//  address-space limitation.

		size_t total_samples=0;

		if(end > info.current_index) {
			total_samples = end-info.current_index;
		} else {
			total_samples = info.current_index - end;
		}

		if(info.current_frequency > mixfreq) {
			done=std::min<size_t>(total_samples/(info.current_frequency/mixfreq)+1, todo);
		} else if (info.current_frequency < mixfreq) {
			done=std::min<size_t>(total_samples * (mixfreq/info.current_frequency)+1, todo);
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
		int written=total-todo;

		l_vol_old=info.oldlvol+(float)written*ramp_tangent_l;
		r_vol_old=info.oldrvol+(float)written*ramp_tangent_r;
		l_vol_new=info.oldlvol+(float)(written+done)*ramp_tangent_l;
		r_vol_new=info.oldrvol+(float)(written+done)*ramp_tangent_r;

		mixdata.increment_fraction.num = info.current_frequency;
		mixdata.increment_fraction.den = mixfreq;
		mixdata.sample=info.sample_data_ptr;
		mixdata.dst_buff=mixing_buffer_index;

		mixdata.sample_offset=info.current_index;
		mixdata.increment=info.increment_index;
		mixdata.samples_to_mix=done;
		mixdata.l_volume_prev=l_vol_old;
		mixdata.r_volume_prev=r_vol_old;
		mixdata.l_volume=l_vol_new;
		mixdata.r_volume=r_vol_new;

		mixdata.fractional_size=FRACTIONAL;
		mixdata.panning = info.panning;

		resampler.set_filter(info.filter.enabled,info.filter.coeffs);

		resampler.mix();

                info.current_index=info.sample_data_ptr->get_current_pos();
		todo-=done;

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
void Tracker_Voice::set_volume(int p_vol) {

	info.volume=(float)p_vol/512.0;
	info.volume*=VOICE_VOLUME_ADJUST;
}
void Tracker_Voice::set_filter(const IIR_SVF::Coeffs& p_coeffs,bool p_enabled) {


	info.filter.coeffs=p_coeffs;
	info.filter.enabled=p_enabled;

	if (!p_enabled) {

		info.filter.hist1=0;
		info.filter.hist2=0;
		info.filter.prev=0;

	}

}


bool Tracker_Voice::has_stopped() {


 	return was_removed_from_mixer();
}

void Tracker_Voice::reset() {

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
