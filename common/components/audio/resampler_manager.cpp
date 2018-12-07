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

#include "resampler_manager.h"


Resampler_Manager * Resampler_Manager::resampler_manager=NULL;

Resampler_Manager * Resampler_Manager::get_resampler_manager() {

 	return resampler_manager;
}


int Resampler_Manager::get_resampler_count() {

 	return resampler_list.size();
}
Resampler * Resampler_Manager::get_resampler(signed int p_index) {

	if (p_index == -1) {
		p_index = get_default_resampler();
	}
	if ((p_index<0) || (p_index>=resampler_list.size()) ) {
		throw Out_Of_Bounds(__FILE__, __LINE__);
	}

	return resampler_list[p_index];

}

void Resampler_Manager::register_resampler(Resampler *p_resampler) {

	default_resampler=resampler_list.size();
 	resampler_list.push_back(p_resampler);

}

int Resampler_Manager::get_default_resampler() {

	return default_resampler;
}
void Resampler_Manager::set_default_resampler(int p_which) {


	if ((p_which<0) || (p_which>=resampler_list.size()))
		return;
	default_resampler=p_which;
}


Resampler_Manager::Resampler_Manager() {

	if (resampler_manager!=NULL) {

		//printf("Error, Resampler_Manager is a SINGLETON!");
		return;
	}
	resampler_manager=this;
}


Resampler_Manager::~Resampler_Manager() {
}
