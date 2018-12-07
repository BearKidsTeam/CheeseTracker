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

#ifndef STRING_HASH_H
#define STRING_HASH_H

#include <string>
	using std::string;
#include "hash_stuff.h"


struct String_Hash {

	hash< char* > hasher;
	size_t operator() ( const string& S ) const { return hasher( S.c_str() ); };
};

#endif


