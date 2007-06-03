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


#ifndef RESAMPLER_CUBIC_H
#define RESAMPLER_CUBIC_H


#include "components/audio/resampler.h"





class Resampler_Cubic : public Resampler {


public:

	virtual void mix(Resampler::Mix_Data *p_data);
	virtual string get_name();
	virtual bool has_interpolation();
	Resampler_Cubic();

};

#endif /* resampler raw */
