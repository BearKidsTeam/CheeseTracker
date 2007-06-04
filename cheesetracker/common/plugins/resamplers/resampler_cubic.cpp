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
#include "resampler_cubic.h"
#include "helpers.h"

/* Code adapted from modplug-xmms, originally Public Domain */

#define SPLINE_QUANTBITS	14
#define SPLINE_QUANTSCALE	(1L<<SPLINE_QUANTBITS)
#define SPLINE_8SHIFT		(SPLINE_QUANTBITS-8)
#define SPLINE_16SHIFT		(SPLINE_QUANTBITS)
// forces coefsset to unity gain
#define SPLINE_CLAMPFORUNITY
// log2(number) of precalculated splines (range is [4..14])
#define SPLINE_FRACBITS 8 // ?
#define SPLINE_LUTLEN (1L<<SPLINE_FRACBITS)

// LUT = "Look Up Table"
//
// See Resampler_Cubic::Resampler_Cubic.

static signed short lut[4*(1L<<SPLINE_FRACBITS)];

//who says templates are useless? :)
template <class Depth,bool FILTER_VERSION>
static void mix_linear(Resampler::Mix_Data *mixdata) {

	HELPER_INITIALIZE

	try {
		//this very much fucking sucks, I need to find a better way..
		if (CHECK_END_OF_SAMPLE_AT_MIX_BEGIN) {
			samples_to_mix-=sample_increment_threshold;
			int aux_samples_to_mix=sample_increment_threshold;

			while (aux_samples_to_mix--) {
				HELPER_MIX_ONE_RAW_SAMPLE
			}
		}
		bool end_of_sample_at_mix_end=CHECK_END_OF_SAMPLE_AT_MIX_END;
		if (end_of_sample_at_mix_end) {
			////printf("end of sample at mix end!\n");
			samples_to_mix-=sample_increment_threshold;
		}

		// The meat of the cubic algorithm is in the Sample_Data class
		// because it depends on private members of that class.

		while(samples_to_mix--) {

			HELPER_MIX_ONE_SAMPLE({
				mixdata->sample->do_cubic_mixer_voodoo(temp_float); 
			})
		}

		if (end_of_sample_at_mix_end) {
			samples_to_mix=sample_increment_threshold;
			while (samples_to_mix--) {
				HELPER_MIX_ONE_RAW_SAMPLE
			};
		}
		HELPER_UPDATE_MIXDATA_STATUS
	} catch (Sample_EOF_Error E) {
		return;
	}
}


void Resampler_Cubic::mix(Resampler::Mix_Data *p_data) {

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

string Resampler_Cubic::get_name() {

	return "Cubic";
}

bool Resampler_Cubic::has_interpolation() {

	return true;
}


Resampler_Cubic::Resampler_Cubic() {

	int length		= (1L<<SPLINE_FRACBITS);
	float length_f	= 1.0f / (float)length;
	float scale	= (float)SPLINE_QUANTSCALE;
	for(int i=0;i<length;i++)
	{	float cm1, c0, c1, c2;
		float x		= ((float)i)*length_f;
		int sum,idx	= i<<2;
		cm1			= (float)floor( 0.5 + scale * (-0.5*x*x*x + 1.0 * x*x - 0.5 * x       ) );
		c0			= (float)floor( 0.5 + scale * ( 1.5*x*x*x - 2.5 * x*x             + 1.0 ) );
		c1			= (float)floor( 0.5 + scale * (-1.5*x*x*x + 2.0 * x*x + 0.5 * x       ) );
		c2			= (float)floor( 0.5 + scale * ( 0.5*x*x*x - 0.5 * x*x                   ) );
		lut[idx+0]	= (signed short)( (cm1 < -scale) ? -scale : ((cm1 > scale) ? scale : cm1) );
		lut[idx+1]	= (signed short)( (c0  < -scale) ? -scale : ((c0  > scale) ? scale : c0 ) );
		lut[idx+2]	= (signed short)( (c1  < -scale) ? -scale : ((c1  > scale) ? scale : c1 ) );
		lut[idx+3]	= (signed short)( (c2  < -scale) ? -scale : ((c2  > scale) ? scale : c2 ) );
#ifdef SPLINE_CLAMPFORUNITY
		sum			= lut[idx+0]+lut[idx+1]+lut[idx+2]+lut[idx+3];
		if( sum != SPLINE_QUANTSCALE )
		{	int max = idx;
			if( lut[idx+1]>lut[max] ) max = idx+1;
			if( lut[idx+2]>lut[max] ) max = idx+2;
			if( lut[idx+3]>lut[max] ) max = idx+3;
			lut[max] += (SPLINE_QUANTSCALE-sum);
		}
#endif
	}
}


