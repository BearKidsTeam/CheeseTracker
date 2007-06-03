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
                          pattern.cpp  -  description
                             -------------------
    begin                : Thu Jan 18 2001
    copyright            : (C) 2001 by c0del4rvi4
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

#include <cassert>
#include <cstdlib>
#include "pattern.h"


Note Pattern::empty_note;
Mutex_Lock *Pattern::data_lock=NULL;

Pattern::Pattern(){

	note_count=0;
	last_request_valid=false;
	length=64;
        empty=true;
	data_lock=NULL;
	clear();
}

Pattern::~Pattern(){


}
bool Pattern::check_for_empty_pattern() {

	int i;

	if (empty) return true;

	for (i=0;i<PATTERN_WIDTH;i++) {

		if (!column[i].empty()) {

			return false;
		}

	}
	return true;
}

void Pattern::process_insert_request(int p_column,int p_row,Note p_note) {

//	Column::const_iterator I;

	if ( p_note==empty_note ) {
//		I=column[p_column].find(p_row);
                if (!empty) {

			note_count -= column[p_column].erase(p_row);
			if (check_for_empty_pattern()) {

				column.clear();
				empty=true;
			}
		}

	} else {

		if (empty) {

			column.resize(PATTERN_WIDTH);
			empty=false;
		}

		if ( column[p_column].find(p_row)==column[p_column].end() ) note_count++;
		column[p_column][p_row]=p_note;
	}


}

Note Pattern::process_retrieve_request(int p_column,int p_row) {

	// This would be an assert, but there is code that
	// triggers this exception on purpose.
	if(!(p_column < column.size())) {
		Out_Of_Bounds OOB;
		throw OOB;
	}

	Note tmp_result;
	Column::const_iterator I;

	tmp_result.clear();


	if (empty) return tmp_result;

	// FIXME: This might be stupid: column[p_column] is
	//	  of type map<int, Note> (typedefed to Column),
	//	  meaning it's an associative array of Notes whose
	//	  key is an int. This saves memory, at the cost
	//	  of CPU time, and this is a performance-critical
	//	  function. Changing this to an array type would
	//	  require making sure the array is long enough to
	//	  hold all the rows requested, ahead of time.
	I=column[p_column].find(p_row);

	if ( I!=column[p_column].end() ) {

		tmp_result=column[p_column][p_row];
	} else {

		tmp_result=empty_note;
	}

	return tmp_result;
}
Note Pattern::get_note(int p_column, int p_row) {

	Note tmp_result;

	assert(data_lock == NULL);  // FIXME -- This is really stupid. 

	if (data_lock!=NULL) data_lock->grab(); 

	if ((p_column==storage_note_column) && (p_row==storage_note_row) && last_request_valid) {

		tmp_result=aux_storage_note;
	} else {

		try {
			tmp_result=process_retrieve_request(p_column,p_row);
		} catch (Out_Of_Bounds OOB) {
			tmp_result = empty_note;
		}
	}

	if (data_lock!=NULL) data_lock->release();

	return tmp_result;
}

Note& Pattern::get_note_ref(int p_column, int p_row) {

	if (data_lock!=NULL) data_lock->grab();

	Note empty_note;

	if ( last_request_valid ) {

		process_insert_request(storage_note_column,storage_note_row,aux_storage_note);
	}


	if ( (p_column>=PATTERN_WIDTH) || (p_row>=length) || (p_column<0) || (p_row<0) ) {

		ERROR("It wants a note from " << p_column << "," << p_row);
		aux_storage_note=empty_note;
		last_request_valid=false;

	} else {

		last_request_valid=true;
		storage_note_column=p_column;
		storage_note_row=p_row;

		try {
			aux_storage_note=process_retrieve_request(p_column,p_row);
		} catch (Out_Of_Bounds OOB) {
			aux_storage_note=empty_note;
		}
	}

	if (data_lock!=NULL) data_lock->release();

	return aux_storage_note;
}

void Pattern::set_length(Uint8 p_length) {

	int i;
	Column::iterator I;		


	if ( (p_length<1) || (p_length>MAX_PATTERN_LENGTH) ) return;

	if (empty) {

		length=p_length;
		return;
	}


	if (data_lock!=NULL) data_lock->grab();
		
       	if (length<p_length) {

       		for (i=0;i<PATTERN_WIDTH;i++) {

       			for (I=column[i].begin();I!=column[i].end();I++) {


       				if (I->first>=length) column[i].erase(I->first);
       			}
       		}
       	} else if (length>p_length) {

       		for (i=0;i<PATTERN_WIDTH;i++) {

       			for (I=column[i].begin();I!=column[i].end();I++) {


       				if (I->first>=p_length) column[i].erase(I->first);
       			}
       		}
       	}
       	length=p_length;
	

	if (data_lock!=NULL) data_lock->release();

}
void Pattern::clear() {


	if (data_lock!=NULL) data_lock->grab();
	
	empty=true;
	column.clear();
        last_request_valid=false;
	storage_note_column=-1;
	storage_note_row=-1;

	if (data_lock!=NULL) data_lock->release();

}

Uint8 Pattern::get_length() {

	return length;
}

int Pattern::get_note_count() {

	return note_count;
}

