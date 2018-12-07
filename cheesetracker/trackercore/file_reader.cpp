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
                          file_reader.cpp  -  description
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

#ifdef HAVE_MMAP
#  ifndef _POSIX_MAPPED_FILES
#    define _POSIX_MAPPED_FILES
#  endif
#  include <unistd.h>
#  include <sys/mman.h>

// For open(2):

#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#endif

#include <cstring>
#include <cerrno>
#include "Error.h"
#include "file_reader.h"

#define THROW_EXCEPTION(operation) {					\
		Error E;						\
		E.set_error_pfx((filename+(string)": "+(string)operation)); \
		E.report_errno(errno);					\
		throw E;						\
}

File_Reader::File_Reader(){

	reader_in_use=false;
	perform_endian_swap=false;
	file_pos=0;
	file_size=0;
	file_contents=NULL;
#ifdef HAVE_MMAP
	fd=-1;
#else
	f=NULL;
#endif
}

File_Reader::~File_Reader(){
	// All clean-up is handled in close()
	close();
}

#ifdef HAVE_MMAP

// Since mmap cannot use stdio FILE pointers, get_size_internal
// must use stat() if mmap is being used.

size_t File_Reader::get_size_internal() {
	struct stat statbuf;
	int status;

	status=fstat(fd, &statbuf);
	if(status==-1)
		THROW_EXCEPTION("fstat")

	return statbuf.st_size;
}
#else

// If mmap is not being used, then pure ANSI C can
// be used to determine the file size.

size_t File_Reader::get_size_internal() {
	int retval;
	int current_pos;
	int status;

	current_pos = get_file_pos_internal();
	if(current_pos == -1) 
		THROW_EXCEPTION("ftell")
	status=fseek(f, 0, SEEK_END);
	if(status == -1) 
		THROW_EXCEPTION("fseek")
	retval = get_file_pos_internal();
	if(retval == -1)
		THROW_EXCEPTION("ftell")
	if(fseek(f, current_pos, SEEK_SET) == -1)
		THROW_EXCEPTION("fseek")

	return retval;
}

#endif
int File_Reader::get_size()	{
	return file_size;
}

void File_Reader::check_for_errors() {
#ifndef HAVE_MMAP
	if ( !feof(f) ) {

		error_ocurred=true;
		error_type=ferror(f);
	}
#endif
}

#ifdef HAVE_MMAP
int File_Reader::open(const char *p_filename) {

	if ( reader_in_use ) return FUNCTION_FAILURE;

	try {
		fd = ::open(p_filename, O_RDONLY);
		filename=p_filename;

		if (fd==-1) 
			THROW_EXCEPTION("open")
		else {
			reader_in_use=true;
		}

		// Map the file to memory and mark it for sequential
		// access (the most commonly-used access method in
		// the program).
		//
		// Mapping to memory is more efficient than reading
		// one byte at a time, because hard drives are
		// better at getting blocks of data. Mapping is
		// better than loading the entire file upon opening
		// (which this program does if mmap() is unavailable)
		// because some algorithms in this program read
		// the head of a file and then close it without reading
		// the rest.
		//
		//

		file_size = get_size_internal();

		file_contents = (byte*) mmap(NULL, file_size, PROT_READ,
		                             MAP_SHARED, fd, 0);

		if(file_contents == MAP_FAILED)
			THROW_EXCEPTION("mmap")
#  ifdef HAVE_MADVISE
		madvise(file_contents, file_size, MADV_SEQUENTIAL);
#  endif

		
	} catch(Error E) {
		error_ocurred=true;
		error_type=E._errno;

		printf("%s\n", E.what());

		close();

		return FUNCTION_FAILURE;
	} catch(exception E) {
		
		close();
		return FUNCTION_FAILURE;
	}
	return FUNCTION_SUCCESS;
}

#else

int File_Reader::open(const char *p_filename) {


	if ( reader_in_use ) return FUNCTION_FAILURE;

	try {
		f=fopen(p_filename,"rb");
		filename=p_filename;

		if (f==NULL) 
			THROW_EXCEPTION("fopen")
		else {
			reader_in_use=true;
		}

		// Read the entire file immediately. The byte-at-a-time
		// interface from older versions of CheeseTracker
		// remains, but the bytes will now come from the
		// internal buffer.
		//
		// Reading the entire file is more efficient than
		// reading one byte at a time (hard drives like to read
		// in blocks). Therefore, this should cause large samples
		// to load more quickly.

		file_size = get_size_internal();

		file_contents = new byte[file_size];
		if((fread(file_contents, 1, file_size, f) < file_size) &&
		   ferror(f))
		{
			THROW_EXCEPTION("fread")
		}
		
	} catch(Error E) {
		error_ocurred=true;
		error_type=E._errno;

		close();

		return FUNCTION_FAILURE;
	} catch(exception E) {
		
		close();
		return FUNCTION_FAILURE;
	}
	return FUNCTION_SUCCESS;
}
#endif


void File_Reader::close() {

	if ( !reader_in_use ) return;

        reader_in_use=false;

#ifdef HAVE_MMAP
	if(fd != -1)
		::close(fd);
	fd=-1;
	if(file_contents != NULL)
		munmap(file_contents, file_size);
#else 
	if(f != NULL) {
		fclose(f);
		f=NULL;
	}
	delete file_contents;
#endif
	file_contents=NULL;
	file_pos=0;
	file_size=0;
	filename="";
}

void File_Reader::seek(Uint32 p_position) {

	file_pos=p_position;
	if(file_pos > file_size)
		file_pos=file_size;
}

bool File_Reader::eof_reached() {

	return file_pos >= file_size;

}

bool File_Reader::error_loading() {

	return error_ocurred;

}


void File_Reader::get_byte_array(Uint8 *p_dest,int p_elements) {

	if(file_pos+p_elements > file_size) {
		// Short read at end of file.
		// FIXME: The remainder of {p_dest} will now
		// contain unanticipated data, but there is no
		// way to inform the caller.

		p_elements -= (file_pos+p_elements)-file_size;
		if(!(p_elements > 0))
			return;
	}

	memcpy((void*)p_dest, (void*)(file_contents+file_pos), p_elements);

	file_pos += p_elements;

}

// A Windowsism: "word" always means 16 bits, and "dword" always means
// 32 bits, regardless of the underlying machine.

void File_Reader::get_word_array(Uint16 *p_dest,int p_elements) {

	// Unfortunately, we can't just memcpy a word array,
	// because byte-order must be accounted for.

	for (int i=0;i<p_elements && !eof_reached();i++)
		*p_dest++ = get_word();
}



