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
                          loader.cpp  -  description
                             -------------------
    begin                : Sun Jan 21 2001
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

#ifdef HAVE_GNU_BASENAME
#undef HAVE_XPG_BASENAME
#endif

#include "loader.h"

#ifdef HAVE_XPG_BASENAME
#  include <libgen.h>
#endif

#include <cstring>


#ifdef HAVE_XPG_BASENAME

// Overload POSIX 'basename' with a version that can take a 'const char'
// filename.

string basename(const char *filename)
{
	char *buffer;
	char *basename_ret;

	if(filename==NULL)
		return "";

	buffer = new char[strlen(filename)+1];
	strcpy(buffer, filename);

	// Since {buffer} is not (const char*) but simply (char*),
	// C++ knows we're referring to the POSIX 'basename'-- the statement
	// below is not a recursive call to this function. POSIX 'basename'
	// may modify its argument.
	
	basename_ret = basename(buffer);

	delete[] buffer;

	return basename_ret;
}

#elif !defined(HAVE_GNU_BASENAME)

// GNU or XPG 'basename' not available. Use a local implementation instead

string basename(const char *filename)
{
#if defined(_WIN32) && !defined(__CYGWIN__)
	const char *retval = rindex(filename, '\\');
#else
	const char *retval = rindex(filename, '/');
#endif
	return (retval ? retval+1 : filename);
}

#endif



const char * Loader::error_string[Loader::HEADER_CORRUPT+1]={"Success","File Loader in use","Cannot open file!","File Error","File Format Not Recognized","Header Corrupt"};

Loader::Loader(){
}

Loader::~Loader(){

	free_info();
}

Loader::Error Loader::abort_load(Error p_cause) {

	file_read.close();

	return p_cause;
}


