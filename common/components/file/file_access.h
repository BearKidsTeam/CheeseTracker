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
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H


#include <stdio.h>
#include <unistd.h>		// ftruncate
#include "typedefs.h"
#include <string>
using std::string;

class File_Manager {
public:

        enum Error {

		OK,
		IN_USE,
		ERROR_OPENING_FILE,
		ERROR_READING_FILE,
		ERROR_WRITING_FILE
	};

	enum Mode  {

		READ,
		WRITE,
		READ_WRITE,
		MAX_MODES
	};

private:
	FILE *f;

	bool manager_in_use;
	bool error_ocurred;
	int error_type;

	bool perform_endian_swap;

	void check_for_errors();

	template<class T>
	void endian_swap(T a, T b) {

		T c;
		if  (perform_endian_swap) c=a; a=b; b=c;
	}

	Error current_error;
public:

	static bool exists(string p_fname);
	Error open(const char *p_filename, Mode p_mode);
	void close();
	void seek(Uint32 p_position);
	void seek_end();
	Uint32 get_pos();

	bool eof_reached();

	void get_byte(Uint8 &p_dest);
	void get_byte_array(Uint8 *p_dest,int p_elements);

	void get_word(Uint16 &p_dest);
	void get_dword(Uint32 &p_dest);

	string get_pascal_string();

	// use this for files WRITTEN in _big_ endian machines (ie, amiga/mac)
	// It's not about the current CPU type but file formats.
	void set_endian_conversion(bool p_swap) { perform_endian_swap=p_swap; }
	bool is_open() { return manager_in_use; };

	Error get_error();

	void store_byte(Uint8 p_dest);
	void store_byte_array(const Uint8 *p_dest,int p_elements);

	void store_word(Uint16 p_dest);
	void store_dword(Uint32 p_dest);

	void store_pascal_string(const char *p_cstring);

	void truncate_file(string p_filename, Uint32 p_length);

	File_Manager();
 	~File_Manager();

};

#endif

