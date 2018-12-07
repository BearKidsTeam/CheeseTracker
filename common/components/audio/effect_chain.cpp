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
#include "effect_chain.h"
 #include "components/audio/effect_source_manager.h"

void Effect_Chain::process_buffer(sample_t *p_buffer,int p_length) {
	if (!chain_params)
		return;

	for (int i=0;i<chain.size();i++) {

		chain[i]->process_buffer(p_buffer,p_length);
	}
}

void Effect_Chain::update_chain() {
	
	if (!chain_params)
		return;
	
	for (int i=0;i<chain.size();i++) {
		delete chain[i];
	}	
	chain.clear();
	Effect_Source_Manager * esm = Effect_Source_Manager::get_singleton_instance();		
	
	for (int i=0;i<chain_params->get_effect_count();i++) {
		Effect* new_fx=NULL;
        	new_fx = esm->instantiate_effect_by_name( chain_params->get_effect( i )->get_identifier_name(), chain_params->get_effect( i ) );
		if (!new_fx) {
			ERROR("No effect of which I have parameter? this is WICKED!! - fx is: " << chain_params->get_effect( i )->get_identifier_name() );
			continue;
		}
		
		chain.push_back(new_fx);
		
	}	
}

void Effect_Chain::erase_chain() {
	
	for (int i=0;i<chain.size();i++) {
		delete chain[i];
	}	
	
	chain.clear();
	chain_connection_change.disconnect();
	chain_connection_delete.disconnect();
	chain_params=NULL;
		
}

void Effect_Chain::set_chain_params(Effect_Parameter_Chain *p_chain_params) {
	
	erase_chain();
	
	chain_params=p_chain_params;
	
	chain_connection_change=chain_params->chain_changed_internal_signal.connect( SigC::slot( *this, &Effect_Chain::update_chain ) );
	chain_connection_delete=chain_params->chain_deleted_internal_signal.connect( SigC::slot( *this, &Effect_Chain::erase_chain ) );
	
	
	update_chain();
}


Effect_Chain::Effect_Chain() {

	chain_params=NULL;
}


Effect_Chain::~Effect_Chain() {
	
	erase_chain();
}


