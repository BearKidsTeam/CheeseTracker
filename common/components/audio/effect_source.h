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
#ifndef EFFECT_SOURCE_H
#define EFFECT_SOURCE_H


#include "components/audio/effect.h"
/**
@author Juan Linietsky
*/
class Effect_Source {
public:

	virtual int get_effect_count()=0;
	virtual string get_effect_identifier_name(int p_which)=0;
	virtual string get_effect_display_name(int p_which)=0;
	virtual Effect *instantiate_effect(int p_which,Effect::Parameters *p_params)=0;
	virtual Effect::Parameters *instantiate_effect_parameters(int p_which)=0;
	virtual string get_name()=0;
	
	virtual ~Effect_Source();
};

#endif
