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
                          loader_it.h  -  description
                             -------------------
    begin                : Sun Jan 21 2001
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

#ifndef LOADER_IT_H
#define LOADER_IT_H

#include "loader.h"
/**
  *@author Juan Linietsky
  */

/******************************
 loader_it.h
 ----------
Impulse Tracker Module Loader!
It lacks support for old
instrument files methinks...
and some other things like
midi.
********************************/

#define ITENVCNT 25




class Loader_IT : public Loader  {


	enum IT_Sample_Flags {

		IT_SAMPLE_EXISTS=1,
		IT_SAMPLE_16BITS=2,
		IT_SAMPLE_STEREO=4,
		IT_SAMPLE_COMPRESSED=8,
		IT_SAMPLE_LOOPED=16,
		IT_SAMPLE_SUSTAIN_LOOPED=32,
		IT_SAMPLE_LOOP_IS_PINGPONG=64,
		IT_SAMPLE_SUSTAIN_LOOP_IS_PINGPONG=128
	};

	struct IT_Header {
		char    identifier[5]; // 4 bytes;
		char	songname[27]; // 26 bytes
		Uint8   hlmin;
		Uint8   hlmaj;
		Uint8	blank01[2];
		Uint16	ordnum;
		Uint16	insnum;
		Uint16	smpnum;
		Uint16	patnum;
		Uint16	cwt;		/* Created with tracker (y.xx = 0x0yxx) */
		Uint16	cmwt;		/* Compatible with tracker ver > than val. */
		Uint16	flags;
		Uint16	special;	/* bit 0 set = song message attached */
		Uint8	globvol;
		Uint8	mixvol;		/* mixing volume [ignored] */
		Uint8	initspeed;
		Uint8	inittempo;
		Uint8	pansep;		/* panning separation between channels */
		Uint8	zerobyte;
		Uint16	msglength;
		Uint32	msgoffset;
		Uint8	blank02[4];
		Uint8	pantable[64];
		Uint8	voltable[64];
	};

	struct IT_Sample {
		char	header[5];
		char	filename[13];
		Uint8	zerobyte;
		Uint8	globvol;
		Uint8	flag;
		Uint8	volume;
		Uint8	panning;
		char	sampname[29];
		Uint8	convert;	/* sample conversion flag */

		Sample_Data data;
		Uint32	c5spd;

		Uint32	sampoffset;
		Uint8	vibspeed;
		Uint8	vibdepth;
		Uint8	vibrate;
		Uint8	vibwave;	/* 0=sine, 1=rampdown, 2=square, 3=random (speed ignored) */

		int real_sample_index; // <-This is a helper for instrument loading
		/* pointer to sampledata! */
	};

	struct IT_Instrument {
		char	header[5];
		Uint32	size;			/* (dword) Instrument size */
		char	filename[13];	/* (char) Instrument filename */
		Uint8	zerobyte;		/* (byte) Instrument type (always 0) */
		Uint8	volflg;
		Uint8	volpts;
		Uint8	volbeg;			/* (byte) Volume loop start (node) */
		Uint8	volend;			/* (byte) Volume loop end (node) */
		Uint8	volsusbeg;		/* (byte) Volume sustain begin (node) */
		Uint8	volsusend;		/* (byte) Volume Sustain end (node) */
		Uint8	panflg;
		Uint8	panpts;
		Uint8	panbeg;			/* (byte) channel loop start (node) */
		Uint8	panend;			/* (byte) channel loop end (node) */
		Uint8	pansusbeg;		/* (byte) channel sustain begin (node) */
		Uint8	pansusend;		/* (byte) channel Sustain end (node) */
		Uint8	pitflg;
		Uint8	pitpts;
		Uint8	pitbeg;			/* (byte) pitch loop start (node) */
		Uint8	pitend;			/* (byte) pitch loop end (node) */
		Uint8	pitsusbeg;		/* (byte) pitch sustain begin (node) */
		Uint8	pitsusend;		/* (byte) pitch Sustain end (node) */
		Uint16	blank;
		Uint8	globvol;
		Uint8	chanpan;
		Uint16	fadeout;		/* Envelope end / NNA volume fadeout */
		Uint8	dnc;			/* Duplicate note check */
		Uint8	dca;			/* Duplicate check action */
		Uint8	dct;			/* Duplicate check type */
		Uint8	nna;			/* New Note Action [0,1,2,3] */
                Uint8	IFC;
		Uint8	IFR;
		Uint8	midichan;
		Uint8	midiprog;
		Uint16	midibank;

		Uint16	trkvers;		/* tracker version used to save [files only] */
		Uint8	ppsep;			/* Pitch-pan Separation */
		Uint8	ppcenter;		/* Pitch-pan Center */
		Uint8	rvolvar;		/* random volume varations */
		Uint8	rpanvar;		/* random panning varations */
		Uint16	numsmp;			/* Number of samples in instrument [files only] */
		char	name[27];		/* Instrument name */
		Uint8	blank01[4];
		Uint16	samptable[Note::NOTES];/* sample for each note [note / samp pairs] */
		Uint8	volenv[200];	     /* volume envelope (IT 1.x stuff) */
		Uint8	oldvoltick[ITENVCNT];/* volume tick position (IT 1.x stuff) */
		Uint8	volnode[ITENVCNT];   /* amplitude of volume nodes */
		Uint16	voltick[ITENVCNT];   /* tick value of volume nodes */
		Sint8	pannode[ITENVCNT];   /* panenv - node points */
		Uint16	pantick[ITENVCNT];   /* tick value of panning nodes */
		Sint8	pitnode[ITENVCNT];   /* pitchenv - node points */
		Uint16	pittick[ITENVCNT];   /* tick value of pitch nodes */

	};


	struct IT_Note {

		Uint8 note,ins,volpan,cmd,inf;
	};

	static const char * IT_Version[];

	/* Variables to store temp data */
	IT_Header header;
	IT_Sample **sample;
	IT_Instrument **instrument;
	Pattern **pattern;
        Uint8 orderlist[500];
        string message;

	/* Mantains the count of what was done */
	int sample_count;
	int instrument_count;
	int pattern_count;


	/* Samples Methods */
	Error load_samples();
	Error load_sample_info(IT_Sample *p_sample);
	Error load_sample_data(IT_Sample *p_sample);
	Error  load_sampledata();
	void transfer_sample_to_song(IT_Sample *p_src_sample,int p_dst_index);

	/* IT sample decompression helper functions */

        Uint32 *source_buffer; 	/* source buffer */
	Uint32 *source_position; 		/* actual reading position */
	Uint8 source_remaining_bits;		/* bits remaining in read dword */

	Uint32 read_n_bits_from_IT_compressed_block(Uint8 p_bits_to_read);
	int read_IT_compressed_block (bool p_16bits);
	void free_IT_compressed_block ();
	int load_sample_8bits_IT_compressed(void *p_dest_buffer,int p_buffsize);
	int load_sample_16bits_IT_compressed(void *p_dest_buffer,int p_buffsize);

	/* end of IT sample decompression functions */

        /* Instruments Methods */
	Error load_instruments();
	Error load_instrument_info(IT_Instrument *p_instrument);
	void transfer_instrument_to_song(IT_Instrument *p_instrument,int p_dest_index);

	/* Pattern Methods */
	Error load_patterns();

	/* Song Info Methods */
        Error load_header();
	Error load_orders();
        void transfer_info();
        Error load_message();

	/* Helpers */
	void clear_structs(bool p_clear_shared_data);
	Error load_body();
	Error give_up_and_clear_structs(Error p_reason);

        Uint32 temporary;
public:

	Error load(const char *p_filename,bool p_load_patterns);
	bool test(const char *p_filename);


	/* Compute Sample Info */
	int get_amount_of_samples();
	Sample_Data *get_sample_data(int p_sample_index);
	string get_sample_name(int p_sample_index);
	void add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument);

		Error load_sample(const char *p_filename,int p_dest_index);
	Sample_Data *load_sample(const char *p_filename);
        Error load_samples_from_instrument(const char *p_filename);

	/* Compute Instrument Info */
	Error load_instrument(const char *p_filename,int p_dest_index);

	void transfer_data_to_song();

	void free_info(bool free_sampledata=false);

	Loader_IT();
	~Loader_IT();
};



#endif
