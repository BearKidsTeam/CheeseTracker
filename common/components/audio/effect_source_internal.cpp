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
#include "effect_source_internal.h"


void Effect_Source_Internal::register_effect_constructor( string p_name,string p_var_name,Effect*(*p_effect_constructor)(Effect::Parameters*),Effect::Parameters*(*p_effect_params_constructor)() ) {

	Constructor new_constructor;
	new_constructor.constructor_ptr=p_effect_constructor;
	new_constructor.params_constructor_ptr=p_effect_params_constructor;
	new_constructor.name=p_name;
	new_constructor.var_name=p_var_name;
	effect_constructor_list.push_back(new_constructor);

}

int Effect_Source_Internal::get_effect_count() {

	return effect_constructor_list.size();
}

string Effect_Source_Internal::get_effect_display_name(int p_which) {

	if ((p_which<0) || (p_which>=effect_constructor_list.size())) {

		return "<INVALID>";
	}

	return effect_constructor_list[p_which].name;

}

string Effect_Source_Internal::get_effect_identifier_name(int p_which) {

	if ((p_which<0) || (p_which>=effect_constructor_list.size())) {

		return "<INVALID>";
	}
	return effect_constructor_list[p_which].var_name;
}

Effect *Effect_Source_Internal::instantiate_effect(int p_which,Effect::Parameters *p_params) {

	if ((p_which<0) || (p_which>=effect_constructor_list.size())) {

		return NULL;
	}
	Effect* (*caller)(Effect::Parameters *);
	caller=effect_constructor_list[p_which].constructor_ptr;

	return (caller)(p_params);

}

Effect::Parameters *Effect_Source_Internal::instantiate_effect_parameters(int p_which) {

	if ((p_which<0) || (p_which>=effect_constructor_list.size())) {

		return NULL;
	}
	Effect::Parameters* (*caller)();
	caller=effect_constructor_list[p_which].params_constructor_ptr;

	return (caller)();

}

string Effect_Source_Internal::get_name() {

	return "Internal";
}

Effect_Source_Internal::Effect_Source_Internal() {

}


Effect_Source_Internal::~Effect_Source_Internal() {

}


