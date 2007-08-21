//
// C++ Interface: dummy_effect
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DUMMY_EFFECT_H
#define DUMMY_EFFECT_H


#include "components/audio/effect.h"

/**
@author Juan Linietsky
*/
class Dummy_Effect : public Effect {

	int a,b;
	Int_Property_Bridge int1,int2;
public:

	void reset();
        void process_buffer(sample_t* p_buffer,int p_length,bool p_stereo_samples,int p_freq);
	string get_name();
	string get_display_name();
	list<Property_Bridge*> get_properties();

	static Effect * create_this() { return new Dummy_Effect; }

	Dummy_Effect();
	~Dummy_Effect();

};

#endif
