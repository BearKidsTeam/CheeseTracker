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
                          loader_it_info.cpp  -  description
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





Loader::Error Loader_IT::load_header() {
	//printf("**load header\n");
	string aux_string;

	file_read.get_byte_array((Uint8*)header.identifier,4);
	header.identifier[4]=0;
	aux_string=(char*)header.identifier;

        if ( aux_string!="IMPM" ) return FILE_FORMAT_NOT_RECOGNIZED;

	file_read.get_byte_array((Uint8*)header.songname,26);
//	file_read.get_byte_array((Uint8*)header.blank01,2);
	header.hlmin=file_read.get_byte();
	header.hlmaj=file_read.get_byte();

	if (header.hlmin==0) header.hlmin=4;
	if (header.hlmaj==0) header.hlmaj=16;

	header.ordnum=file_read.get_word();
	header.insnum=file_read.get_word();
	header.smpnum=file_read.get_word();
	header.patnum=file_read.get_word();
	header.cwt=file_read.get_word();		/* Created with tracker (y.xx = 0x0yxx) */
	header.cmwt=file_read.get_word();		/* Compatible with tracker ver > than val. */
	header.flags=file_read.get_word();
	header.special=file_read.get_word();
	header.globvol=file_read.get_byte();
	header.mixvol=file_read.get_byte();		/* mixing volume [ignored] */
	header.initspeed=file_read.get_byte();
	header.inittempo=file_read.get_byte();
	header.pansep=file_read.get_byte();		/* panning separation between channels */
	header.zerobyte=file_read.get_byte();
	header.msglength=file_read.get_word();
	header.msgoffset=file_read.get_dword();
	file_read.get_byte_array((Uint8*)header.blank02,4);
	file_read.get_byte_array((Uint8*)header.pantable,64);
	file_read.get_byte_array((Uint8*)header.voltable,64);

	if ( file_read.eof_reached() ) return HEADER_CORRUPT;
	if ( file_read.error_loading() ) return FILE_ERROR;
	//printf("**load header complete\n");

/*
	cout << "Success loading header!" <<endl;

	cout << "NAME: " << (char*)header.songname << endl;
	cout << "ORDERS: " << header.ordnum << endl;
	cout << "INSTRUMENTS: " << header.insnum << endl;
	cout << "SAMPLES: " << header.smpnum << endl;
	cout << "PATTERNS: " << header.patnum << endl;
 	cout << "Initial Tempo: " << (int)header.inittempo << endl;
	cout << "Initial Speed: " << (int)header.initspeed << endl;
*/
	sample_count=0;

	return SUCCESS;
}

Loader::Error Loader_IT::load_message() {

	if (!(header.special & 1)) {

		message="";
		return SUCCESS;
	}		

	char *tmpmsg;
	int i;

	file_read.seek(header.msgoffset);

	//(void*)tmpmsg=malloc(header.msglength+1);
	tmpmsg=(char *)malloc(header.msglength+1);
	file_read.get_byte_array((Uint8*)tmpmsg,header.msglength);
	tmpmsg[header.msglength]=0;

	message=tmpmsg;
	free(tmpmsg);
	for (i=0;(unsigned)i<message.length();i++) if (message[i]==13) message[i]=10;

	if (file_read.eof_reached() || file_read.error_loading()) {


		return FILE_ERROR;

	}

	return SUCCESS;
}

Loader::Error Loader_IT::load_orders() {

	file_read.seek(0xC0);
	file_read.get_byte_array((Uint8*)orderlist,header.ordnum);

	if (file_read.eof_reached() || file_read.error_loading()) {


		return FILE_ERROR;

	}

	return SUCCESS;
}


void Loader_IT::transfer_info() {

	int i;

	song->variables.name=header.songname;

	song->variables.row_highlight_major=header.hlmaj;
	song->variables.row_highlight_minor=header.hlmin;

	song->variables.use_stereo=(header.flags & 1);
	//song->variables.use_instruments=(header.flags & 4);
	song->variables.use_linear_slides=(header.flags & 8);
	song->variables.old_effects=(header.flags & 16);
	song->variables.compatible_gxx=(header.flags & 32);

	song->variables.mixing_volume=header.mixvol;
	song->variables.stereo_separation=header.pansep;

	// initial variables

	song->initial_variables.global_volume=header.globvol;
	song->initial_variables.speed=header.initspeed;
	song->initial_variables.tempo=header.inittempo;

	for (i=0;i<header.ordnum;i++) {

		Uint8 tmp_order_value;

		tmp_order_value=orderlist[i];

		if (tmp_order_value==255)  {

			tmp_order_value=255;
		} else if (tmp_order_value==254)  {

			tmp_order_value=253;
		} else if (tmp_order_value<120)  {

			//nothing!
		} else {

			tmp_order_value=255;
		}

		song->set_order(i,tmp_order_value);
	}

	for (i=0;i<64;i++) {

		if (header.pantable[i]<65) {

			song->initial_variables.channel[i].pan=header.pantable[i];
		} else {

			song->initial_variables.channel[i].pan=32;
		}

		song->initial_variables.channel[i].mute=(header.pantable[i]>=128);
		song->initial_variables.channel[i].surround=(header.pantable[i]==100);
		song->initial_variables.channel[i].volume=header.voltable[i]>64?64:header.voltable[i];
	}

	song->variables.message=message;

}



