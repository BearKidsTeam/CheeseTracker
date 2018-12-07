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
                          pattern.h  -  description
                             -------------------
    begin                : Thu Jan 18 2001
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

#ifndef PATTERN_H
#define PATTERN_H

#include <map>
#include <vector>
	using std::vector;

	using std::map;

#include "Error.h"
#include "note.h"
#include "common/os/mutex_lock.h"

#define PATTERN_WIDTH 64
#define MIN_PATTERN_LENGTH 32
#define MAX_PATTERN_LENGTH 200

/**
  *@author Juan Linietsky
  */
/******************************
 pattern.h
 ------------------------

And you thought this was an array...
or a linked list.. HA HA HA.
********************************/


class Pattern {

	//We store a column as a map
	typedef map < int , Note > Column;

	//And all the colummns as a vector
	vector<Column> column;

	//Length in rows
	Uint8 length;

	//an empty note looks like this
	static Note empty_note;

	// data abstraction helpers
	Note aux_storage_note;
	int storage_note_row;
	int storage_note_column;
	bool last_request_valid;
	void process_insert_request(int p_column,int p_row,Note p_note);
        Note process_retrieve_request(int p_column,int p_row);

	//misc vars for optimization...
	//The pattern de-initializes itself
	//when it contains no data.

	bool empty;
	bool check_for_empty_pattern();
	int note_count;


public:

	static Mutex_Lock *data_lock;

	//The only way to modify an existing note!
	Note& get_note_ref(int p_column, int p_row);

	//The safe way to retrieve a note!
	Note get_note(int p_column, int p_row);

	void set_length(Uint8 p_length);
	Uint8 get_length();
	int get_note_count();

        bool in_use() { return !empty; };

	void clear();

	Pattern();
	~Pattern();
};

#endif
