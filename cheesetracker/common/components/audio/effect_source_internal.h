//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EFFECT_SOURCE_INTERNAL_H
#define EFFECT_SOURCE_INTERNAL_H

#include "components/audio/effect_source.h"
#include <vector>
using std::vector;

/**
@author Juan Linietsky
*/
class Effect_Source_Internal : public Effect_Source {

	struct Constructor {

		Effect*(*constructor_ptr)(Effect::Parameters*);
		Effect::Parameters*(*params_constructor_ptr)();
		string name;
		string var_name;
	};
	vector<Constructor> effect_constructor_list;

public:
	int get_effect_count();
	string get_effect_identifier_name(int p_which);
	string get_effect_display_name(int p_which);
	Effect *instantiate_effect(int p_which,Effect::Parameters *p_params);
	Effect::Parameters *instantiate_effect_parameters(int p_which);
	string get_name();

	void register_effect_constructor( string p_name,string p_var_name,Effect*(*p_effect_constructor)(Effect::Parameters*),Effect::Parameters*(*p_effect_params_constructor)() );
	Effect_Source_Internal();
	~Effect_Source_Internal();
};

#endif
