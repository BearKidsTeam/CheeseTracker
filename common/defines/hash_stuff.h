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
/*
  $Id: hash_stuff.h,v 1.4 2003/03/11 21:45:01 punto Exp $
 */


#include "string_hash.h"

#ifndef _HASH_STUFF_H
#define _HASH_STUFF_H

#if (__GNUC__ == 3) || (__GNUC__ > 3)


	#ifdef USING_STLPORT
		#include <hash_map>
			using std::hash_map;
			using std::hash;
			
	#else
		#include <ext/hash_map>
			using __gnu_cxx::hash_map;
			using __gnu_cxx::hash;
	#endif
	
    
#else


#include <hash_map>
    using std::hash_map;

#endif


#endif
