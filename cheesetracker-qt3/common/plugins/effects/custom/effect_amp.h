//
// C++ Interface: effect_amp
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EFFECT_AMP_H
#define EFFECT_AMP_H


#include "common/components/audio/effect.h"
#include "plugins/effects/custom/custom_fx_param_list.h"

/**
@author Juan Linietsky
*/
class Effect_Amp : public Effect{


	Custom_FX_Param_List *params;
	
	void set_sampling_rate(int p_rate) {}

	Effect_Amp();
	Effect_Amp(Effect::Parameters *p_params);	
public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length);

        static Effect * create_this(Effect::Parameters *p_params) { return new Effect_Amp(p_params); }
        static Effect::Parameters * create_parameters();


	~Effect_Amp();

};

#endif
