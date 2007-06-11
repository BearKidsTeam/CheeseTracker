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
#ifndef _RESAMPLER_H_
#define _RESAMPLER_H_

#include "sample_data.h"
#include "iir_svf.h"

/**
 *
 * Juan Linietsky
 **/
class Resampler {
public:

	struct Mix_Data {

		int amount;
		sample_t *dst_buff;
		Sample_Data *sample;


		Sint64 sample_offset;
		Sint32 increment;
		Sint32 samples_to_mix;
		int panning;
		size_t mixfreq;
		size_t current_frequency;

		float l_volume;
		float r_volume;
		//volume ramp data
		float l_volume_prev;
		float r_volume_prev;

		struct Filter {

			bool enabled;
                        IIR_SVF::Coeffs coeffs;
			float hist_a1,hist_a2,hist_b1,hist_b2; //prev is previous input, hist and hist 2 are filter history
		} filter;
		class Fraction {
			public:
                        	size_t num;
                        	size_t den;
                        	size_t operator *(size_t);
			}increment_fraction;

	};


	virtual void mix(Mix_Data *p_data)=0;


	virtual string get_name()=0;
	virtual bool has_interpolation()=0; //it's good to distingish them!
	

	Resampler();
	virtual ~Resampler();
};

#endif
