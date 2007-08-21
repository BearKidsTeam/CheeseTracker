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
                          file_reader.h  -  portable file reading class
                             -------------------
    begin                : Tue Jan 23 2001
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

#ifndef FILE_READER_H
#define FILE_READER_H

#include "stdio.h"
#include "typedefs.h"
/**
  *@author Juan Linietsky
  */

/******************************
 file_reader.h
 ----------

Some file read operations, abstracted
in a class for ease of use.
********************************/


class File_Reader {

#ifdef HAVE_MMAP

	int fd;
#else
	FILE *f;
#endif

	bool reader_in_use;
	bool error_ocurred;
	int error_type;

	bool perform_endian_swap;

	void check_for_errors();
	size_t file_pos;
	size_t file_size;

	byte *file_contents;
	string filename;

	template<class T>
	void endian_swap(T a, T b) {

		T c;
		if  (perform_endian_swap) { c=a; a=b; b=c; }
	}

#ifndef HAVE_MMAP
	int get_file_pos_internal() { return ftell(f); };
#endif
	size_t get_size_internal();

	public:

	int open(const char *p_filename);
	void close();
	void seek(Uint32 p_position);	
	int get_file_pos() { return file_pos; };
	int get_size();
	bool eof_reached();
	bool error_loading();

	inline const char *get_filename() {
		return filename.c_str();
	}

	// FIXME: Undefined return value if eof_reached() is true.
	// This comes from the total lack of error handling in this
	// program.

	inline Uint8 get_byte() {

		Uint8 p_dest;

		if(!eof_reached()) {
			p_dest = file_contents[file_pos];
			file_pos++;
		}
		if(eof_reached())
			file_pos=file_size;

		return p_dest;
	}

	inline Uint16 get_word() {

		Uint16 p_dest;

		Uint8 aux_byte1,aux_byte2;

		aux_byte1=get_byte();
		aux_byte2=get_byte();

		endian_swap(aux_byte1,aux_byte2);

		p_dest=aux_byte2;
		p_dest<<=8;
		p_dest|=aux_byte1;

		return p_dest;

	}

	inline Uint32 get_dword() {

		Uint16 aux_word1,aux_word2;
		Uint32 p_dest;

		aux_word1=get_word();
		aux_word2=get_word();

		endian_swap(aux_word1,aux_word2);

		p_dest=aux_word2;
		p_dest<<=16;
		p_dest|=aux_word1;

		return p_dest;
	}

	void get_byte_array(Uint8 *p_dest,int p_elements);
	void get_word_array(Uint16 *p_dest,int p_elements);

	// use this for files WRITTEN in _big_ endian machines (ie, amiga)
	// It's not about the current CPU type but file formats.
	void set_endian_conversion(bool Big_Endian) { perform_endian_swap=Big_Endian; }
        bool is_open() { return reader_in_use; };

	File_Reader();
	~File_Reader();
};

#endif
