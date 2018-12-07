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
#ifndef EFFECT_STEREO_ENHANCER_H
#define EFFECT_STEREO_ENHANCER_H

#include "common/components/audio/effect.h"
#include <vector>
	using std::vector;
/**
@author Juan Linietsky
*/
class Effect_Stereo_Enhancer : public Effect {

	int delay;
	int feedback;

	Int_Property_Bridge prop_delay;
	Int_Property_Bridge prop_feedback;

	int mix_freq;
	vector<sample_t> buffer;
	unsigned int buffer_pos;

	void check_buffer_length();
public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length,bool p_stereo_samples,int p_buffer_freq);
	string get_name();
	string get_display_name();
	list<Property_Bridge*> get_properties();

        static Effect * create_this() { return new Effect_Stereo_Enhancer; }

	Effect_Stereo_Enhancer();
	~Effect_Stereo_Enhancer();

};

#endif
