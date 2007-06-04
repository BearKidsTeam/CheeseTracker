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
#include <cstdio>
#include "resampler_linear.h"
#include "helpers.h"

//who says templates are useless? :)
template <class Depth,bool FILTER_VERSION>
static void mix_linear(Resampler::Mix_Data *mixdata) {

	HELPER_INITIALIZE
	// Uint32 fractional_mask=(1L<<fractional_size)-1L;
	//sample_t next_index;

	if (CHECK_END_OF_SAMPLE_AT_MIX_BEGIN) {
		samples_to_mix-=sample_increment_threshold;
		int aux_samples_to_mix=sample_increment_threshold;

		while (aux_samples_to_mix--) {
			HELPER_MIX_ONE_RAW_SAMPLE
		};
	}
	bool end_of_sample_at_mix_end=CHECK_END_OF_SAMPLE_AT_MIX_END;
	if (end_of_sample_at_mix_end) {
		////printf("end of sample at mix end!\n");
		samples_to_mix-=sample_increment_threshold;
	}

	try {

		while(samples_to_mix--) {
			//very simple raw resampling

			// final_data=data_ptr[real_index];
			//
			// Mon Apr 30 02:31:15 EDT 2007
			//
			//
			// final_data=final_data+((data_ptr[real_index+1]-final_data)*Sint32(segment_offset&fractional_mask) >> fractional_size);

			HELPER_MIX_ONE_SAMPLE({ 
				mixdata->sample->get_sample_for_linear_mixer(temp_float);
			})

		}

		if (end_of_sample_at_mix_end) {
			samples_to_mix=sample_increment_threshold;
			while (samples_to_mix--) {
				HELPER_MIX_ONE_RAW_SAMPLE
			};
		}
	} catch (Sample_EOF_Error E) {
		return;
	}
	HELPER_UPDATE_MIXDATA_STATUS
}


void Resampler_Linear::mix(Resampler::Mix_Data *p_data) {

//	//printf("HOHO volumes %i,%i,\n",p_data->l_volume,p_data->r_volume);
	if (p_data->sample->is_16bit()) {

		if (p_data->filter.enabled) {

			mix_linear<Sint16,true>(p_data);
		} else {

			mix_linear<Sint16,false>(p_data);
		}
	} else {

		if (p_data->filter.enabled) {

			mix_linear<Sint8,true>(p_data);
		} else {
		
			mix_linear<Sint8,false>(p_data);
		}
	
	}
}

string Resampler_Linear::get_name() {

	return "Linear";
}

bool Resampler_Linear::has_interpolation() {

	return true;
}

