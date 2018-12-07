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

#include "resampler_instance.h"
#include <stdio.h>




Resampler_Instance::Resampler_Instance() {

	current_resampler=DEFAULT_RESAMPLER;
	resampler_manager=Resampler_Manager::get_resampler_manager();
	mixdata.filter.hist_a1=0;
	mixdata.filter.hist_a2=0;
	mixdata.filter.hist_b1=0;
	mixdata.filter.hist_b2=0;
}


Resampler_Instance::~Resampler_Instance()
{
}
