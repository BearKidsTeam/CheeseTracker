//
// C++ Implementation: effect_amp
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "effect_amp.h"


void Effect_Amp::reset() {


}


void Effect_Amp::process_buffer(sample_t* p_buffer,int p_length) {

	float multiplier;
	multiplier=params->get_param_float(0);

	p_length*=2;

	while (p_length--) {

		*p_buffer*=multiplier;
		p_buffer++;
	}

}


Effect_Amp::Effect_Amp(Effect::Parameters *p_params) : Effect(p_params) {

	params=dynamic_cast<Custom_FX_Param_List*>(p_params);
	
	if (params==NULL) {
		ERROR("This will CRASH HOW DID IT HAPPEN???!!!!!!!");
	}
	
}


Effect_Amp::~Effect_Amp()
{
}


Effect::Parameters * Effect_Amp::create_parameters() {
	
	Custom_FX_Param_List * params= new Custom_FX_Param_List("Amplifier","INTERNAL: amplifier");

	params->add_float_param("Ratio","ratio",0.0,4.0,1.0);

	return params;	
}
