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
                          note.h  -  description
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

#ifndef NOTE_H
#define NOTE_H

#include "typedefs.h"

#define EMPTY_FIELD 255

/******************************
 note.h
 ------------------------

note event!! not as small as peoplethinks!
********************************/

struct Note {

	enum {

		NOTES=120,
		OFF=254,
		CUT=253
	};


	Uint8 note;
	Uint8 instrument;
	Uint8 volume;
	Uint8 command;
	Uint8 parameter;

	void clear() {

		note=EMPTY_FIELD;
		instrument=EMPTY_FIELD;
		volume=EMPTY_FIELD;
		command=EMPTY_FIELD;
		parameter=0;
	}
	
	void raise() {

		if (note<NOTES-1) note++;

	}

	void lower() {

		if ((note>0) && (note<NOTES)) note--;

	}

	bool operator== (Note rvalue) {

		return (
			 (note==rvalue.note) &&
			 (instrument==rvalue.instrument) &&
			 (volume==rvalue.volume) &&
			 (command==rvalue.command) &&
			 (parameter==rvalue.parameter)
			);
	}

	Note() {

		clear();
	}
};


#endif /* note.h */
