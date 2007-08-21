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
                          tables.h  -  description
                             -------------------
    begin                : Sat Apr 14 2001
    copyright            : (C) 2001 by Juan Linietsky
    email                : reduz@anime.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TABLES_H
#define TABLES_H

#include "typedefs.h"

/**conversion tables/functions
  *@author Juan Linietsky
  */

/******************************
 tables.h
 --------

Tables methods for miscelaneous
conversion utilities
********************************/

class Tables {
public:
	
        static Uint16 old_period_table[12*2];
        static Uint32 linear_period_to_freq_tab[768];

        static Uint32 get_old_period(Uint16 note,Sint32 speed);
        static Uint32 get_amiga_period(Uint16 note,Sint32 fine);
        static Uint32 get_linear_period(Uint16 note,Sint32 fine);
        static Uint32 get_linear_frequency(Uint32 period);
	static Uint32 get_old_frequency(Uint32 period);

	Tables();
	~Tables();
};

#endif
