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
                          loader_it_samples.cpp  -  description
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
#include "player_data.h"
#include "ns_autoptr.h"



Loader::Error Loader_IT::load_sample_info(IT_Sample *p_sample) {

 	p_sample->header[4]=0; //lets just make sure.

       	file_read.get_byte_array((Uint8*)p_sample->header,4);

       	if (strcmp(p_sample->header,"IMPS")!=0) {
               	return HEADER_CORRUPT;
       	}

       	file_read.get_byte_array((Uint8*)p_sample->filename,12);
       	p_sample->zerobyte=file_read.get_byte();
       	p_sample->globvol=file_read.get_byte();
       	p_sample->flag=file_read.get_byte();
       	p_sample->volume=file_read.get_byte();
       	file_read.get_byte_array((Uint8*)p_sample->sampname,26);
       	p_sample->convert=file_read.get_byte();	/* sample conversion flag */
       	p_sample->panning=file_read.get_byte();
	size_t sample_size=file_read.get_dword();
       	p_sample->data.set_loop_begin( file_read.get_dword() );
	p_sample->data.set_loop_end( file_read.get_dword() );
	p_sample->c5spd=file_read.get_dword();
	/*p_sample->data.set_sustain_loop_begin=*/file_read.get_dword();
	/*p_sample->data.sustain_loop_end=*/file_read.get_dword();
	p_sample->sampoffset=file_read.get_dword();
	p_sample->vibspeed=file_read.get_byte();
	p_sample->vibdepth=file_read.get_byte();
	p_sample->vibrate=file_read.get_byte();
	p_sample->vibwave=file_read.get_byte();	/* 0=sine, 1=rampdown, 2=square, 3=random (speed ignored) */

	p_sample->data.set_loop_enabled( (p_sample->flag >> 4) & 1);
	p_sample->data.set_loop_ping_pong((p_sample->flag >> 6) & 1);
//	p_sample->data.sustain_loop_on=(p_sample->flag >> 5) & 1;
//	p_sample->data.sustain_pingpong_loop=(p_sample->flag >> 7) & 1;

	p_sample->data.set_size(sample_size);

	p_sample->data.set_c5_freq(p_sample->c5spd);
	//p_sample->data.import_frequency(p_sample->c5spd);

	//p_sample->data.data_ptr=NULL;
	p_sample->header[4]=0;
	p_sample->filename[13]=0;
	p_sample->sampname[29]=0;

	return SUCCESS;

}

Loader::Error Loader_IT::load_sample_data(IT_Sample *p_sample) {


	if ((p_sample->flag & IT_SAMPLE_EXISTS)) { 
		int aux_sample_properties = p_sample->flag & 10; // 2nd and 4th bytes only

		file_read.seek(p_sample->sampoffset);

		if(aux_sample_properties & IT_SAMPLE_COMPRESSED) {
				size_t size = p_sample->data.get_size();
				int multiplier=1;
				if(aux_sample_properties & IT_SAMPLE_16BITS)
					size *= 2;
				if(aux_sample_properties & IT_SAMPLE_STEREO) {
					size *= 2;
					multiplier=2;
					p_sample->data.set_num_channels(2);
				} else {
					p_sample->data.set_num_channels(1);
				}

				char *auxdest=new char[size];
				ns_autoptr<char> ns_auxdest;
				ns_auxdest.arr_new(auxdest);
				int status=0;

				// FIXME: Are these decompressors nearly identical?
				// The decompressors are assumed to handle only mono
				// samples (since the original author programmed this
				// function to give up if stereo was encountered).
				//
				// Therefore, twice as many samples will be decompressed
				// if the sample is stereo, resulting in the need for
				// the multiplier.

				if(aux_sample_properties & IT_SAMPLE_16BITS) 
					status = load_sample_16bits_IT_compressed((void*)auxdest, p_sample->data.get_size()*multiplier);
				else
					status = load_sample_8bits_IT_compressed((void*)auxdest, p_sample->data.get_size()*multiplier);

				if ( status ) {

					free(auxdest);
					return FILE_ERROR;
				}


				if(aux_sample_properties & IT_SAMPLE_STEREO) {
					size_t jx;
					p_sample->data.seek(0);

					// What's this? It's called, "Proper handling of stereo."
					// Added Sun Apr 8 02:53:35 EDT 2007 by <godless@users.sf.net>

					// jx increments by two samples (either 8 or 16 bit) and will not exceed
					// the size of the auxdest buffer because auxdest is twice as big, being
					// composed of pairs of samples instead of just individuals.
					//

					sample_int_t buffer[2];

					for(jx=0; !p_sample->data.eof_reached(); jx+=2) {
						for(size_t chan=0; chan < p_sample->data.num_channels(); chan++) {
							if(aux_sample_properties & IT_SAMPLE_16BITS) {
								buffer[chan] = CONVERT_FROM_TYPE(Sint16, ((Sint16*)auxdest)[jx+chan]);
							} 
							else {
								buffer[chan] = CONVERT_FROM_TYPE(Sint8, ((Sint8*)auxdest)[jx+chan]);
							}
						}
						p_sample->data.put_sample(buffer);
					}
					// The convert flag simply specifies if the sample is signed.
					// Other bits were proposed, but never implemented in Impulse
					// Tracker.

					if(!p_sample->convert)
						p_sample->data.change_sign();
				}
				else { // COMPRESSED MONO

					sample_int_t buffer;
					p_sample->data.seek(0);

					for(size_t ix=0; !p_sample->data.eof_reached(); ix++) {
						if(p_sample->flag & IT_SAMPLE_16BITS) 
							buffer=CONVERT_FROM_TYPE(Uint16, ((Uint16*)auxdest)[ix]);
						else
							buffer=CONVERT_FROM_TYPE(Uint8, ((Uint8*)auxdest)[ix]);
						p_sample->data.put_sample(&buffer);
					}
				}
			} else { // NOT COMPRESSED
				// IT is a dead format: It will never change to the point where more than
				// two channels will need to be supported. Compare this with the WAV
				// loader, where a proper channel loop is used because WAV format can
				// specify more than two channels.

				size_t size = p_sample->data.get_size();

				if(p_sample->flag & IT_SAMPLE_STEREO) {
					p_sample->data.set_num_channels(2);
					p_sample->data.seek(0);
					p_sample->data.set_size(size/2);

					while(!p_sample->data.eof_reached()) {
						sample_int_t buffer[2];
						for(size_t chan=0; chan<2; chan++) {
							if(p_sample->flag & IT_SAMPLE_16BITS) {
								buffer[chan] = CONVERT_FROM_TYPE(Sint16, file_read.get_word());
							}
							else {
								buffer[chan] = CONVERT_FROM_TYPE(Sint8, file_read.get_byte());
							}
						}
						p_sample->data.put_sample(buffer);

					}
				} else { // MONO
					sample_int_t buffer;
					p_sample->data.set_num_channels(1);
					p_sample->data.seek(0);
					p_sample->data.set_size(size);

					while(!p_sample->data.eof_reached()) {
						if(p_sample->flag & IT_SAMPLE_16BITS) 
							buffer=CONVERT_FROM_TYPE(Sint16, file_read.get_word());
						else
							buffer=CONVERT_FROM_TYPE(Sint8, file_read.get_byte());
						p_sample->data.put_sample(&buffer);
					}
				}

			}


	} 

	return SUCCESS;
}


Loader::Error Loader_IT::load_samples() {

	Error tmp_result = SUCCESS;
        Uint32 *sample_offset;
	string aux_string;
	int i;

	sample_offset=(Uint32 *)malloc(header.smpnum*4);
	sample=(IT_Sample **)malloc(sizeof(IT_Sample*)*header.smpnum);

	file_read.seek(0xC0+header.ordnum+header.insnum*4);

	for (i=0;i<header.smpnum;i++) {

         	sample_offset[i]=file_read.get_dword();
		sample[i]=new IT_Sample;



	}

	for (i=0;i<header.smpnum;i++) {

		file_read.seek(sample_offset[i]);

		if (load_sample_info(sample[i])) {

			ERROR("Error reading sample" << i);
		}

		sample_count++;
	
	}

	free(sample_offset);

	if (file_read.eof_reached() || file_read.error_loading()) {

		//[[todo]] Clean up the mess (samples loaded)

		tmp_result = FILE_ERROR;

	}

	return tmp_result;
}
/* * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE

 -The following sample decompression code is based on xmp's code.(http://xmp.helllabs.org) which is based in openCP code.

* NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE * NOTICE */

Uint32 Loader_IT::read_n_bits_from_IT_compressed_block (Uint8 p_bits_to_read) {

    Uint32 aux_return_value;
    Uint32 val;
    Uint8 *buffer=(Uint8*)source_position;

    if ( p_bits_to_read <= source_remaining_bits ) {

    	val=buffer[3];
	val<<=8;
    	val|=buffer[2];
	val<<=8;
    	val|=buffer[1];
	val<<=8;
    	val|=buffer[0];

	aux_return_value = val & ((1 << p_bits_to_read) - 1);
	val >>= p_bits_to_read;
	source_remaining_bits -= p_bits_to_read;

	buffer[3]=val>>24;
    	buffer[2]=(val>>16)&0xFF;
    	buffer[1]=(val>>8)&0xFF;
    	buffer[0]=(val)&0xFF;

    } else {
    	aux_return_value=buffer[3];
	aux_return_value<<=8;
    	aux_return_value|=buffer[2];
	aux_return_value<<=8;
    	aux_return_value|=buffer[1];
	aux_return_value<<=8;
    	aux_return_value|=buffer[0];

	Uint32 nbits = p_bits_to_read - source_remaining_bits;
//	aux_return_value = *source_position;
  	source_position++;
        buffer+=4;
    	val=buffer[3];
	val<<=8;
    	val|=buffer[2];
	val<<=8;
    	val|=buffer[1];
	val<<=8;
    	val|=buffer[0];
	aux_return_value |= ((val & ((1 << nbits) - 1)) << source_remaining_bits);
	val >>= nbits;
	source_remaining_bits = 32 - nbits;
	buffer[3]=val>>24;
    	buffer[2]=(val>>16)&0xFF;
    	buffer[1]=(val>>8)&0xFF;
    	buffer[0]=(val)&0xFF;

    }

    return aux_return_value;
}

int Loader_IT::read_IT_compressed_block (bool p_16bits) {

	Uint16 size;

	size=file_read.get_word();

	if (file_read.eof_reached() || file_read.error_loading()) return FILE_ERROR;

	source_buffer = (Uint32 *)calloc (4, (size >> 2) + 2);

	if (source_buffer==NULL) return FILE_ERROR;

	//if (p_16bits)
	//	file_read.get_word_array((Uint16*)source_buffer,size/2);
	//else
	file_read.get_byte_array((Uint8*)source_buffer,size);
	//printf("size %i %x\n",size,size);
	if (file_read.eof_reached() || file_read.error_loading()) {

		free (source_buffer);
		return FILE_ERROR;
	}

	source_position = source_buffer;
	source_remaining_bits = 32;

	return SUCCESS;
}

void Loader_IT::free_IT_compressed_block () {


	if (source_buffer!=NULL) free (source_buffer);

	source_buffer = NULL;

}

int Loader_IT::load_sample_8bits_IT_compressed(void *p_dest_buffer,int p_buffsize) {

	Sint8 *dest_buffer;		/* destination buffer which will be returned */
   	Uint16 block_length;		/* length of compressed data block in samples */
	Uint16 block_position;		/* position in block */
	Uint8 bit_width;			/* actual "bit width" */
	Uint16 aux_value;			/* value read from file to be processed */
	Sint8 d1, d2;		/* integrator buffers (d2 for it2.15) */
	Sint8 *dest_position;		/* position in output buffer */
	Sint8 v;			/* sample value */
	bool it215; // is this an it215 module?

	dest_buffer = (Sint8 *) p_dest_buffer;

	if (dest_buffer==NULL) return FILE_ERROR;

	memset (dest_buffer, 0, p_buffsize);

	dest_position = dest_buffer;

	it215=(header.cmwt==0x215);

	/* now unpack data till the dest buffer is full */

	while (p_buffsize) {
	/* read a new block of compressed data and reset variables */
		if ( read_IT_compressed_block(false) ) return FILE_ERROR;


		block_length = (p_buffsize < 0x8000) ? p_buffsize : 0x8000;

		block_position = 0;

		bit_width = 9;		/* start with width of 9 bits */

		d1 = d2 = 0;		/* reset integrator buffers */

	/* now uncompress the data block */
		while ( block_position < block_length ) {

			aux_value = read_n_bits_from_IT_compressed_block(bit_width);			/* read bits */

			if ( bit_width < 7 ) { /* method 1 (1-6 bits) */

				if ( aux_value == (1 << (bit_width - 1)) ) { /* check for "100..." */

					aux_value = read_n_bits_from_IT_compressed_block(3) + 1; /* yes -> read new width; */
		    			bit_width = (aux_value < bit_width) ? aux_value : aux_value + 1;
							/* and expand it */
		    			continue; /* ... next value */
				}

			} else if ( bit_width < 9 ) { /* method 2 (7-8 bits) */

				Uint8 border = (0xFF >> (9 - bit_width)) - 4;
							/* lower border for width chg */

				if ( aux_value > border && aux_value <= (border + 8) ) {

					aux_value -= border; /* convert width to 1-8 */
					bit_width = (aux_value < bit_width) ? aux_value : aux_value + 1;
							/* and expand it */
		    			continue; /* ... next value */
				}


			} else if ( bit_width == 9 ) { /* method 3 (9 bits) */

				if ( aux_value & 0x100 ) {			/* bit 8 set? */

					bit_width = (aux_value + 1) & 0xff;		/* new width... */
		    			continue;				/* ... and next value */
				}

			} else { /* illegal width, abort */

				free_IT_compressed_block();

				return FILE_ERROR;
			}

			/* now expand value to signed byte */
			if ( bit_width < 8 ) {

				Uint8 tmp_shift = 8 - bit_width;

				v=(aux_value << tmp_shift);
				v>>=tmp_shift;

			} else v = (Sint8) aux_value;

			/* integrate upon the sample values */
			d1 += v;
	    		d2 += d1;

			/* ... and store it into the buffer */
			*(dest_position++) = it215 ? d2 : d1;
			block_position++;

		}

		/* now subtract block lenght from total length and go on */
		free_IT_compressed_block();
		p_buffsize -= block_length;
	}


	return SUCCESS;
}

int Loader_IT::load_sample_16bits_IT_compressed(void *p_dest_buffer,int p_buffsize) {

	Sint16 *dest_buffer;		/* destination buffer which will be returned */
   	Uint16 block_length;		/* length of compressed data block in samples */
	Uint16 block_position;		/* position in block */
	Uint8 bit_width;			/* actual "bit width" */
	Uint32 aux_value;			/* value read from file to be processed */
	Sint16 d1, d2;		/* integrator buffers (d2 for it2.15) */
	Sint16 *dest_position;		/* position in output buffer */
	Sint16 v;			/* sample value */

	bool it215; // is this an it215 module?

	dest_buffer = (Sint16 *) p_dest_buffer;

	if (dest_buffer==NULL) return FILE_ERROR;

	memset (dest_buffer, 0, p_buffsize*2);

	dest_position = dest_buffer;

	it215=(header.cmwt==0x215);


	while (p_buffsize) {
	/* read a new block of compressed data and reset variables */
		if ( read_IT_compressed_block(true) ) {

			return FILE_ERROR;
		}


		block_length = (p_buffsize < 0x4000) ? p_buffsize : 0x4000;

		block_position = 0;

		bit_width = 17;		/* start with width of 9 bits */

		d1 = d2 = 0;		/* reset integrator buffers */

		while ( block_position < block_length ) {

			aux_value = read_n_bits_from_IT_compressed_block(bit_width);			/* read bits */

			if ( bit_width < 7 ) { /* method 1 (1-6 bits) */

				if ( (signed)aux_value == (1 << (bit_width - 1)) ) { /* check for "100..." */

					aux_value = read_n_bits_from_IT_compressed_block(4) + 1; /* yes -> read new width; */
		    			bit_width = (aux_value < bit_width) ? aux_value : aux_value + 1;
							/* and expand it */
		    			continue; /* ... next value */
				}

			} else if ( bit_width < 17 ) {

				Uint16 border = (0xFFFF >> (17 - bit_width)) - 8;

				if ( (int)aux_value > (int)border && (int)aux_value <= ((int)border + 16) ) {

					aux_value -= border; /* convert width to 1-8 */
					bit_width = (aux_value < bit_width) ? aux_value : aux_value + 1;
							/* and expand it */
		    			continue; /* ... next value */
				}


			} else if ( bit_width == 17 ) {

				if ( aux_value & 0x10000 ) {			/* bit 8 set? */

					bit_width = (aux_value + 1) & 0xff;		/* new width... */
		    			continue;				/* ... and next value */
				}

			} else { /* illegal width, abort */

			 	ERROR("Sample has illegal BitWidth ");

				free_IT_compressed_block();
			
				return FILE_ERROR;
			}

			/* now expand value to signed byte */
			if ( bit_width < 16 ) {

				Uint8 tmp_shift = 16 - bit_width;

				v=(aux_value << tmp_shift);
				v>>=tmp_shift;

			} else v = (Sint16) aux_value;

			/* integrate upon the sample values */
			d1 += v;
	    		d2 += d1;

			/* ... and store it into the buffer */
			*(dest_position++) = it215 ? d2 : d1;
			block_position++;

		}

		/* now subtract block lenght from total length and go on */
		free_IT_compressed_block();
		p_buffsize -= block_length;
	}


	return SUCCESS;

}

Loader::Error Loader_IT::load_sampledata() {

	int i;

	for (i=0;i<header.smpnum;i++) {

		load_sample_data(sample[i]);

	}

	return SUCCESS;
}

void Loader_IT::transfer_sample_to_song(IT_Sample *p_src_sample,int p_dst_index) {

	song->get_sample(p_dst_index)->reset(); // <- just to make sure that nothing bad might happen!

	song->get_sample(p_dst_index)->data=p_src_sample->data;
	song->get_sample(p_dst_index)->name=p_src_sample->sampname;
	song->get_sample(p_dst_index)->def_volume=p_src_sample->volume;
	song->get_sample(p_dst_index)->glb_volume=p_src_sample->globvol;
	song->get_sample(p_dst_index)->def_panning_on=p_src_sample->panning & 128;
	song->get_sample(p_dst_index)->def_panning=p_src_sample->panning & 127;
	song->get_sample(p_dst_index)->vibrato_type=p_src_sample->vibwave;
	song->get_sample(p_dst_index)->vibrato_speed=p_src_sample->vibspeed;
	song->get_sample(p_dst_index)->vibrato_depth=p_src_sample->vibdepth;
	song->get_sample(p_dst_index)->vibrato_rate=p_src_sample->vibrate;

	song->get_sample(p_dst_index)->in_use=p_src_sample->flag & IT_SAMPLE_EXISTS;

}


int Loader_IT::get_amount_of_samples() {

	int i,count=0;

	for (i=0;i<sample_count;i++) {

		if (!sample[i]->data.is_empty())
			count++;
	}

	return count;
}

Sample_Data *Loader_IT::get_sample_data(int p_sample_index) {

	int i,count=0;

	if ((p_sample_index<0) || (p_sample_index>=get_amount_of_samples())) return NULL;

	for (i=0;i<sample_count;i++) {

		if (!sample[i]->data.is_empty()) {

			if (count==p_sample_index) break;
			count++;
		}
	}

	return &sample[i]->data;
}

string Loader_IT::get_sample_name(int p_sample_index) {

	int i,count=0;

	if ((p_sample_index<0) || (p_sample_index>=get_amount_of_samples())) return NULL;

	for (i=0;i<sample_count;i++) {

		if (!sample[i]->data.is_empty()) {

			if (count==p_sample_index) break;
			count++;
		}
	}

	return sample[i]->sampname;


}

void Loader_IT::add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument) {

	int i,count=0;

	if ((p_sample_index<0) || (p_sample_index>=get_amount_of_samples())) return;

	for (i=0;i<sample_count;i++) {

		if (!sample[i]->data.is_empty()) {

			if (count==p_sample_index) break;
			count++;
		}
	}

	song->get_sample(p_dest_index)->reset();
	transfer_sample_to_song(sample[i],p_dest_index);

	if (create_instrument) {

		int aux_inst=song->find_unused_instrument();

		if (aux_inst!=-1) {

			song->get_instrument(aux_inst)->reset();
			song->get_instrument(aux_inst)->name=song->get_sample(p_dest_index)->name;

			for(i=0;i<Note::NOTES;i++) {

				song->get_instrument(aux_inst)->get_layer(0)->sample_number[i]=p_dest_index;
			}
		}
	}

}

Loader::Error Loader_IT::load_sample(const char *p_filename,int p_dest_index) {

	IT_Sample p_sample;

	if ( file_read.is_open() ) return LOADER_IN_USE;
	if ( file_read.open(p_filename) ) return CANNOT_OPEN_FILE;

	if (load_sample_info(&p_sample)) {

		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;
	}

	if (load_sample_data(&p_sample)) {

		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;
	}

	file_read.close();

        transfer_sample_to_song(&p_sample,p_dest_index);

	return SUCCESS;
}

Sample_Data *Loader_IT::load_sample(const char *p_filename) {

	IT_Sample p_sample;

	if ( file_read.is_open() ) return NULL;
	if ( file_read.open(p_filename) ) return NULL;

	if (load_sample_info(&p_sample)) {

		file_read.close();
		return NULL;
	}

	if (load_sample_data(&p_sample)) {

		file_read.close();
		return NULL;		
	}
		
	file_read.close();

	Sample_Data *new_sample = new Sample_Data;
	*new_sample=p_sample.data;

	return new_sample;
}

Loader::Error Loader_IT::load_samples_from_instrument(const char *p_filename) {

	IT_Instrument aux_instr;
	int i;

	if ( file_read.is_open() ) return LOADER_IN_USE;
	if ( file_read.open(p_filename) ) return CANNOT_OPEN_FILE;

	if (load_instrument_info(&aux_instr)) {

		file_read.close();
		return FILE_FORMAT_NOT_RECOGNIZED;		
	}

	//I seriously should do some error checking here!

	instrument=(IT_Instrument **)malloc(sizeof(IT_Instrument*));
	instrument[0]=new IT_Instrument;
	*instrument[0]=aux_instr;

	header.insnum=1;
	header.smpnum=aux_instr.numsmp;
	header.patnum=0;

	instrument_count=1;


	pattern=(Pattern **)malloc(0);
	pattern_count=0;

	file_read.seek(554);

	sample=(IT_Sample **)malloc(sizeof(IT_Sample*)*aux_instr.numsmp);
        sample_count=aux_instr.numsmp;

	for (i=0;i<aux_instr.numsmp;i++) {

		sample[i] = new IT_Sample;
		load_sample_info(sample[i]);
	}	

	for (i=0;i<aux_instr.numsmp;i++) {
   		


		load_sample_data(sample[i]);
	}	

	file_read.close();


	return SUCCESS;
}


