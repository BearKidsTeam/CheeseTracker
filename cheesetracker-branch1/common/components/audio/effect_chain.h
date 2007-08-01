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
#ifndef EFFECT_CHAIN_H
#define EFFECT_CHAIN_H


#include "components/audio/effect.h"
#include "components/audio/effect_parameter_chain.h"
#include <vector>
	using std::vector;
/**
@author Juan Linietsky
*/
class Effect_Chain : public SigC::Object {

	vector<Effect*> chain;
	Effect_Parameter_Chain *chain_params;
	
	SigC::Connection chain_connection_change;
	SigC::Connection chain_connection_delete;

	void erase_chain();
	void update_chain();	
public:

	int get_effect_count();
	Effect* get_effect(int p_which);

	void process_buffer(sample_t *p_buffer,int p_length);
	
	void set_chain_params(Effect_Parameter_Chain *chain_params);
	
	

	Effect_Chain();
	~Effect_Chain();

};

#endif
