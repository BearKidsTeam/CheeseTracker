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
#ifndef EFFECT_DISTORT_H
#define EFFECT_DISTORT_H


#include "common/components/audio/effect.h"

/**
@author Juan Linietsky
*/
class Effect_Distort : public Effect{

	int percent;
	int clip;

	Int_Property_Bridge prop_percent;
	Int_Property_Bridge prop_clip;

public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length,bool p_stereo_samples,int p_buffer_freq);
	string get_name();
	string get_display_name();
	list<Property_Bridge*> get_properties();

        static Effect * create_this() { return new Effect_Distort; }

	Effect_Distort();
	~Effect_Distort();

};

#endif
