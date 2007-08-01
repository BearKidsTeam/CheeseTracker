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
#ifndef HELPERS_H
#define HELPERS_H

#include <cstdio>
#include "ns_autoptr.h"
#include "../../../cheesetracker/trackercore/range_defs.h"

#define HARD_CODED_MIXER_CHANNELS 2


//This will not really happen if the sample is 16 bits, as it
//will be removed by the optimizer

#define DEPTH_SHIFT ( (sizeof(Depth)==1) ? 8 : 0 )


/* Since all resamplers will most likely share this, and this is the most critical section
  of the code (besides i cannot trust the compiler for inlining this properly)
  I am forced to go with a macro scheme to share the common parts of te resampling process.
*/

//need local variables in the stack, as the stack will be cached in a single cache line.
#define HELPER_INITIALIZE \
										\
		/*Precision template */						\
		sample_t *dest_buffer=mixdata->dst_buff;			\
		/* Data handler */						\
		float final_float[HARD_CODED_MIXER_CHANNELS];			\
		for(size_t ix=0; ix<HARD_CODED_MIXER_CHANNELS; ix++)		\
			final_float[ix] = 0.0;					\
		size_t channels = mixdata->sample->num_channels();		\
		float pan_normalized = (float)mixdata->panning/(float)PAN_RIGHT;\
		bool right_of_centre = pan_normalized > 0.5;			\
		float from_centre = (right_of_centre ? (pan_normalized - 0.5) : (0.5 - pan_normalized))*2.0;		\
        	float send_l=mixdata->l_volume;                                \
        	float send_r=mixdata->r_volume;  				\
        	float send_l_prev=mixdata->l_volume_prev;                      \
        	float send_r_prev=mixdata->r_volume_prev;  			\
        	/* Filter Setup */                                              \
		float hist_a1=mixdata->filter.hist_a1;				\
		float hist_a2=mixdata->filter.hist_a2;				\
		float hist_b1=mixdata->filter.hist_b1;				\
		float hist_b2=mixdata->filter.hist_b2;				\
		float pre_filter;						\
		float f_a1=mixdata->filter.coeffs.a1;	                        \
		float f_a2=mixdata->filter.coeffs.a2;			\
		float f_b0=mixdata->filter.coeffs.b0;	                        \
		float f_b1=mixdata->filter.coeffs.b1;	                        \
		float f_b2=mixdata->filter.coeffs.b2;			\
        	                                                                \
        	/* Ramp Setup */                                                \
		float ramp_l_base=(float)(send_l-send_l_prev)/(float)mixdata->samples_to_mix; \
		float ramp_r_base=(float)(send_r-send_r_prev)/(float)mixdata->samples_to_mix; \
		float ramp_l=ramp_l_base;                          \
		send_l=send_l_prev;				\
		send_r=send_r_prev;				\
		int sample_increment_threshold=5;			\
        	                                                                \
        	size_t samples_to_mix=mixdata->samples_to_mix;                  \
		if (sample_increment_threshold>samples_to_mix) 			\
			sample_increment_threshold=samples_to_mix;

       // CHECK_END_OF_SAMPLE_AT_MIX_BEGIN checks if the current sample
       // position (in channel 0) is less than sample_increment_threshold
       // positions away from the end of the sample, with the cursor
       // incrementing away from the end (backwards).

#define CHECK_END_OF_SAMPLE_AT_MIX_BEGIN  (mixdata->sample->fixedpoint_is_backwards() && mixdata->sample->get_current_pos() < mixdata->sample->get_size() - sample_increment_threshold)


	// CHECK_END_OF_SAMPLE_AT_MIX_END checks if the current
	// sample cursor is less than sample_increment_threshold+
	// samples_to_mix*increment positions away from the end of the
	// sample, with incrementation moving forward.

// NOTE: The multiplication below is an overloaded operator that requires an integer on the right side. The left side
// of the * is of type MixData::Fraction. The operatior returns type size_t.

#define CHECK_END_OF_SAMPLE_AT_MIX_END (!mixdata->sample->fixedpoint_is_backwards() && mixdata->sample->get_current_pos() < (mixdata->increment_fraction * samples_to_mix)+ sample_increment_threshold)

// FIXME: This is just stupid.

#define HELPER_BEGIN_LOOP	\
				\
		while(samples_to_mix--) {


// Mon Apr 23 00:56:59 EDT 2007
//
// THE GENEALOGY OF send_l AND send_r.
//
// Here is yet another great dilemma for the support of stereo samples.
// No wonder Linietsky started over. 
//
// Since it was previously assumed that samples would always be mono, "panning"
// in the stereo mixing buffer was acheived by dicking around with "volume"
// coefficients for the two mixing channels. (Note that it was also previously
// assumed that all sound cards are stereo-- Those of you whose sound cards
// support 6 to 12 channels of output are shit out of luck, and Linietsky's
// rewrite, ChibiTracker, is just as utterly stereo as CheeseTracker.).
//
// First, in Tracker_Voice::mix(), the value info.panning, a number whose range
// is from 0 to PAN_RIGHT, is retrieved and normalized into a floating-point number
// between 0 and 1 (0.5 is the center-pan volume). This normalized number is given
// the name "pan", which is local to Tracker_Voice::mix.
//
// INTRODUCTION OF THE FIRST VOLUME VARIABLES
//
// Next, two values called info.lvolsel and info.rvolsel are derived from "pan". In
// the event that info.panning contains the special value PAN_SURROUND, both of
// the former values are assigned the value info.volume/2.0.
//
// In all other cases, info.lvolsel=vol*(1.0-pan) (that is, it equals
// the panning distance from normalized PAN_RIGHT times info.volume), and
// info.rvolsel=vol*pan. So center-panned mono samples are half as loud in each
// speaker as mono samples that are panned to the left or right.

// There is some ramp-handling code (which has to consider the info.lvolsel
// and info.rvolsel values) and some memory management after that,

// Control then passes down to Tracker_Voice::add_to_mix_buffer(), where even
// more calculations are done on the left and right volumes.

// Two ramp-volume variables are declared, ramp_tangent_l and ramp_tangent_r,
// which further affect the final volumes that are used to acheive panning:

// ramp_tangent_l = (info.lvolsel-info.oldlvol)/(total_samples_to_process),

// and almost the exact same thing is done with ramp_tangent_r. In the event that
// info.first_mix and info.sample_needs_ramp_up are both true, info.oldlvol
// will equal 0, otherwise if only info.first_mix is true, then info.oldlvol will
// equal info.lvolsel. If neither of the two booleans is true, then info.oldlvol
// is left uninitialized, and presumably contains the value from the last call to
// Tracker_Voice::mix().

// After that, two left and right-named variables are declared that have nothing
// to do with channels.

// After that, there are reams and reams of code that deal with positioning
// various index indicators correctly so that a specific number of samples
// are put into the mixing buffer, and so that those samples come from the
// correct segment of the Sample_Data.  There is code that performs a very similar
// function within the Sample_Data, but this code deals with sample loops and
// the like, which would probably be more at home within the Sample_Data class.
//
// A note of caution: The position indicator in the Sample_Data is shared by
// many different routines, and is therefore not guaranteed to retain its
// value across successive calls to the mixer.

// Then, four more volume variables are declared: l_vol_new, l_vol_old,
// and equivalent r_ named variables. They are filled with l_vol_old =
// written*ramp_tangent_l, and l_vol_new = (written+done)*ramp_tangent_l.
// Notice that ramp_"tangent"_l actually has nothing to do with tangents--
// it is merely a normalized number that is multiplied by two size_t numbers,
// presumably resulting in a fraction.

// These four variables are then stored in the mixdata structure that gets
// passed to this code:
//
// mixdata.l_volume = l_vol_new;
// mixdata.l_volume_prev = l_vol_old;
//
// And equivalent for the right channel.
//
// Then, control finally passes to one of the Resampler::mix() functions,
// which all use the macros in this file, and inherit the mixdata.l_volume and
// mixdata.r_volume whose values are derived practically all over the place.

// The local variables send_l and send_r are initialized with mixdata.l_volume
// and mixdata.r_volume.
//
// send_l and send_r are multilpied by the floating-point form of the
// assumed-to-be-mono Sample_Data, and the results are added to {dest_buffer}
// in turn, so that {dest_buffer} ends up being an interleaved, two-channel,
// floating-point sample buffer.
//
// Since the ultimate value of send_l and send_r depends on data in the
// Tracker_Voice::info structure, which is not accessible from here, at
// least something of their value would have to be calculated in
// the Tracker_Voice class.
//
// Also, send_l and send_r are a combination of panning information (calculated
// almost at the very beginning), and volume-ramping information (which appears
// to be for the purpose of eliminating clicking sounds).
//
// If my presumtions are correct, then it should be possible to separate the
// volume calculation from the panning calculation. This would result in a
// single volume variable in the Resampler and the Tracker_Voice, which would
// simplify a lot of code, and pave the way to eventually extend the internal
// mixer to support more than two channels.
//
// For this to work, the mixdata structure has to be given a field into which
// I can put info.panning, which is not accessible in this scope.
//
// The single volume calculation would be left where it is (it is far too
// complicated to play around with and is Good Enough). The panning calculation
// would be moved to the Resampler, where it could be improved to do "something
// reasonable" on sound cards with more than 2 channels, at least until I come up with
// a representation of panning that can fully exploit the capabilities of
// a many-channel sound card (the representation would have to be integrated
// into the UI before it would be of any use-- it won't be as simple as
// "left-right" or even "left-right-front-center-back", and it will have to
// change according to either how many speakers are actually present, or how
// many speakers the user or song has specified. Panning schemes of songs
// written for many-speaker systems would have to degrade gracefully when
// played back on stereo equipment. Support for mono equipment should be
// considered).

#define HELPER_PERFORM_VOLUME_RAMP 	\
					\
			send_l+=ramp_l; 		

#define HELPER_PERFORM_FILTERING 	\
					\
		/* This will also be removed if compiling with no filter version */				\
			if (FILTER_VERSION) {										\
				pre_filter=final_float[0];											\
				for(size_t chan=0; chan<HARD_CODED_MIXER_CHANNELS; chan++) 				\
					final_float[chan] *= f_b0 + hist_b1 * f_b1  + hist_b2 * f_b2 + hist_a1 * f_a1 + hist_a2 * f_a2; 	\
				hist_a2=hist_a1;                                                             		\
				hist_b2=hist_b1;                                                             		\
				hist_b1=pre_filter;			\
				hist_a1=final_float[0];			\
			}

#define HELPER_PERFORM_MIXDOWN 	\
                                \
			for(size_t chan = 0; chan < HARD_CODED_MIXER_CHANNELS; chan++)		\
				*dest_buffer++ += send_l * final_float[chan];			\

#define HELPER_END_LOOP	\
                        \
		}


// This macro is only valid in the scope of the PREPROCESS_ROUTINE argument
// to be given to HELPER_MIX_ONE_SAMPLE.

#define CURRENT_CHANNEL (chan % channels)

// This is the new HELPER_MIX_ONE_SAMPLE. It mixes more-than-two-channel
// samples down to two channels, and mixes mono samples up to stereo. It
// also does the Right Thing for stereo-to-stereo mixing. Because the
// panning variable is still a single axis (capable of representing only a
// left-to-right value), this mixer can only handle a two-channel output
// mixer. Changing HARD_CODED_MIXER_CHANNELS to a number greater than
// 2 would result in all channels other than channel 0 being treated as
// "the right channel".

// Supporting a multi-channel mixer would require a completely different
// concept of "panning", which would eliminate the whole concept of
// this_side_of_centre, upon which this macro critically depends.

// The argument PREPROCESS_ROUTINE is intended to be a block of code that
// gets executed before the sample is adjusted for panning. The block
// of code is responsible for fetching the next sample from channel
// CURRENT_CHANNEL of mixdata->sample and mixing it into the local
// variable temp_float, which is zero-initialized. It should be given
// in curly brackets. Don't forget the semicolon at the end of the last
// statement. A parse error here would be very difficult to trace.
//
// The final output goes to the two-channel, floating-point final_float[]
// array.


// It was my intention that PREPROCESS_ROUTINE would simply be the
// algorithm that makes the cubic, cosine, etc. mixers different
// from each other. But instead, it turns out that all of the
// mixers depend so heavily on the fixed-point sample position
// indicator (which I made private to Sample_Data because having it
// out in the open was causing problems) that most of the code had to
// be moved into Sample_Data, so PREPROCESS_ROUTINE typically consists of
// a call to one of the methods I added to Sample_Data.


#define HELPER_MIX_ONE_SAMPLE(PREPROCESS_ROUTINE)		\
for(size_t chan=0; chan<HARD_CODED_MIXER_CHANNELS; chan++)								\
	final_float[chan] = 0.0;											\
	ns_autoptr<float> ns_temp_float;										\
	float *temp_float = new float[channels];									\
	ns_temp_float.arr_new(temp_float);										\
	PREPROCESS_ROUTINE												\
for(size_t chan=0; chan<std::max<size_t>(HARD_CODED_MIXER_CHANNELS, channels); chan++) {				\
	bool this_side_of_centre = (chan % HARD_CODED_MIXER_CHANNELS) ? !right_of_centre : right_of_centre;		\
															\
	/* Panning towards one side makes the other side quieter. */							\
	/* But the side being panned toward does not get louder. */							\
	if(!this_side_of_centre) {											\
		temp_float[CURRENT_CHANNEL] -= temp_float[CURRENT_CHANNEL] * from_centre;				\
	}														\
	final_float[chan % HARD_CODED_MIXER_CHANNELS] += temp_float[CURRENT_CHANNEL];					\
}															\
	/* Renormalize */												\
if(channels > HARD_CODED_MIXER_CHANNELS) {										\
	for(size_t chan=0; chan<HARD_CODED_MIXER_CHANNELS; chan++)							\
		final_float[chan] = final_float[chan] / ((float)channels/(float)HARD_CODED_MIXER_CHANNELS);		\
}															\
mixdata->sample->fixedpoint_move_cursor();										\
HELPER_PERFORM_VOLUME_RAMP												\
HELPER_PERFORM_FILTERING												\
HELPER_PERFORM_MIXDOWN


#define HELPER_MIX_ONE_RAW_SAMPLE					\
	HELPER_MIX_ONE_SAMPLE({mixdata->sample->get_f_sample(temp_float);				\
				})				\



#define HELPER_UPDATE_MIXDATA_STATUS	\
					\
		mixdata->filter.hist_a1=hist_a1;        						\
		mixdata->filter.hist_a2=hist_a2;                                                   \
		mixdata->filter.hist_b1=hist_b1;        						\
		mixdata->filter.hist_b2=hist_b2;                                                   \
		mixdata->sample_offset=mixdata->sample->get_current_pos();



#endif


