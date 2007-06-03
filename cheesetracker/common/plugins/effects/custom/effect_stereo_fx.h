//
// C++ Interface: effect_stereo_fx
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EFFECT_STEREO_FX_H
#define EFFECT_STEREO_FX_H

#include "common/components/audio/effect.h"
#include <vector>
	using std::vector;
	
#include "plugins/effects/custom/custom_fx_param_list.h"

/**
@author Juan Linietsky
*/
class Effect_Stereo_FX : public Effect {

 	enum Params {
		
		PARAM_DRY_SEPARATION,
		PARAM_DRY_COMPENSATION,
		PARAM_DRY_LEVEL,
		PARAM_FEEDBACK_INPUT_TYPE,
		PARAM_FEEDBACK_INPUT_STEREO,
		PARAM_FEEDBACK_LEVEL,
		PARAM_FEEDBACK_DELAY_TIME,
		PARAM_FEEDBACK_DELAY_SEPARATION
				
	};

	enum {
		
		BUFFER_SIZE=(1<<16)
	};
	
	
	vector<sample_t> buffer; //this buffer is mono.. there is no point in making it stereo
	unsigned int buffer_pos;
	int wrap_size;

	Custom_FX_Param_List *parameters;

	void check_buffer_length();
	
	Effect_Stereo_FX();
	Effect_Stereo_FX(Effect::Parameters *p_params);	
		
public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length);

        static Effect * create_this(Effect::Parameters *p_params) { return new Effect_Stereo_FX(p_params); }
        static Effect::Parameters * create_parameters();
	
};

#endif
