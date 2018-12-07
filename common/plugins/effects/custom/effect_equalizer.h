//
// C++ Interface: effect_equalizer
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EFFECT_EQUALIZER_H
#define EFFECT_EQUALIZER_H

 #include "plugins/effects/custom/custom_fx_param_list.h"
#include "common/components/audio/effect.h"
#include <vector>
	using std::vector;
/**
@author Juan Linietsky
*/
class Effect_Equalizer : public Effect {

public:

	enum Band_Count {
		BANDS_6,
		BANDS_10,
		BANDS_21,
		BANDS_31
	};

private:
	struct Band {

		float c1,c2,c3;
		struct History {
			float a1,a2,a3;
			float b1,b2,b3;
		} ht_l,ht_r; //history
		Band();
	};

	vector<Band*> bands;

	Band_Count eq_type;

	double mix_freq;
	
	Custom_FX_Param_List *parameters;

	Effect_Equalizer(Effect::Parameters *p_params);	
        static Effect::Parameters * create_parameters(Band_Count p_count); 	
public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length);

        static Effect::Parameters * create_parameters_6bands()  { return create_parameters(BANDS_6); }
        static Effect::Parameters * create_parameters_10bands() { return create_parameters(BANDS_10); }
        static Effect::Parameters * create_parameters_21bands() { return create_parameters(BANDS_21); }
        static Effect::Parameters * create_parameters_31bands() { return create_parameters(BANDS_31); }
	

        static Effect * create_this(Effect::Parameters *p_params) { return new Effect_Equalizer(p_params); }	

        
        
	
	

	~Effect_Equalizer();

};

#endif
