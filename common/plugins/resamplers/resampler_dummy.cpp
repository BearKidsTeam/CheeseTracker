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
#include "resampler_dummy.h"
#include "helpers.h"


//who says templates are useless? :)
template <class Depth,bool FILTER_VERSION>
static void mix_raw(Resampler::Mix_Data *mixdata) {

	HELPER_INITIALIZE
	HELPER_BEGIN_LOOP
		//very simple raw resampling
	HELPER_END_LOOP
	HELPER_UPDATE_MIXDATA_STATUS
}


void Resampler_Dummy::mix(Resampler::Mix_Data *p_data) {

	return;
//	printf("HOHO volumes %i,%i,\n",p_data->l_volume,p_data->r_volume);
	if (p_data->sample->is_16bit()) {

		if (p_data->filter.enabled) {

			mix_raw<Sint16,true>(p_data);
		} else {

			mix_raw<Sint16,false>(p_data);
		}
	} else {

		if (p_data->filter.enabled) {
		
			mix_raw<Sint8,true>(p_data);
		} else {
		
			mix_raw<Sint8,false>(p_data);
		}
	
	}
}

string Resampler_Dummy::get_name() {

	return "None";
}

bool Resampler_Dummy::has_interpolation() {

	return false;
}
