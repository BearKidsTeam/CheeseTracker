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

#ifndef _RESAMPLER_INSTANCE_H_
#define _RESAMPLER_INSTANCE_H_

#include "resampler_manager.h"
/**
 *
 * Juan Linietsky
 **/
class Resampler_Instance {


	Resampler_Manager * resampler_manager;
	int current_resampler;

	Resampler::Mix_Data mixdata;

public:

	Resampler::Mix_Data & get_mixdata()  { return mixdata; }

	enum {
		DEFAULT_RESAMPLER=-1,
	};

	inline void set_current_resampler(int p_index);
	inline void mix();

	inline void set_sample_data(Sample_Data *p_sdata);

	inline void set_dest_buffer(sample_t *p_buffer);

	inline void set_sample_offset(Sint64 p_offset);
	inline void set_increment(Sint64 p_increment);
	inline void set_samples_to_mix(int p_amount);
	inline void set_l_volume(float p_volume);
	inline void set_r_volume(float p_volume);
	inline void set_previous_l_volume(float p_volume);
	inline void set_previous_r_volume(float p_volume);

	inline Sint64 get_sample_offset();


	inline void set_filter(bool p_enabled,const IIR_SVF::Coeffs &p_coeffs );

	Resampler_Instance();
	~Resampler_Instance();
};

inline void Resampler_Instance::set_sample_data(Sample_Data *p_sdata) {

	 mixdata.sample=p_sdata;

}
inline void Resampler_Instance::set_dest_buffer(sample_t *p_buffer) {

	 mixdata.dst_buff=p_buffer;

}

inline void Resampler_Instance::set_sample_offset(Sint64 p_offset) {

	 mixdata.sample_offset=p_offset;


}
inline void Resampler_Instance::set_increment(Sint64 p_increment) {

	 mixdata.increment=p_increment;


}

inline void Resampler_Instance::set_samples_to_mix(int p_amount) {

	 mixdata.samples_to_mix=p_amount;

}

inline void Resampler_Instance::set_l_volume(float p_volume) {

	 mixdata.l_volume=p_volume;

}
inline void Resampler_Instance::set_r_volume(float p_volume) {

	 mixdata.r_volume=p_volume;


}
inline void Resampler_Instance::set_previous_l_volume(float p_volume) {

	 mixdata.l_volume_prev=p_volume;


}
inline void Resampler_Instance::set_previous_r_volume(float p_volume) {

	 mixdata.r_volume_prev=p_volume;

}


inline void Resampler_Instance::set_filter(bool p_enabled,const IIR_SVF::Coeffs &p_coeffs) {


	 mixdata.filter.enabled=p_enabled;

	 mixdata.filter.coeffs=p_coeffs;


}

inline Sint64 Resampler_Instance::get_sample_offset() {

 	return mixdata.sample_offset;
}

inline void Resampler_Instance::set_current_resampler(int p_index) {

	if (current_resampler==p_index)
		return;

	if (p_index<0) {
		current_resampler=DEFAULT_RESAMPLER;
	} else if (p_index>=resampler_manager->get_resampler_count())  {
		throw Out_Of_Bounds(__FILE__, __LINE__, "Invalid resampler!");
	}

	current_resampler=p_index;
}

void Resampler_Instance::mix() {

	int resampler_idx=current_resampler;

	resampler_manager->get_resampler(resampler_idx)->mix(&mixdata);
}

#endif
