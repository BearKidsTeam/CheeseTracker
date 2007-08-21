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
#ifndef EFFECT_ECHO_H
#define EFFECT_ECHO_H

#include "common/components/audio/effect.h"
#include <vector>
	using std::vector;
	
#include "plugins/effects/custom/custom_fx_param_list.h"
/**
@author Juan Linietsky
*/
class Effect_Echo : public Effect {

	int delay;
	int separation;
	int feedback;
	
	enum Params {
		
		PARAM_DELAY,
		PARAM_FEEDBACK,
		PARAM_SEPARATION
	};

	enum {
		
		BUFFER_SIZE=(1<<16)
	};
	
	
	vector<sample_t> buffer;
	unsigned int buffer_pos;
	int wrap_size;

	Custom_FX_Param_List *parameters;
	void check_buffer_length();
	
	Effect_Echo();
	Effect_Echo(Effect::Parameters *p_params);	
public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length);

        static Effect * create_this(Effect::Parameters *p_params) { return new Effect_Echo(p_params); }
        static Effect::Parameters * create_parameters();


	~Effect_Echo();

};

#endif
