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

#ifndef _TRACKER_VOICE_H_
#define _TRACKER_VOICE_H_

#include "components/audio/voice.h"
#include "components/audio/sample_data.h"
#include "range_defs.h"
#include "components/audio/resampler_instance.h"
#include "ns_autoptr.h"

/**
 *
 * Juan Linietsky
 **/
class Tracker_Voice : public Voice
{

	struct Info {

		bool active;            /* =1 -> sample is playing */
		//UWORD     flags;             /* 16/8 bits looping/one-shot */

                Sample_Data *sample_data_ptr;
		
		bool playing_backwards;

		size_t start_index;             /* start index */
		size_t fixedpoint_offset;	// An offset expressed in 1/2048th of a sample.
		Uint32 current_frequency;  //frq;               /* current frequency */

		float volume;               /* current volume */
		int panning;               /* current panning position */
	
		//int rampvol;
		float lvolsel,rvolsel;
		float oldlvol,oldrvol;

		ptrdiff_t current_index;           /* current index in the sample */
		
		Uint16 reverb_send;
  		Uint16 chorus_send;

		bool first_mix;
		bool sample_needs_ramp_up;

		//filter
		struct Filter {
			IIR_SVF::Coeffs coeffs;
			Sint32 prev,hist1,hist2;

			bool enabled;
		} filter;

		void clear() {

			active=playing_backwards=false;
			start_index=current_frequency=0;
			fixedpoint_offset=0;
			chorus_send=reverb_send=0;
			volume=lvolsel=rvolsel=oldlvol=oldrvol=0;
			panning=current_index=0;
                        sample_data_ptr=NULL;
			first_mix=true;
                        filter.enabled=false;
			sample_needs_ramp_up=false;
			filter.prev=filter.hist1=filter.hist2=0;

		}

		void restart() {

			playing_backwards=false;
			current_index=start_index;
			fixedpoint_offset=0;
			if(sample_data_ptr) {
				// NOTE: sample_data_ptr IS ALREADY LOCKED.
				current_index=sample_data_ptr->seek(current_index);
				if(sample_data_ptr->fixedpoint_is_backwards()) {
					sample_data_ptr->fixedpoint_aboutface();
				}
			}
			
			active=true;
			first_mix=true;
		}

		Info() {

			clear();

		}
	} info;


	Resampler_Instance resampler;

	size_t channel_index;
	int current_prio;
	size_t mixfreq;
	float preamp;

	size_t idxsize,loop_begin,loop_end; // index size/begin/end for fast loop/pos check

	//virtuals
	virtual Priority get_priority(); //finally, this is proovided to poll priority.
	virtual int get_channel();
	virtual void set_mix_frequency(float p_mixfreq);
	virtual void set_preamp(float p_preamp);
	virtual void mix(size_t p_amount,sample_t* p_where, bool use_mutex=true); //set where to mix

	void add_to_mix_buffer(size_t p_amount,sample_t *p_buffer);


	bool can_mix();
public: /* MIXING! */

	//buffer
	void set_global_effect_send(int p_buffer,int p_amount); //amount is 0-255
	void set_global_effect_buffer(int p_buffer,sample_32s_t* p_location); //effect buffer_location
	bool is_active();

	//control

	void set_frequency(Uint32 p_freq);
	void set_panning(int p_pan);
	void set_volume(int p_vol);
        void set_filter(const IIR_SVF::Coeffs& p_coeffs,bool p_enabled);

	int set_channel(int p_channel);

	bool has_stopped();

	int get_current_freq();
	int get_current_sample_pos();


	void set_priority(int p_prio);
	void setup_sample(Sample_Data *p_sdata,size_t p_offset);

	void reset();

  Tracker_Voice();
  ~Tracker_Voice();
};

#endif
