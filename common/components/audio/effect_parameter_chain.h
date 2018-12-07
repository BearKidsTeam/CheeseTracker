//
// C++ Interface: effect_parameter_chain
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EFFECT_PARAMETER_CHAIN_H
#define EFFECT_PARAMETER_CHAIN_H


#include "components/audio/effect.h"
#include <vector>
	using std::vector;

/**
@author Juan Linietsky
*/

class Effect_Chain;

class Effect_Parameter_Chain{

 	vector<Effect::Parameters*> chain;
protected:
friend class Effect_Chain;	
	
	SigC::Signal0<void> chain_changed_internal_signal;
	SigC::Signal0<void> chain_deleted_internal_signal;
public:
	
	SigC::Signal0<void> chain_changed_signal;
	
	void add_effect(Effect::Parameters *p_effect);
	int get_effect_count();
	Effect::Parameters* get_effect(int p_which);
	bool remove_effect(int p_which);

	bool move_effect_up(int p_which);
	bool move_effect_down(int p_which);
	
	void copy_from(Effect_Parameter_Chain *p_lval);
	
	void reset();

	Effect_Parameter_Chain();
	~Effect_Parameter_Chain();

};

#endif
