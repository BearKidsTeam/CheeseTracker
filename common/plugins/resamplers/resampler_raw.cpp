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
#include "resampler_raw.h"
#include "helpers.h"

DEFINE_GETTER(raw_getter, { data_ptr->get_f_sample(output); });

//who says templates are useless? :)
template <bool FILTER_VERSION>
static void mix_raw(Resampler::Mix_Data *mixdata) {
	raw_getter get_raw(mixdata->sample);
	mix_sample(mixdata, &get_raw, FILTER_VERSION);
}


void Resampler_Raw::mix(Resampler::Mix_Data *p_data) {

//	//printf("HOHO volumes %i,%i,\n",p_data->l_volume,p_data->r_volume);
	if (p_data->filter.enabled) {
		mix_raw<true>(p_data);
	} else {
		mix_raw<false>(p_data);
	}
}

string Resampler_Raw::get_name() {

	return "Raw";
}

bool Resampler_Raw::has_interpolation() {

	return false;
}
