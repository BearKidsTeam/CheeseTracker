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

#ifndef _FREQ_CONVERSION_H_
#define _FREQ_CONVERSION_H_

#include "common/components/audio/tables.h"

/**
 * 
 * Juan Linietsky
 **/

Uint32 get_frequency(Uint32 period,bool linear);
Uint32 get_period(Uint16 note,Sint32 speed,bool linear);
#endif
