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


/***********p****************************************************************
                          loader_s3m.cpp  -  description
                             -------------------
    begin                : Tue Apr 24 2001
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

#include "loader_s3m.h"
#include "ns_autoptr.h"




int Loader_S3M::load_header() {

	int i;

        file_read.get_byte_array((Uint8*)header.songname,28);
        header.t1a=file_read.get_byte();
        header.type=file_read.get_byte();
        file_read.get_byte_array((Uint8*)header.unused1,2);
        header.ordnum=file_read.get_word();
        header.insnum=file_read.get_word();
        header.patnum=file_read.get_word();
        header.flags=file_read.get_word();
        header.tracker=file_read.get_word();
        header.fileformat=file_read.get_word();
        file_read.get_byte_array((Uint8*)header.scrm,4);
        header.scrm[4]=0;
        if (strcmp(header.scrm,"SCRM")!=0) return FILE_ERROR;

        header.mastervol=file_read.get_byte();
        header.initspeed=file_read.get_byte();
        header.inittempo=file_read.get_byte();
        header.mastermult=file_read.get_byte();
        header.ultraclick=file_read.get_byte();
        header.pantable=file_read.get_byte();
        file_read.get_byte_array((Uint8*)header.unused2,8);
        header.special=file_read.get_word();
        file_read.get_byte_array((Uint8*)header.channels,32);

        file_read.get_byte_array((Uint8*)orderlist,header.ordnum);

        header.scrm[4]=0;
	if (strcmp(header.scrm,"SCRM")!=0) return FILE_ERROR;
	//sample parapointers
	for (i=0;i<header.insnum;i++) {

		int parapointer;
		parapointer=file_read.get_word();
		parapointer=(parapointer*16);
		sample_parapointers[i]=parapointer;
	}
	//pattern
	for (i=0;i<header.patnum;i++) {

		int parapointer;
		parapointer=file_read.get_word();
		parapointer=(parapointer*16);
		pattern_parapointers[i]=parapointer;
	}

        if (header.pantable==252) {

	        file_read.get_byte_array((Uint8*)header.pannings,32);
	}

	return SUCCESS;


}

int Loader_S3M::load_sample(S3M_Sample *p_sample) {

		int real_sample_size;
		Mutex_Lock_Container *p_sample_lock = p_sample->data.lock();
		ns_autoptr<Mutex_Lock_Container> ns_p_sample_lock;
		ns_p_sample_lock.ptr_new(p_sample_lock);

	        p_sample->type=file_read.get_byte();
                file_read.get_byte_array((Uint8*)p_sample->filename,12);
		p_sample->filename[12]=0;
                p_sample->memsegh=file_read.get_byte();
                p_sample->memsegl=file_read.get_word();
                size_t sample_size=file_read.get_dword();
                p_sample->data.set_loop_begin(file_read.get_dword());
                p_sample->data.set_loop_end(file_read.get_dword()-1);

                p_sample->volume=file_read.get_byte();
                p_sample->dsk=file_read.get_byte();
                p_sample->pack=file_read.get_byte();

                p_sample->flags=file_read.get_byte();
                p_sample->c2spd=file_read.get_dword();
		file_read.get_dword(); //useless crap
		file_read.get_dword();
		file_read.get_dword();
                file_read.get_byte_array((Uint8*)p_sample->sampname,28);
		p_sample->sampname[28]=0;
		file_read.get_byte_array((Uint8*)p_sample->scrs,4);
		p_sample->scrs[4]=0;

                p_sample->data.set_loop_enabled(p_sample->flags&1);
                bool data_is_16bits=p_sample->flags&4;

		if ((p_sample->type!=1) || strcmp(p_sample->scrs,"SCRS")) return FILE_ERROR;

		p_sample->data.set_c5_freq(p_sample->c2spd<<1);

		file_read.seek((((Uint32)p_sample->memsegh<<16)|p_sample->memsegl)*16);

		real_sample_size=sample_size<<BITBOOL(p_sample->flags&4);

		p_sample->data.set_num_channels(1);
		size_t divisor = data_is_16bits ? 2 : 1;

		p_sample->data.set_size(real_sample_size/divisor);
		p_sample->data.seek(0);

		for(size_t ix=0; ix<real_sample_size/divisor; ix++) {
			sample_int_t buffer;
			if(data_is_16bits)
				buffer=CONVERT_FROM_TYPE(Sint16, file_read.get_word());
			else
				buffer=CONVERT_FROM_TYPE(Sint8, file_read.get_byte());
			p_sample->data.put_sample(&buffer);
		}

                p_sample->data.change_sign(); // <- signed/unsigned conversion

		return SUCCESS;

}

int Loader_S3M::load_sample_data(S3M_Sample *p_sample) {

		return SUCCESS;
}


void Loader_S3M::transfer_sample_to_song(S3M_Sample *p_sample,int p_dest_index,bool create_instrument) {

	Sample *smp = song->get_sample(p_dest_index);
	smp->reset();

	smp->name		= p_sample->sampname;
	smp->filename		= p_sample->filename;
	smp->def_volume		= p_sample->volume;
	smp->data		= p_sample->data;
	smp->in_use		= !smp->data.is_empty();

	song->make_instrument_out_of_sample_at(p_dest_index,p_dest_index);
}

int Loader_S3M::load_pattern(Pattern *p_pattern) {

        int row=0,flag,ch;
	Note n;
	int length,accum=0;

	length=file_read.get_word();
        p_pattern->set_length(64);

        /* clear pattern data */
        while((row<64) && (accum<=length) ) {
                flag=file_read.get_byte();
		accum++;

		n.clear();
                if(flag) {
                       // ch=remap[flag&31];
//                        ch=remap[flag&31];
//                        if(ch!=-1)
//                                n=s3mbuf[(64U*ch)+row];
//                        else
//                                n=&dummy;

			ch=flag&31;

                        if(flag&32) {
                                n.note=file_read.get_byte();
				if (n.note==255) {

					n.note=EMPTY_FIELD;
				} else if (n.note==254) {

					n.note=Note::CUT;
				} else {
				
                                	n.note=((n.note>>4)*12)+(n.note&0xF);
				}

                                n.instrument=file_read.get_byte()-1;
				accum+=2;

                        }
                        if(flag&64) {
                                n.volume=file_read.get_byte();
                                if (n.volume>64) n.volume=64;
				accum++;

                        }
                        if(flag&128) {
                                n.command=file_read.get_byte()-1;
                                n.parameter=file_read.get_byte();
				accum+=2;
                        }
		
			p_pattern->get_note_ref(ch,row)=n;
                } else row++;
        }
        return SUCCESS;


}

int Loader_S3M::load_samples() {

	int i;

	for(i=0;i<header.insnum;i++) {

		file_read.seek(sample_parapointers[i]);
		sample[i]=new S3M_Sample;
		load_sample(sample[i]);
		load_sample_data(sample[i]);

		sample_count++;
	}

	return SUCCESS;
}

int Loader_S3M::load_patterns() {

	int i;

	for(i=0;i<header.patnum;i++) {

		file_read.seek(pattern_parapointers[i]);
		pattern[i]=new Pattern;
		load_pattern(pattern[i]);
		pattern_count++;
	}
	return SUCCESS;

}

Loader::Error Loader_S3M::load(const char *p_filename,bool p_load_patterns) {

	if ( file_read.is_open() ) return LOADER_IN_USE;
	if ( file_read.open(p_filename) ) return CANNOT_OPEN_FILE;
        sample_count=0;
	pattern_count=0;

	if (load_header()) {
		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;
	}		

        sample=(S3M_Sample **)malloc(sizeof(S3M_Sample*)*header.insnum);
        pattern=(Pattern **)malloc(sizeof(Pattern*)*header.patnum);
	
	if (load_samples()) {
		file_read.close();
		free_info(true);
		return FILE_ERROR;
	}		

	if (p_load_patterns) {	
	
		if (load_patterns()) {
	
			file_read.close();
			free_info(true);
			return FILE_ERROR;
		}		
	}

	file_read.close();

	return SUCCESS;
}

bool Loader_S3M::test(const char *p_filename) {

	if ( file_read.is_open() ) return LOADER_IN_USE;
	if ( file_read.open(p_filename) ) return CANNOT_OPEN_FILE;

	if (load_header()) {
		file_read.close();
		return false;
	}		

	file_read.close();

	return true;
}
int Loader_S3M::get_amount_of_samples() {


	return 0;
}
Sample_Data *Loader_S3M::get_sample_data(int p_sample_index) {


	return NULL;
}
string Loader_S3M::get_sample_name(int p_sample_index) {


	return "";
}
void Loader_S3M::add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument) {

}
Loader::Error Loader_S3M::load_sample(const char *p_filename,int p_dest_index) {


	return FILE_ERROR;
}
Loader::Error Loader_S3M::load_samples_from_instrument(const char *p_filename) {

	return FILE_FORMAT_NOT_RECOGNIZED;
}
Loader::Error Loader_S3M::load_instrument(const char *p_filename,int p_dest_index) {

	return FILE_FORMAT_NOT_RECOGNIZED;
}
void Loader_S3M::transfer_data_to_song() {

	int i;

	song->variables.name=header.songname;
//	song->variables.filename=
	song->variables.message="";
	song->variables.row_highlight_minor=4;
	song->variables.row_highlight_minor=16;
        song->variables.mixing_volume=header.mastermult;
	song->variables.use_linear_slides=false;
	song->variables.old_effects=!(header.flags&64);
	song->variables.compatible_gxx=true;

        song->initial_variables.global_volume=header.mastervol;
        song->initial_variables.speed=header.initspeed;
        song->initial_variables.tempo=header.inittempo;

	//[TODO] Set Panning Positions

	for (i=0;i<header.ordnum;i++) song->set_order(i,orderlist[i]);


	for (i=0;i<sample_count;i++) transfer_sample_to_song(sample[i],i,false);
	for (i=0;i<pattern_count;i++) *song->get_pattern(i)=*pattern[i];



}

void Loader_S3M::free_info(bool free_sampledata) {

	int i;

	for (i=0;i<sample_count;i++) {

		delete sample[i];
	}

	free(sample);


	for (i=0;i<pattern_count;i++) {
	
		delete pattern[i];
	}

	free(pattern);

	sample_count=0;
	pattern_count=0;	

}



Loader_S3M::Loader_S3M(){

	format_name="Scream Tracker";
}
Loader_S3M::~Loader_S3M(){
}

