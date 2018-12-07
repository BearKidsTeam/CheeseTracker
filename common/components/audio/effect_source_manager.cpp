//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "effect_source_manager.h"

Effect_Source_Manager * Effect_Source_Manager::singleton_instance=NULL;

Effect_Source_Manager * Effect_Source_Manager::get_singleton_instance() {

	return singleton_instance;
}

Effect *Effect_Source_Manager::instantiate_effect_by_name(string p_name,Effect::Parameters *p_params) {

	for (int i=0;i<effect_source_list.size();i++) {

		for (int j=0;j<effect_source_list[i]->get_effect_count();j++) {

			if (effect_source_list[i]->get_effect_identifier_name(j)==p_name) {

				return effect_source_list[i]->instantiate_effect(j,p_params);
			}
		}
	}

	return NULL; //not found
}

Effect::Parameters *Effect_Source_Manager::instantiate_effect_parameters_by_name(string p_name) {

	for (int i=0;i<effect_source_list.size();i++) {

		for (int j=0;j<effect_source_list[i]->get_effect_count();j++) {

			if (effect_source_list[i]->get_effect_identifier_name(j)==p_name) {

				return effect_source_list[i]->instantiate_effect_parameters(j);
			}
		}
	}

	return NULL; //not found
}

int Effect_Source_Manager::get_effect_source_count() {

	return effect_source_list.size();
}


Effect_Source* Effect_Source_Manager::get_effect_source(int p_which) {

	if ((p_which<0) || (p_which>=effect_source_list.size()))
		return NULL;

	return effect_source_list[p_which];
}


string Effect_Source_Manager::get_effect_source_name(int p_which) {

	if ((p_which<0) || (p_which>=effect_source_list.size()))
		return "<ERROR>";

	return effect_source_list[p_which]->get_name();


}


void Effect_Source_Manager::register_effect_source(Effect_Source *p_src) {

	effect_source_list.push_back(p_src);
}

Effect_Source_Manager::Effect_Source_Manager() {

	if (singleton_instance) {
		ERROR("Singleton instance exists!!!");
		return;
	}
	singleton_instance=this;
}


Effect_Source_Manager::~Effect_Source_Manager()
{
}


