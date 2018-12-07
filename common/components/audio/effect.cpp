/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://reduz.com.ar/cheesetronic
    copyright            : (C) 2003 by Juan Linietsky
    email                : coding@reduz.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
                          effect.cpp  -  description
                             -------------------
    begin                : Sat Aug 17 2002
    copyright            : (C) 2002 by red
    email                : red@server
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "effect.h"
#include "sound_driver_manager.h"




void Effect::Parameters::copy_from(Parameters* rvalue) {

	list<Property_Bridge*> rlist = rvalue->get_property_list();
	list<Property_Bridge*> mylist = get_property_list();

	list<Property_Bridge*>::iterator rit = rlist.begin();
	list<Property_Bridge*>::iterator myit = mylist.begin();

	while (rit != rlist.end() && myit != mylist.end()) {

		(*myit)->copy_value(*(*rit));

		rit++;
		myit++;
	};
};



Effect::Parameters *Effect::get_parameters() {

	return data;

}

void Effect::sampling_rate_changed_notify() {


}

void Effect::sampling_rate_changed(int p_rate) {

	sampling_rate=p_rate;
	sampling_rate_changed_notify();
}


float Effect::get_sampling_rate() {

	return sampling_rate;
}

Effect::Effect(Parameters *p_data) {

	data=p_data;
	Sound_Driver_Manager::get_singleton_instance()->sampling_rate_changed_signal.connect(
		SigC::slot(*this,&Effect::sampling_rate_changed)
	);

	sampling_rate=Sound_Driver_Manager::get_singleton_instance()->get_mix_frequency();
	printf("FX Sampling rate is %f\n",sampling_rate);
}

Effect::~Effect() {
	
}

Effect::Parameters::~Parameters() {}
