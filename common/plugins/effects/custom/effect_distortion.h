//
// C++ Interface: effect_distortion
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EFFECT_DISTORTION_H
#define EFFECT_DISTORTION_H

#include "common/components/audio/effect.h"
#include "plugins/effects/custom/custom_fx_param_list.h"
#include <vector>
	using std::vector;
/**
@author Juan Linietsky
*/
class Effect_Distortion : public Effect{


	enum Parameters {
		PARAM_DRIVE,
		PARAM_TYPE,
	};
	
	enum DistortionType {
		POWER,
		ATAN,
		LIMIT,
		ILIMIT,
		DIFF,
		DECIMATOR,
		MAX_DISTORTIONS
	};

	float old_r,old_l;		
	
	Custom_FX_Param_List *params;

	void set_sampling_rate(int p_rate) {}
	

	Effect_Distortion();
	Effect_Distortion(Effect::Parameters *p_params);	
public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length);

        static Effect * create_this(Effect::Parameters *p_params) { return new Effect_Distortion(p_params); }
        static Effect::Parameters * create_parameters();


	~Effect_Distortion();

};


#endif
