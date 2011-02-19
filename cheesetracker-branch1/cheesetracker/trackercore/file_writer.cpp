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
                          file_writer.cpp  -  description
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

#include "file_writer.h"
#include <cstring>

File_Writer::File_Writer(){

	writer_in_use=false;
	perform_endian_swap=false;
	error_ocurred=false;
}

File_Writer::~File_Writer(){
}


void File_Writer::check_for_errors() {

//.	if ( !feof(f) ) {
		ERROR("Error writing to file!!!");
		error_ocurred=true;
		error_type=ferror(f);
//	}
}

int File_Writer::open(const char *p_filename) {


	if ( writer_in_use ) return FUNCTION_FAILURE;

	f=fopen(p_filename,"wb");


	if (f==NULL) return FUNCTION_FAILURE;
	else {
                error_ocurred=false;
         	writer_in_use=true;
		return FUNCTION_SUCCESS;

	}
}


void File_Writer::close() {

	if ( !writer_in_use ) return;

	writer_in_use=false;
	fclose(f);
}


bool File_Writer::error_saving() {

	return error_ocurred;

}

void File_Writer::store_byte(Uint8 p_dest) {

	if ( fwrite(&p_dest,1,1,f)==0 ) check_for_errors();
}

void File_Writer::store_byte_array(Uint8 *p_dest,int p_elements) {

	if ( fwrite(p_dest,p_elements,1,f)==0 ) check_for_errors();
}

void File_Writer::store_word_array(Uint16 *p_src,int p_elements) {

	for (int i=0;i<p_elements;i++)
		store_word( *(p_src++) );
}


void File_Writer::store_word(Uint16 p_dest) {

	Uint8 aux_byte1,aux_byte2;

	aux_byte1=p_dest & 0xFF;
	aux_byte2=p_dest >> 8;

//	endian_swap(aux_byte1,aux_byte2);

	store_byte(aux_byte1);
	store_byte(aux_byte2);

}

void File_Writer::store_dword(Uint32 p_dest) {

	Uint16 aux_word1,aux_word2;


	aux_word1=p_dest & 0xFFFF;
	aux_word2=p_dest>>16;

//	endian_swap(aux_word1,aux_word2);

	store_word(aux_word1);
	store_word(aux_word2);


}

void File_Writer::store_sample(sample_int_t p_src) {
	store_word(p_src);
}

void File_Writer::store_pascal_string(const char *p_cstring) {

	Uint8 length;
	int real_length;
	int i;

	real_length=strlen(p_cstring);
	if (real_length>255) length=255; else length=real_length;

	store_byte(length);

	for (i=0;i<length;i++) {
	
		store_byte((Uint8)p_cstring[i]);
	}

}

void File_Writer::set_file_pos(int p_pos) {

	if (fseek(f,p_pos,SEEK_SET)) check_for_errors();

}
int File_Writer::get_file_pos() {

	return ftell(f);
}

