//
// C++ Interface: effect_echo
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EFFECT_PSHIFT_H
#define EFFECT_PSHIFT_H

#include "common/components/audio/effect.h"
#include <vector>
	using std::vector;
	
#include "plugins/effects/custom/custom_fx_param_list.h"
/**
@author Juan Linietsky
*/
class Effect_PShift : public Effect {


	enum Params {
                PARAM_SHIFT,
		PARAM_BLOCKSIZE
	};

	enum {
		
		BUFFER_SIZE=(1<<16)
	};
	
	
	vector<sample_t> buffer;
	unsigned int buffer_pos;
	int wrap_size;


	int cur_bsize;
        float cur_shift;
        float fade_threshold;
	float pre_peak_dB_max;
	float pre_peak_dB_min;


        unsigned int sample_counter;

	Custom_FX_Param_List *parameters;
	void check_buffer_length();
	
	Effect_PShift();
	Effect_PShift(Effect::Parameters *p_params);	
public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length);

        static Effect * create_this(Effect::Parameters *p_params) { return new Effect_PShift(p_params); }
        static Effect::Parameters * create_parameters();


	~Effect_PShift();

};

#endif
