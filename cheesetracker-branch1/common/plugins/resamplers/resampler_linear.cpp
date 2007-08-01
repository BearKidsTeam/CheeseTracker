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
#include "resampler_kernel.h"
#include "resampler_linear.h"
#include "helpers.h"

DEFINE_GETTER(linear_getter, { data_ptr->get_sample_for_linear_mixer(output); });

//who says templates are useless? :)
template <bool FILTER_VERSION>
static void mix_linear(Resampler::Mix_Data *mixdata) {
	linear_getter get_linear(mixdata->sample);
	mix_sample(mixdata, &get_linear, FILTER_VERSION);
}


void Resampler_Linear::mix(Resampler::Mix_Data *p_data) {
	if (p_data->filter.enabled) {
		mix_linear<true>(p_data);
	} else {
		mix_linear<false>(p_data);
	}
}

string Resampler_Linear::get_name() {
	return "Linear";
}

bool Resampler_Linear::has_interpolation() {
	return true;
}

