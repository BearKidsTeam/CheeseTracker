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
                          file_writer.h  -  description
                             -------------------
    begin                : Sat Feb 10 2001
    copyright            : (C) 2001 by Juan Sebastian Linietsky
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

#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <stdio.h>
#include "typedefs.h"
#include "sample_defs.h"

/**writes... files?
  *@author Juan Sebastian Linietsky
  */
/******************************
 file_writer.h
 ----------

Some file write operations, abstracted
in a class for ease of use.
********************************/

class File_Writer {

    	FILE *f;

	bool writer_in_use;
	bool error_ocurred;
	int error_type;

	bool perform_endian_swap;

	void check_for_errors();

public:

        int open(const char *p_filename);
	void close();
//	void seek(Uint32 p_position);	

//	bool eof_reached();
	bool error_saving();

	void store_byte(Uint8 p_dest);
	void store_byte_array(Uint8 *p_dest,int p_elements);
	void store_word_array(Uint16 *p_src,int p_elements);

	void store_word(Uint16 p_dest);
	void store_dword(Uint32 p_dest);
	void store_sample(sample_int_t p_dest);

	void store_pascal_string(const char *p_cstring);

	void set_file_pos(int p_pos);
	int get_file_pos();

	// use this for files WRITTEN in _big_ endian machines (ie, amiga/mac)
	// It's not about the current CPU type but file formats.
	void set_endian_conversion(bool p_swap) { perform_endian_swap=p_swap; }
        bool is_open() { return writer_in_use; };

	File_Writer();
	~File_Writer();
};

#endif
