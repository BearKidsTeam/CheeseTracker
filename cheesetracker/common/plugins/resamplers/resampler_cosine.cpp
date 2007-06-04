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
#include "resampler_cosine.h"
#include "helpers.h"
#include <cmath>

//who says templates are useless? :)
template <class Depth,bool FILTER_VERSION>
static void mix_cosine(Resampler::Mix_Data *mixdata,Sint32 *p_table, bool cosine_mode) {

	HELPER_INITIALIZE

	if (CHECK_END_OF_SAMPLE_AT_MIX_BEGIN) {
		samples_to_mix-=sample_increment_threshold;
		int aux_samples_to_mix=sample_increment_threshold;

		while (aux_samples_to_mix--) {
			HELPER_MIX_ONE_RAW_SAMPLE
		};
	}
	bool end_of_sample_at_mix_end=CHECK_END_OF_SAMPLE_AT_MIX_END;
	if (end_of_sample_at_mix_end) {
		// printf("end of sample at mix end!\n");
		samples_to_mix-=sample_increment_threshold;
	}

	while(samples_to_mix--) {
		// Mon Apr 30 01:14:44 EDT 2007
		// Cosine-based sample hacking moved into the
		// Sample_Data because it depended very heavily
		// on the fixed-point part of the current position
		// in the sample, which I really, really want
		// to keep confined to the Sample_Data implementation.

		HELPER_MIX_ONE_SAMPLE( {
			mixdata->sample->get_sample_for_cosine_mixer(temp_float, cosine_mode);
		})
	}

	if (end_of_sample_at_mix_end) {
		samples_to_mix=sample_increment_threshold;
		while (samples_to_mix--) {
			HELPER_MIX_ONE_RAW_SAMPLE
		};
	}
	HELPER_UPDATE_MIXDATA_STATUS
}


void Resampler_Cosine::mix(Resampler::Mix_Data *p_data) {

//	//printf("HOHO volumes %i,%i,\n",p_data->l_volume,p_data->r_volume);
	if (p_data->sample->is_16bit()) {

		if (p_data->filter.enabled) {

			mix_cosine<Sint16,true>(p_data,frac_cosine,mode == COSINE);
		} else {

			mix_cosine<Sint16,false>(p_data,frac_cosine,mode == COSINE);
		}
	} else {

		if (p_data->filter.enabled) {

			mix_cosine<Sint8,true>(p_data,frac_cosine, mode == COSINE);
		} else {

			mix_cosine<Sint8,false>(p_data,frac_cosine, mode == COSINE);
  }
	
	}
}

string Resampler_Cosine::get_name() {

	if (mode==COSINE)
		return "Cosine";
	else
		return "FM";
}

bool Resampler_Cosine::has_interpolation() {

	return true;
}

Resampler_Cosine::Resampler_Cosine(Mode p_mode) {

	mode =p_mode;
	for (int i=0;i<COSINE_LEN;i++) {

		float x=(float)i*M_PI/(float)COSINE_LEN;

		float cosval=(p_mode==COSINE)?cos(x):cos(cos(x)*x);
		frac_cosine[i]=int( (1.0-cosval) * 0.5 * (float)COSINE_LEN);
	}
}

