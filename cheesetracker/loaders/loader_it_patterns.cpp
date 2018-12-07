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
                          loader_it_patterns.cpp  -  description
                             -------------------
    begin                : Wed Apr 11 2001
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

#include "loader_it.h"






Loader::Error Loader_IT::load_patterns() {

	Loader::Error tmp_result = SUCCESS;

        Uint32 *pattern_offset;	
	int i;

	pattern_offset=(Uint32 *)malloc(header.patnum*4);
	pattern=(Pattern **)malloc(sizeof(Pattern*)*header.patnum);

	file_read.seek(0xC0+header.ordnum+header.insnum*4+header.smpnum*4);

	for (i=0;i<header.patnum;i++) {

         	pattern_offset[i]=file_read.get_dword();
		pattern[i]=new Pattern;
	}



	for (i=0;i<header.patnum;i++) {

		Uint16 pat_size;
		Uint16 pat_length;

		if (pattern_offset[i]==0) {

			pattern[i]->set_length(64);

		} else {

			int row=0,flag,channel,j;
			Uint8 aux_byte;
			Uint32 reserved;
			Uint8 chan_mask[64];
			Note last_value[64];

			for (j=0;j<64;j++) {

				chan_mask[j]=0;
				last_value[j].clear();
			}

			file_read.seek(pattern_offset[i]);

			pat_size=file_read.get_word();
			pat_length=file_read.get_word();
			reserved=file_read.get_dword();
			//[[TODO check for corrupt data

			pattern[i]->set_length(pat_length);

			do {

				aux_byte=file_read.get_byte();
				flag=aux_byte;

				if ( flag==0 ) {

					row++;
				} else {

					channel=(flag-1) & 63;

					if ( flag & 128 ) {

						aux_byte=file_read.get_byte();
						chan_mask[channel]=aux_byte;
					}


					if ( chan_mask[channel]&1 ) { // read note
				
						aux_byte=file_read.get_byte();
						
						if ( aux_byte<120 ) pattern[i]->get_note_ref(channel,row).note=aux_byte;
						else if ( aux_byte==255 ) pattern[i]->get_note_ref(channel,row).note=Note::OFF;
						else if ( aux_byte==254 ) pattern[i]->get_note_ref(channel,row).note=Note::CUT;
						else pattern[i]->get_note_ref(channel,row).note=EMPTY_FIELD;

						last_value[channel].note=pattern[i]->get_note(channel,row).note;
					}
						
	
					if ( chan_mask[channel]&2 ) {
	
						aux_byte=file_read.get_byte();
						if ( aux_byte<100 ) pattern[i]->get_note_ref(channel,row).instrument=aux_byte-1;
						else pattern[i]->get_note_ref(channel,row).instrument=EMPTY_FIELD;

						last_value[channel].instrument=pattern[i]->get_note(channel,row).instrument;
					}
					if ( chan_mask[channel]&4 ) {

						aux_byte=file_read.get_byte();
						if ( aux_byte<213 ) pattern[i]->get_note_ref(channel,row).volume=aux_byte;
						else pattern[i]->get_note_ref(channel,row).volume=EMPTY_FIELD;

						last_value[channel].volume=pattern[i]->get_note(channel,row).volume;
					}
					if ( chan_mask[channel]&8 ) {

						aux_byte=file_read.get_byte();
						if ( aux_byte<213 ) pattern[i]->get_note_ref(channel,row).command=aux_byte-1;
						else pattern[i]->get_note_ref(channel,row).command=EMPTY_FIELD;
						last_value[channel].command=pattern[i]->get_note(channel,row).command;

						aux_byte=file_read.get_byte();
						pattern[i]->get_note_ref(channel,row).parameter=aux_byte;
						last_value[channel].parameter=pattern[i]->get_note(channel,row).parameter;

						if ((last_value[channel].command==('Z'-'A')) && (last_value[channel].parameter<0x7F)) {
							last_value[channel].parameter/=2; //fit back to IT ranges, since we have doubled them
						}
					}

					if ( chan_mask[channel]&16 ) {

						pattern[i]->get_note_ref(channel,row).note=last_value[channel].note;
					}

					if ( chan_mask[channel]&32 ) {

						pattern[i]->get_note_ref(channel,row).instrument=last_value[channel].instrument;
					}
					if ( chan_mask[channel]&64 ) {

						pattern[i]->get_note_ref(channel,row).volume=last_value[channel].volume;
					}
					if ( chan_mask[channel]&128 ) {

						pattern[i]->get_note_ref(channel,row).command=last_value[channel].command;
						pattern[i]->get_note_ref(channel,row).parameter=last_value[channel].parameter;
					}
				}
			} while(row<pat_length);

		}

		if (file_read.eof_reached() || file_read.error_loading()) {

			ERROR("Warning: Pattern " << i << "is corrupted");
		}

		pattern_count++;
	}

	free(pattern_offset);

//	if (file_read.eof_reached() || file_read.error_loading()) {

		//[[todo]] Clean up the mess (samples loaded)

//		tmp_result = FILE_ERROR;

//	}

	return tmp_result;
}

