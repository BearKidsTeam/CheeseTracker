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
//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "path_data.h"
#include <stdio.h>
#include <stdlib.h>

string Path_Data::get_home_dir() {

	if (getenv("HOME"))
		return getenv("HOME");
	else
		return ".";
}
bool Path_Data::file_exists(string p_file) {

	FILE *f = fopen(p_file.c_str(),"rb");
	if (f) {
		fclose(f);
		return true;
	} else {

		return false;
	}

}

Path_Data::Path_Data()
{
}


Path_Data::~Path_Data()
{
}


