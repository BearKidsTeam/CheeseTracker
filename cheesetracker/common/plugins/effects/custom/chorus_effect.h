//
// C++ Interface: chorus_effect
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CHORUS_EFFECT_H
#define CHORUS_EFFECT_H

#include "plugins/effects/freeverb/revmodel.h"
#include "common/components/audio/effect.h"
#include <vector>
	using std::vector;
	
#include "plugins/effects/custom/custom_fx_param_list.h"
/**
@author Juan Linietsky
*/
class Chorus_Effect : public Effect {

	enum {

		BUFFER_EXTRA_ALLOCATON=30000,
		RINBUFFER_BITS=16,
		RINGBUFFER_LEN=(1<<RINBUFFER_BITS),
		RINGBUFFER_MASK=(RINGBUFFER_LEN-1)
				
	};

	enum ParamNames {
		PARAM_VOICES,
		PARAM_DELAY,
		PARAM_SPEED,
		PARAM_DEPTH,
		PARAM_WET,
		PARAM_DRY,
		PARAM_WIDTH			
	};

   	vector<float> ringbuffer;

	unsigned int buffer_index;
	double counter;
	Custom_FX_Param_List * params;

	Chorus_Effect();
	Chorus_Effect(Effect::Parameters *p_params);
public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length);

        static Effect * create_this(Effect::Parameters *p_params) { return new Chorus_Effect(p_params); }
        static Effect::Parameters * create_parameters();

	~Chorus_Effect();

};

#endif
