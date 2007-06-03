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
                          loader_it_instruments.cpp  -  description
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




Loader::Error Loader_IT::load_instrument_info(IT_Instrument *p_instrument) {

	int j;

       	p_instrument->header[4]=0; //lets just make sure.

	file_read.get_byte_array((Uint8*)p_instrument->header,4);	/* (const char) Instrument filename */

	if (strcmp(p_instrument->header,"IMPI") != 0 ) {

		return FILE_ERROR;
	}


	file_read.get_byte_array((Uint8*)p_instrument->filename,12);	/* (const char) Instrument filename */
	p_instrument->zerobyte=file_read.get_byte();		/* (byte) Instrument type (always 0) */
	p_instrument->nna=file_read.get_byte();			/* New Note Action [0,1,2,3] */
	p_instrument->dct=file_read.get_byte();			/* Duplicate check type */
	p_instrument->dca=file_read.get_byte();			/* Duplicate check action */
	p_instrument->fadeout=file_read.get_word();		/* Envelope end / NNA volume fadeout */
	p_instrument->ppsep=file_read.get_byte();			/* Pitch-pan Separation */
	p_instrument->ppcenter=file_read.get_byte();		/* Pitch-pan Center */
	p_instrument->globvol=file_read.get_byte();
	p_instrument->chanpan=file_read.get_byte();
	p_instrument->rvolvar=file_read.get_byte();		/* random volume varations */
	p_instrument->rpanvar=file_read.get_byte();		/* random panning varations */
	file_read.get_byte_array((Uint8*)p_instrument->blank01,2);
	p_instrument->numsmp=file_read.get_byte();		/* random panning varations */
	file_read.get_byte_array((Uint8*)p_instrument->blank01,1);
	file_read.get_byte_array((Uint8*)p_instrument->name,26);
	p_instrument->IFC=file_read.get_byte();
	p_instrument->IFR=file_read.get_byte();
	p_instrument->midichan=file_read.get_byte();
	p_instrument->midiprog=file_read.get_byte();
	p_instrument->midibank=file_read.get_word();

	for (j=0;j<Note::NOTES;j++) p_instrument->samptable[j]=file_read.get_word(); /* sample for each note [note / samp pairs] */

	p_instrument->volflg=file_read.get_byte();
	p_instrument->volpts=file_read.get_byte();
	p_instrument->volbeg=file_read.get_byte();			/* (byte) Volume loop start (node) */
	p_instrument->volend=file_read.get_byte();			/* (byte) Volume loop end (node) */
	p_instrument->volsusbeg=file_read.get_byte();		/* (byte) Volume sustain begin (node) */
	p_instrument->volsusend=file_read.get_byte();		/* (byte) Volume Sustain end (node) */

	for (j=0;j<ITENVCNT;j++) {

		p_instrument->volnode[j]=file_read.get_byte();
		p_instrument->voltick[j]=file_read.get_word();
	}

	p_instrument->zerobyte=file_read.get_byte();

	p_instrument->panflg=file_read.get_byte();
	p_instrument->panpts=file_read.get_byte();
	p_instrument->panbeg=file_read.get_byte();			/* (byte) channel loop start (node) */
	p_instrument->panend=file_read.get_byte();			/* (byte) channel loop end (node) */
	p_instrument->pansusbeg=file_read.get_byte();		/* (byte) channel sustain begin (node) */
	p_instrument->pansusend=file_read.get_byte();		/* (byte) channel Sustain end (node) */

	for (j=0;j<ITENVCNT;j++) {

		Uint8 tmp_byte;
		tmp_byte=file_read.get_byte();
		p_instrument->pannode[j]=tmp_byte;
		p_instrument->pantick[j]=file_read.get_word();
	}

	p_instrument->zerobyte=file_read.get_byte();

	p_instrument->pitflg=file_read.get_byte();
	p_instrument->pitpts=file_read.get_byte();
	p_instrument->pitbeg=file_read.get_byte();			/* (byte) pitch loop start (node) */
	p_instrument->pitend=file_read.get_byte();			/* (byte) pitch loop end (node) */
	p_instrument->pitsusbeg=file_read.get_byte();		/* (byte) pitch sustain begin (node) */
	p_instrument->pitsusend=file_read.get_byte();		/* (byte) pitch Sustain end (node) */

	for (j=0;j<ITENVCNT;j++) {

		Uint8 tmp_byte;
		tmp_byte=file_read.get_byte();
		p_instrument->pitnode[j]=tmp_byte;
		p_instrument->pittick[j]=file_read.get_word();
	}

	p_instrument->zerobyte=file_read.get_byte();

	p_instrument->header[4]=0;
	p_instrument->filename[13]=0;
	p_instrument->name[29]=0;

	return SUCCESS;

}


Loader::Error Loader_IT::load_instruments() {

	Loader::Error tmp_result = SUCCESS;
        Uint32 *instrument_offset;
	string aux_string;
	int i;

	instrument_offset=(Uint32 *)malloc(header.insnum*4);
	instrument=(IT_Instrument **)malloc(sizeof(IT_Instrument*)*header.insnum);

	file_read.seek(0xC0+header.ordnum);

	for (i=0;i<header.insnum;i++) {

         	instrument_offset[i]=file_read.get_dword();
		instrument[i]=new IT_Instrument;

	}



	for (i=0;i<header.insnum;i++) {


		file_read.seek(instrument_offset[i]);


//		Uint8	volenv[200];	     /* volume envelope (IT 1.x stuff) */
//		Uint8	oldvoltick[ITENVCNT];/* volume tick position (IT 1.x stuff) */

		load_instrument_info(instrument[i]);
		instrument_count++;

	}

	free(instrument_offset);

	if (file_read.eof_reached() || file_read.error_loading()) {

		//[[todo]] Clean up the mess (samples loaded)
		tmp_result = FILE_ERROR;
	}

	return tmp_result;
}


void Loader_IT::transfer_instrument_to_song(IT_Instrument *p_instrument,int p_dest_index) {

	int j;

	song->get_instrument(p_dest_index)->reset();
	song->get_instrument(p_dest_index)->name=p_instrument->name;

	song->get_instrument(p_dest_index)->filename=p_instrument->filename;
	song->get_instrument(p_dest_index)->get_layer(0)->NNA_type=p_instrument->nna;
	song->get_instrument(p_dest_index)->get_layer(0)->duplicate_check_type=p_instrument->dct;
	song->get_instrument(p_dest_index)->get_layer(0)->duplicate_check_action=p_instrument->dca;

	for (j=0;j<Note::NOTES;j++) {

		song->get_instrument(p_dest_index)->get_layer(0)->note_number[j]=(p_instrument->samptable[j] &255 );
		song->get_instrument(p_dest_index)->get_layer(0)->sample_number[j]=(p_instrument->samptable[j] >>8)-1;

	}

		/* volume flags */

	song->get_instrument(p_dest_index)->get_layer(0)->volume.global_amount=p_instrument->globvol;
	song->get_instrument(p_dest_index)->get_layer(0)->volume.fadeout=p_instrument->fadeout;
	song->get_instrument(p_dest_index)->get_layer(0)->volume.random_variation=p_instrument->rvolvar;
		/* volume envelope flags */

	song->get_instrument(p_dest_index)->get_layer(0)->volume.envelope.reset();
	song->get_instrument(p_dest_index)->get_layer(0)->volume.envelope.set_enabled(p_instrument->volflg & 1);
	song->get_instrument(p_dest_index)->get_layer(0)->volume.envelope.set_loop_enabled(p_instrument->volflg & 2);
	song->get_instrument(p_dest_index)->get_layer(0)->volume.envelope.set_sustain_loop_enabled(p_instrument->volflg & 4);

	for (j=0;j<p_instrument->volpts;j++) {

		song->get_instrument(p_dest_index)->get_layer(0)->volume.envelope.add_node_at_offset(
			p_instrument->voltick[j],
			p_instrument->volnode[j]
		);
	}

	song->get_instrument(p_dest_index)->get_layer(0)->volume.envelope.set_loop_begin(p_instrument->volbeg);
	song->get_instrument(p_dest_index)->get_layer(0)->volume.envelope.set_loop_end(p_instrument->volend);
	song->get_instrument(p_dest_index)->get_layer(0)->volume.envelope.set_sustain_loop_begin(p_instrument->volsusbeg);
	song->get_instrument(p_dest_index)->get_layer(0)->volume.envelope.set_sustain_loop_end(p_instrument->volsusend);

		/* panning flags */

	song->get_instrument(p_dest_index)->get_layer(0)->panning.default_amount=((p_instrument->chanpan&127)<65)?(p_instrument->chanpan&127):32;
	song->get_instrument(p_dest_index)->get_layer(0)->panning.use_default=!(p_instrument->chanpan&128);

	song->get_instrument(p_dest_index)->get_layer(0)->panning.random_variation=p_instrument->rpanvar;
	song->get_instrument(p_dest_index)->get_layer(0)->panning.pitch_separation=(Sint8)p_instrument->ppsep;
	song->get_instrument(p_dest_index)->get_layer(0)->panning.pitch_center=p_instrument->ppcenter;

		/* panning envelope flags */

	song->get_instrument(p_dest_index)->get_layer(0)->panning.envelope.reset();
	song->get_instrument(p_dest_index)->get_layer(0)->panning.envelope.set_enabled(p_instrument->panflg & 1);
	song->get_instrument(p_dest_index)->get_layer(0)->panning.envelope.set_loop_enabled(p_instrument->panflg & 2);
	song->get_instrument(p_dest_index)->get_layer(0)->panning.envelope.set_sustain_loop_enabled(p_instrument->panflg & 4);


	for (j=0;j<p_instrument->panpts;j++) {


		song->get_instrument(p_dest_index)->get_layer(0)->panning.envelope.add_node_at_offset(
			p_instrument->pantick[j],
			p_instrument->pannode[j]
		);
	}

	song->get_instrument(p_dest_index)->get_layer(0)->panning.envelope.set_loop_begin(p_instrument->panbeg);
	song->get_instrument(p_dest_index)->get_layer(0)->panning.envelope.set_loop_end(p_instrument->panend);
	song->get_instrument(p_dest_index)->get_layer(0)->panning.envelope.set_sustain_loop_begin(p_instrument->pansusbeg);
	song->get_instrument(p_dest_index)->get_layer(0)->panning.envelope.set_sustain_loop_end(p_instrument->pansusend);

		/* pitch envelope flags */

	song->get_instrument(p_dest_index)->get_layer(0)->pitch.envelope.reset();
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.envelope.set_enabled(p_instrument->pitflg & 1);
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.use_as_filter=p_instrument->pitflg & 128;
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.envelope.set_loop_enabled(p_instrument->pitflg & 2);
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.envelope.set_sustain_loop_enabled(p_instrument->pitflg & 4);

	for (j=0;j<p_instrument->pitpts;j++) {

		bool filt=song->get_instrument(p_dest_index)->get_layer(0)->pitch.use_as_filter;

		song->get_instrument(p_dest_index)->get_layer(0)->pitch.envelope.add_node_at_offset(
			p_instrument->pittick[j],
			(p_instrument->pitnode[j]+32)/
			(filt?2:1)-32
		);
	}

	song->get_instrument(p_dest_index)->get_layer(0)->pitch.envelope.set_loop_begin(p_instrument->pitbeg);
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.envelope.set_loop_end(p_instrument->pitend);
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.envelope.set_sustain_loop_begin(p_instrument->pitsusbeg);
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.envelope.set_sustain_loop_end(p_instrument->pitsusend);

	song->get_instrument(p_dest_index)->get_layer(0)->pitch.default_cutoff=(p_instrument->IFC&0x7F) /2;
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.default_Q=p_instrument->IFR&0x7F;
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.use_default_Q=p_instrument->IFR&0x80;
	song->get_instrument(p_dest_index)->get_layer(0)->pitch.use_default_cutoff=p_instrument->IFC&0x80;



// [[TODO]] midi stuff
//		song->get_instrument(i)-> =instrument[i]-> ;


}


Loader::Error Loader_IT::load_instrument(const char *p_filename,int p_dest_index) {

	int i;

        if (load_samples_from_instrument(p_filename)!=SUCCESS) {

		return FILE_ERROR;
	}

	for (i=0;i<sample_count;i++) {

		int new_slot=song->find_unused_sample_slot();

		if (new_slot!=-1) {
			transfer_sample_to_song(sample[i],new_slot);

		} else {
			sample[i]->data.alloc_channels(0);
		}

		sample[i]->real_sample_index=new_slot;
	}

	transfer_instrument_to_song(instrument[0],p_dest_index);


	for (i=0;i<Note::NOTES;i++) {

		int sample_pos;

		sample_pos=song->get_instrument(p_dest_index)->get_layer(0)->sample_number[i];

		if (sample_pos<MAX_INSTRUMENTS) {

			if (sample[sample_pos]->real_sample_index==-1) {

				song->get_instrument(p_dest_index)->get_layer(0)->sample_number[i]=EMPTY_FIELD;
			} else {

				song->get_instrument(p_dest_index)->get_layer(0)->sample_number[i]=sample[sample_pos]->real_sample_index;
			}
		}
	}

	//ok, all done mah friends!
        clear_structs(true);

	return SUCCESS;
}



