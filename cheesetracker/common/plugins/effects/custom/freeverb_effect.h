//
// C++ Interface: freeverb_effect
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FREEVERB_EFFECT_H
#define FREEVERB_EFFECT_H

#include "plugins/effects/freeverb/revmodel.h"
#include "common/components/audio/effect.h"
#include <vector>
	using std::vector;
	
#include "plugins/effects/custom/custom_fx_param_list.h"
/**
@author Juan Linietsky
*/
class Freeverb_Effect : public Effect {

	revmodel freeverb; //freeverb object

	
	enum ParamNames {
		
		PARAM_ROOM_SIZE,
		PARAM_DAMP,
		PARAM_WET,
		PARAM_DRY,
		PARAM_WIDTH
	};
		
	struct Params {
		float room_size;
		float damp;
		float wet;
		float dry;
		float width;
	} current_parm,bridge_parm;

	enum {
		INTERNAL_BUFFER_SIZE=8192
	};

        vector<float> buffer_in_r;
        vector<float> buffer_in_l;
        vector<float> buffer_out_r;
        vector<float> buffer_out_l;

	void check_params_changed();

	Freeverb_Effect(Effect::Parameters *p_parameters);
	
	Custom_FX_Param_List * parameters;
	
public:
	void reset();
        void process_buffer(sample_t* p_buffer,int p_length);

        static Effect * create_this(Effect::Parameters *p_parameters) { return new Freeverb_Effect(p_parameters); }
        static Effect::Parameters * create_parameters();


	~Freeverb_Effect();

};

#endif
