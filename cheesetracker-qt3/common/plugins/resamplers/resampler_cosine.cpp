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
#include "resampler_kernel.h"
#include "resampler_cosine.h"
#include "helpers.h"
#include <cmath>

class cosine_getter : public sample_getter {
		Sample_Data *data_ptr;
		bool cosine_mode;
	public:
		cosine_getter(Sample_Data *data, bool cos_mode) {
			data_ptr = data;
			cosine_mode = cos_mode;
		}
		virtual void operator()(float *output) {
			data_ptr->get_sample_for_cosine_mixer(output, cosine_mode);
		}
		virtual ~cosine_getter() { };
};

//who says templates are useless? :)
template <bool FILTER_VERSION>
static void mix_cosine(Resampler::Mix_Data *mixdata,Sint32 *p_table, bool cosine_mode) {
	cosine_getter get_cosine(mixdata->sample, cosine_mode);
	mix_sample(mixdata, &get_cosine, FILTER_VERSION);
}


void Resampler_Cosine::mix(Resampler::Mix_Data *p_data) {
	if (p_data->filter.enabled) {
		mix_cosine<true>(p_data,frac_cosine,mode == COSINE);
	} else {
		mix_cosine<false>(p_data,frac_cosine,mode == COSINE);
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

