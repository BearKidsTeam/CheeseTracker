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
#include <cstring>
#include "file_access.h"

File_Manager::File_Manager() {

	manager_in_use=false;
	perform_endian_swap=false;
	error_ocurred=false;
	current_error=OK;
}

File_Manager::~File_Manager() {
}

bool File_Manager::exists(string p_fname) {

	FILE *g;
	//printf("opening file %s\n", p_fname.c_str());
	g=fopen(p_fname.c_str(),"rb");
	if (g==NULL) {

		return false;
	} else {

		fclose(g);
		return true;
	}

}

void File_Manager::check_for_errors() {

	if ( !feof(f) ) {

		error_ocurred=true;
		error_type=ferror(f);
		perror("uh?");
		current_error=ERROR_READING_FILE;//must specify better
	}
}


File_Manager::Error File_Manager::open(const char *p_filename, Mode p_mode) {

	static const char* mode_string[MAX_MODES]={"rb","wb","rb+"};

	if ( manager_in_use ) return IN_USE;

	//printf("open(): opening %s\n", p_filename);
	f=fopen(p_filename,mode_string[p_mode]);


	if (f==NULL)
		return ERROR_OPENING_FILE;
	else {
		error_ocurred=false;
		manager_in_use=true;
		return OK;
	}
}

void File_Manager::close() {

	current_error=OK;
	if ( !manager_in_use ) return;
        manager_in_use=false;
	fclose(f);
}


void File_Manager::seek(Uint32 p_position) {

	if ( fseek(f,p_position,SEEK_SET) ) check_for_errors();
}
void File_Manager::seek_end() {

	if ( fseek(f,0,SEEK_END) ) check_for_errors();
}

Uint32 File_Manager::get_pos() {

	Uint32 aux_position;
	if ( !(aux_position = ftell(f)) ) {
		check_for_errors();
	};
	return aux_position;
};



bool File_Manager::eof_reached() {

	return (feof(f)!=0);

}

File_Manager::Error File_Manager::get_error() {

	return current_error;
}

void File_Manager::get_byte(Uint8 &p_dest) {

	if ( fread(&p_dest,1,1,f)==0 ) check_for_errors();
}

void File_Manager::get_byte_array(Uint8 *p_dest,int p_elements) {

	if ( fread(p_dest,p_elements,1,f)==0 ) check_for_errors();
}

void File_Manager::get_word(Uint16 &p_dest) {

	Uint8 aux_byte1,aux_byte2;

	get_byte(aux_byte1);
	get_byte(aux_byte2);

	endian_swap(aux_byte1,aux_byte2);

	p_dest=aux_byte2;
	p_dest<<=8;
	p_dest|=aux_byte1;

}

void File_Manager::get_dword(Uint32 &p_dest) {

	Uint16 aux_word1,aux_word2;

	get_word(aux_word1);
	get_word(aux_word2);

	endian_swap(aux_word1,aux_word2);

	p_dest=aux_word2;
	p_dest<<=16;
	p_dest|=aux_word1;
}

string File_Manager::get_pascal_string() {

	string aux_string;
	Uint8 length;
	char aux_char;
	int i;

	aux_string="";

	get_byte(length);

	for (i=0;i<length;i++) {

		get_byte((Uint8&)aux_char);
		aux_string=aux_string+aux_char;
	}

	return aux_string;
}

void File_Manager::store_byte(Uint8 p_dest) {

	if ( fwrite(&p_dest,1,1,f)==0 ) check_for_errors();
}

void File_Manager::store_byte_array(const Uint8 *p_dest,int p_elements) {

	if ( fwrite(p_dest,p_elements,1,f)==0 ) check_for_errors();
}

void File_Manager::store_word(Uint16 p_dest) {

	Uint8 aux_byte1,aux_byte2;

	aux_byte1=p_dest & 0xFF;
	aux_byte2=p_dest >> 8;

//	endian_swap(aux_byte1,aux_byte2);

	store_byte(aux_byte1);
	store_byte(aux_byte2);

}

void File_Manager::store_dword(Uint32 p_dest) {

	Uint16 aux_word1,aux_word2;


	aux_word1=p_dest & 0xFFFF;
	aux_word2=p_dest>>16;

//	endian_swap(aux_word1,aux_word2);

	store_word(aux_word1);
	store_word(aux_word2);


}

void File_Manager::store_pascal_string(const char *p_cstring) {

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

void File_Manager::truncate_file(string p_filename, Uint32 p_length) {

	/// @todo: no truncate?

	if (truncate(p_filename.c_str(), (off_t)p_length))
		check_for_errors();
};



