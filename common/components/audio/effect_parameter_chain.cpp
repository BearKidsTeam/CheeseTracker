//
// C++ Implementation: effect_parameter_chain
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "effect_parameter_chain.h"

#include "effect_source_manager.h"

void Effect_Parameter_Chain::copy_from(Effect_Parameter_Chain *p_lval) {

	reset();

	for (int i=0; i<p_lval->get_effect_count(); i++) {

		Effect::Parameters* rparameter = p_lval->get_effect(i);
		Effect::Parameters* par = Effect_Source_Manager::get_singleton_instance()->instantiate_effect_parameters_by_name(rparameter->get_identifier_name());
		par->copy_from(rparameter);

		add_effect(par);
	};

	chain_changed_signal.emit();
}


void Effect_Parameter_Chain::add_effect(Effect::Parameters *p_effect) {

	chain.push_back(p_effect);
	
	chain_changed_internal_signal.emit();
	chain_changed_signal.emit();
	
}

int Effect_Parameter_Chain::get_effect_count() {
	
	return chain.size();	
}

Effect::Parameters* Effect_Parameter_Chain::get_effect(int p_which) {
	
	if ((p_which<0) ||  (p_which>=chain.size()))
		return NULL;

	return chain[p_which];	
}

bool Effect_Parameter_Chain::remove_effect(int p_which) {

	if ((p_which<0) ||  (p_which>=chain.size()))
		return false;

	delete chain[p_which];
	chain.erase(chain.begin()+p_which);
	
	chain_changed_internal_signal.emit();
	chain_changed_signal.emit();
	
	return true;
}


bool Effect_Parameter_Chain::move_effect_up(int p_which) {

	if (p_which<1) return false;
	if (p_which>=chain.size()) return false;

	Effect::Parameters *fx=chain[p_which-1];
	chain[p_which-1]=chain[p_which];
	chain[p_which]=fx;

	chain_changed_internal_signal.emit();
	chain_changed_signal.emit();
	
	return true;
}

bool Effect_Parameter_Chain::move_effect_down(int p_which) {

	if (p_which<0) return false;
	if (p_which>=(chain.size()-1)) return false;

	Effect::Parameters *fx=chain[p_which+1];
	chain[p_which+1]=chain[p_which];
	chain[p_which]=fx;
	
	chain_changed_internal_signal.emit();
	chain_changed_signal.emit();

	return true;
}


Effect_Parameter_Chain::Effect_Parameter_Chain() {
}


Effect_Parameter_Chain::~Effect_Parameter_Chain() {
	
	chain_deleted_internal_signal.emit();
	
	for (int i=0;i<chain.size();i++) {
		delete chain[i];
	}	
}


void Effect_Parameter_Chain::reset() {

	while (get_effect_count()) {
		remove_effect(0);
	};
}
