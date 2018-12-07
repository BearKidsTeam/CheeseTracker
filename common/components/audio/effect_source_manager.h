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
#ifndef EFFECT_SOURCE_MANAGER_H
#define EFFECT_SOURCE_MANAGER_H

#include "components/audio/effect_source.h"

#include <vector>
	using std::vector;
/**
@author Juan Linietsky
*/
class Effect_Source_Manager {

	vector<Effect_Source*> effect_source_list;
        static Effect_Source_Manager * singleton_instance;
public:

	Effect *instantiate_effect_by_name(string p_name,Effect::Parameters *p_params);
	Effect::Parameters *instantiate_effect_parameters_by_name(string p_name);
	static Effect_Source_Manager * get_singleton_instance();
	int get_effect_source_count();
	Effect_Source* get_effect_source(int p_which);
	string get_effect_source_name(int p_which);


	void register_effect_source(Effect_Source *p_src);


	Effect_Source_Manager();
	~Effect_Source_Manager();

};

#endif
