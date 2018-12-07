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
#ifndef _RESAMPLER_NEW_H_
#define _RESAMPLER_NEW_H_

#include "sample_data.h"
#include "iir_svf.h"

namespace better {

/**
 *
 * Juan Linietsky
 **/
class Resampler {
public:

	virtual void mix(Mix_Data *p_data)=0;
	virtual string get_name()=0;
	virtual bool has_interpolation()=0; //it's good to distingish them!
	

	Resampler();
	virtual ~Resampler();
};

}

#endif
