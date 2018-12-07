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

#ifndef _RESAMPLER_MANAGER_H_
#define _RESAMPLER_MANAGER_H_


#include "resampler.h"
#include <vector>
	using std::vector;
/**
 * 
 * Juan Linietsky
 **/
class Resampler_Manager {

	static Resampler_Manager * resampler_manager;

	vector<Resampler*> resampler_list;
	int default_resampler;

public:

	static Resampler_Manager * get_resampler_manager();
	int get_resampler_count();
	Resampler * get_resampler(int p_index);

	void register_resampler(Resampler *p_resampler);
	int get_default_resampler();
	void set_default_resampler(int p_which);

	Resampler_Manager();
	~Resampler_Manager();
};

#endif
