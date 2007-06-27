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
                          saver_it.cpp  -  description
                             -------------------
    begin                : Sun Apr 22 2001
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


#include <algorithm>
#include "saver_it.h"
#include "ns_autoptr.h"


#define BITBOOL(m_exp) ((m_exp)?1:0)


Saver_IT::Saver_IT(){

	sample_count=0;
	pattern_count=0;
	instrument_count=0;
        orders_count=0;
	message_length=0;
}
Saver_IT::~Saver_IT(){
}

void Saver_IT::calculate_values() {

	int i;

	orders_count=1;
	for (i=MAX_ORDERS-1;i>=0;i--) {

		if (song->get_order(i)!=EMPTY_FIELD) {
			orders_count=i+2; //for reason unknown, IT wont load the last one if i dont do this
			break;
		}
	}
	if (orders_count>MAX_ORDERS) orders_count=MAX_ORDERS;

	for (i=MAX_SAMPLES-1;i>=0;i--) {

		sample_count=i+1;
		if ((song->get_sample(i)->in_use) || (song->get_sample(i)->name!="")) break;
	}
	for (i=MAX_INSTRUMENTS-1;i>=0;i--) {

		int j,samples_found=0;
		instrument_count=i+1;
		for (j=0;j<Note::NOTES;j++) if (song->get_instrument(i)->get_layer(0)->sample_number[j]!=EMPTY_FIELD) samples_found++;

		if (samples_found) break;
	}

	for (i=MAX_PATTERNS-1;i>=0;i--) {

		pattern_count=i+1;
		if (song->get_pattern(i)->in_use()) break;
	}

	message_length=song->variables.message.length()+1;
	if (message_length>8000) message_length=8000;

}

void Saver_IT::write_header() {

	const char* file_head="IMPM";
	char title[26];
	int i,tmpint;
	for(i=0;i<25;i++) title[i]=(i>=(signed)song->variables.name.length())?0:song->variables.name[i];
	title[25]=0;

	message_length=song->variables.message.length()+1;
	message_offset=0;


	writer.store_byte_array((Uint8*)file_head,4);
	writer.store_byte_array((Uint8*)title,26);
	writer.store_byte(song->variables.row_highlight_minor);
	writer.store_byte(song->variables.row_highlight_major);

	writer.store_word(orders_count);
	writer.store_word(instrument_count);
	writer.store_word(sample_count);
	writer.store_word(pattern_count);
	writer.store_word(0x214); // <- blex0rs
	writer.store_word(0x214); // <- blex0rs
	tmpint=(song->variables.use_stereo)|
	       (1<<2)| //uses instruments allways
               (song->variables.use_linear_slides<<3)|
               (song->variables.old_effects<<4)|
               (song->variables.compatible_gxx<<5);
	writer.store_word(tmpint); // <- blex0rs
	writer.store_word((song->variables.message.length()>0)?1:0);
	writer.store_byte(song->initial_variables.global_volume);
	writer.store_byte(song->variables.mixing_volume);
	writer.store_byte(song->initial_variables.speed);
	writer.store_byte(song->initial_variables.tempo);
	writer.store_byte(song->variables.stereo_separation);
	writer.store_byte(0);
	writer.store_word(message_length);

	// Let's store the message just after the end of the header, so we dont have to deal with
	// coming back to rewrite the header
	writer.store_dword(0);
	writer.store_dword(0);
	for (i=0;i<PATTERN_WIDTH;i++) {

		if (song->initial_variables.channel[i].surround) {

			writer.store_byte(100);
		} else if (song->initial_variables.channel[i].mute) {

			writer.store_byte(128);
		} else {

			writer.store_byte(song->initial_variables.channel[i].pan);
		}
	}
	for (i=0;i<PATTERN_WIDTH;i++) {

		writer.store_byte(song->initial_variables.channel[i].volume);
	}

	write_orders();
	// Clear those ugly parapointers


	for (i=0;i<(instrument_count+sample_count+pattern_count);i++) writer.store_dword(0);



}
void Saver_IT::write_sample(int p_sample_index) {

	write_sample_internal(p_sample_index,true);
}

void Saver_IT::write_sample_internal(int p_sample_index,bool p_write_data) {

	const char *sample_head="IMPS";
	const char *filename="XXXXXXXX.YYY";
	char name[26];
	int tmpint,i;

	for(i=0;i<25;i++) name[i]=(i>=(signed)song->get_sample(p_sample_index)->name.length())?0:song->get_sample(p_sample_index)->name[i];
	writer.store_byte_array((Uint8*)sample_head,4);
	writer.store_byte_array((Uint8*)filename,12);
	writer.store_byte(0);
	writer.store_byte(song->get_sample(p_sample_index)->glb_volume);
	//FLAGS!
	tmpint=BITBOOL(song->get_sample(p_sample_index)->in_use) |
     	       (BITBOOL(song->get_sample(p_sample_index)->data.is_16bit())<<1)|
               BITBOOL(song->get_sample(p_sample_index)->data.num_channels() > 1) << 2  |
		// Flag 3 is zero - sample uncompressed
     	       (BITBOOL(song->get_sample(p_sample_index)->data.is_loop_enabled())<<4)|
//     	       (BITBOOL(song->get_sample(p_sample_index)->data.sustain_loop_on)<<5)|
     	       (0<<5)|
     	       (BITBOOL(song->get_sample(p_sample_index)->data.is_loop_ping_pong())<<6)|
     	       (0<<7);
//     	       (BITBOOL(song->get_sample(p_sample_index)->data.sustain_pingpong_loop)<<7);

	writer.store_byte(tmpint);
	writer.store_byte(song->get_sample(p_sample_index)->def_volume);
	writer.store_byte_array((Uint8*)name,26);
	writer.store_byte(1); // signed!
	Uint8 defpan=song->get_sample(p_sample_index)->def_panning;
	defpan|=song->get_sample(p_sample_index)->def_panning_on?128:0;
	writer.store_byte(defpan);

	Mutex_Lock_Container *sample_data_lock;

	// Critical section!

	sample_data_lock = song->get_sample(p_sample_index)->data.lock(__FILE__, __LINE__);
	writer.store_dword(song->get_sample(p_sample_index)->data.get_size());
	writer.store_dword(song->get_sample(p_sample_index)->data.get_loop_begin());
	writer.store_dword(song->get_sample(p_sample_index)->data.get_loop_end());
	writer.store_dword(song->get_sample(p_sample_index)->data.get_c5_freq());
	delete sample_data_lock;

	// End critical section!

	writer.store_dword(0);
	writer.store_dword(0);
//	writer.store_dword(song->get_sample(p_sample_index)->data.sustain_loop_begin);
//	writer.store_dword(song->get_sample(p_sample_index)->data.sustain_loop_end);

 	writer.store_dword(writer.get_file_pos()+8);
	writer.store_byte(song->get_sample(p_sample_index)->vibrato_speed);
	writer.store_byte(song->get_sample(p_sample_index)->vibrato_depth);
	writer.store_byte(song->get_sample(p_sample_index)->vibrato_rate);
	writer.store_byte(song->get_sample(p_sample_index)->vibrato_type);

	/** SAMPLE IS STORED UNCOMPRESSED **/
	// CHANNELS AFTER THE SECOND ONE ARE DISCARDED

 	if (p_write_data) {

		if (song->get_sample(p_sample_index)->in_use) {

			Sample *smp = song->get_sample(p_sample_index);
			// Touch the sample so that we don't access it
			// while it's being modified or while it's in
			// use_fixedpoint() mode.
			Mutex_Lock_Container *read_lock = smp->data.lock(__FILE__, __LINE__);
			// Ensure that the touch will be released at the end
			// of this if-block.
			ns_autoptr<Mutex_Lock_Container> ns_read_lock;
			ns_read_lock.ptr_new(read_lock);

			for(size_t ix=0; ix< smp->data.get_size(); ix++) {
				const sample_int_t *buffer = smp->data.get_int_sample();

				for(size_t chan=0;
				    chan < std::min<size_t>(2, smp->data.num_channels());
				    chan++) {
					if(smp->data.is_16bit())
						writer.store_word(CONVERT_TO_TYPE(Sint16, buffer[chan]));
					else
						writer.store_byte(CONVERT_TO_TYPE(Sint8, buffer[chan]));
				}
			}
		}
  	}


}
void Saver_IT::write_instrument(int p_instrument_index) {

	write_instrument_internal(song->get_instrument(p_instrument_index));
}

void Saver_IT::write_instrument_internal(Instrument *p_aux_instr) {

	char *instrument_head="IMPI";
	char *filename="XXXXXXXX.YYY";
	char name[26];
	int tmpint,i;
	bool sample_used[MAX_SAMPLES];
	int samples_used=0;
        Uint8 auxbyte;
        Instrument *instr=p_aux_instr;

	memset(sample_used,0,MAX_SAMPLES);

	for(i=0;i<25;i++) name[i]=(i>=(signed)instr->name.length())?0:instr->name[i];

	writer.store_byte_array((Uint8*)instrument_head,4);
	writer.store_byte_array((Uint8*)filename,12);
	writer.store_byte(0);
	writer.store_byte(instr->get_layer(0)->NNA_type);
	writer.store_byte(instr->get_layer(0)->duplicate_check_type);
	writer.store_byte(instr->get_layer(0)->duplicate_check_action);
	writer.store_word(instr->get_layer(0)->volume.fadeout);
	writer.store_byte(instr->get_layer(0)->panning.pitch_separation);
	writer.store_byte(instr->get_layer(0)->panning.pitch_center);
	writer.store_byte(instr->get_layer(0)->volume.global_amount);
	writer.store_byte(instr->get_layer(0)->panning.default_amount | (BITBOOL(!instr->get_layer(0)->panning.use_default)<<7));

	writer.store_byte(instr->get_layer(0)->volume.random_variation);
	writer.store_byte(instr->get_layer(0)->panning.random_variation);
	writer.store_word(0x214);

	for(i=0;i<Note::NOTES;i++) {

		if (instr->get_layer(0)->sample_number[i]<MAX_SAMPLES) {

			sample_used[instr->get_layer(0)->sample_number[i]]=true;
		}
	}
	for(i=0;i<MAX_SAMPLES;i++) {

		if (sample_used[i]) samples_used++;
	}

	writer.store_byte(samples_used);
	writer.store_byte(0);
	writer.store_byte_array((Uint8*)name,26);

	auxbyte=instr->get_layer(0)->pitch.default_cutoff*2;
	if (auxbyte>0x7F)
		auxbyte=0x7F;

	auxbyte|=instr->get_layer(0)->pitch.use_default_cutoff?0x80:0;
	writer.store_byte(auxbyte); // filter.. NOT implemented

	auxbyte=instr->get_layer(0)->pitch.default_Q;
	auxbyte|=instr->get_layer(0)->pitch.use_default_Q?0x80:0;
	writer.store_byte(auxbyte); // filter.. NOT implemented

	writer.store_byte(0); // midi channel.. NOT implemented
	writer.store_byte(0); // midi program.. NOT implemented
	writer.store_word(0); // midi bank.. NOT implemented

	for (i=0;i<Note::NOTES;i++) {

		writer.store_byte(instr->get_layer(0)->note_number[i]);
		writer.store_byte(instr->get_layer(0)->sample_number[i]+1);
	}

	/* VOLUME ENVELOPE */

	tmpint=(BITBOOL(instr->get_layer(0)->volume.envelope.is_enabled())) |
               (BITBOOL(instr->get_layer(0)->volume.envelope.is_loop_enabled())<<1) |
               (BITBOOL(instr->get_layer(0)->volume.envelope.is_sustain_loop_enabled())<<2);
	writer.store_byte(tmpint);
	writer.store_byte(instr->get_layer(0)->volume.envelope.get_node_count());
	writer.store_byte(instr->get_layer(0)->volume.envelope.get_loop_begin());
	writer.store_byte(instr->get_layer(0)->volume.envelope.get_loop_end());
	writer.store_byte(instr->get_layer(0)->volume.envelope.get_sustain_loop_begin());
	writer.store_byte(instr->get_layer(0)->volume.envelope.get_sustain_loop_end());

	for (i=0;i<25;i++) {

		writer.store_byte(instr->get_layer(0)->volume.envelope.get_node_height(i));
		writer.store_word(instr->get_layer(0)->volume.envelope.get_node_offset(i));
	}
        writer.store_byte(0);

	/* PANNING ENVELOPE */

	tmpint=(BITBOOL(instr->get_layer(0)->panning.envelope.is_enabled())) |
               (BITBOOL(instr->get_layer(0)->panning.envelope.is_loop_enabled())<<1) |
               (BITBOOL(instr->get_layer(0)->panning.envelope.is_sustain_loop_enabled())<<2);
	writer.store_byte(tmpint);
	writer.store_byte(instr->get_layer(0)->panning.envelope.get_node_count());
	writer.store_byte(instr->get_layer(0)->panning.envelope.get_loop_begin());
	writer.store_byte(instr->get_layer(0)->panning.envelope.get_loop_end());
	writer.store_byte(instr->get_layer(0)->panning.envelope.get_sustain_loop_begin());
	writer.store_byte(instr->get_layer(0)->panning.envelope.get_sustain_loop_end());

	for (i=0;i<25;i++) {

		writer.store_byte(instr->get_layer(0)->panning.envelope.get_node_height(i));
		writer.store_word(instr->get_layer(0)->panning.envelope.get_node_offset(i));

	}
        writer.store_byte(0);

	/* PITCH ENVELOPE */

	tmpint=(BITBOOL(instr->get_layer(0)->pitch.envelope.is_enabled())) |
               (BITBOOL(instr->get_layer(0)->pitch.envelope.is_loop_enabled())<<1) |
               (BITBOOL(instr->get_layer(0)->pitch.envelope.is_sustain_loop_enabled())<<2) |
               (BITBOOL(instr->get_layer(0)->pitch.use_as_filter)<<7);

	writer.store_byte(tmpint);
	writer.store_byte(instr->get_layer(0)->pitch.envelope.get_node_count());
	writer.store_byte(instr->get_layer(0)->pitch.envelope.get_loop_begin());
	writer.store_byte(instr->get_layer(0)->pitch.envelope.get_loop_end());
	writer.store_byte(instr->get_layer(0)->pitch.envelope.get_sustain_loop_begin());
	writer.store_byte(instr->get_layer(0)->pitch.envelope.get_sustain_loop_end());

	for (i=0;i<25;i++) {

		Sint8 height=((instr->get_layer(0)->pitch.envelope.get_node_height(i)+32)*
			(instr->get_layer(0)->pitch.use_as_filter?2:1));
		if (height>64)
			height=64;
		height-=32;
		writer.store_byte(height);
		writer.store_word(instr->get_layer(0)->pitch.envelope.get_node_offset(i));
	}

        writer.store_byte(0);

	//wasted.. eh?
        for (i=0;i<4;i++) writer.store_byte(0);

}

void Saver_IT::write_orders() {

	int i;

	for (i=0;i<orders_count;i++) {

         	if (song->get_order(i)==EMPTY_FIELD) {

			writer.store_byte(255);
		} else if (song->get_order(i)==ORDER_BREAK) {

			writer.store_byte(254);

		} else {

			writer.store_byte(song->get_order(i));
		}
	}
}

void Saver_IT::write_message() {

	if (message_length==0) return;

	message_offset=writer.get_file_pos();
	writer.store_byte_array((Uint8*)song->variables.message.c_str(),message_length);
}



void Saver_IT::write_pattern(int p_pattern) {

	Note note_cache[PATTERN_WIDTH];
	bool first_parameter[PATTERN_WIDTH];
	Uint8 mask_cache[PATTERN_WIDTH];

	Note empty_note;
        int i,j;
	int length;
	Uint32 begin_pos;
	Uint32 end_pos;


	enum {

		WRITE_NOTE=(1),
		WRITE_INSTRUMENT=(1<<1),
		WRITE_VOLUME=(1<<2),
		WRITE_COMMAND=(1<<3),
		WRITE_NOTE_MASK=(1<<4),
		WRITE_INSTRUMENT_MASK=(1<<5),
		WRITE_VOLUME_MASK=(1<<6),
		WRITE_COMMAND_MASK=(1<<7)
	};

	for(j=0;j<PATTERN_WIDTH;j++) {

		first_parameter[j]=true;
		mask_cache[j]=0;
	}


	begin_pos=writer.get_file_pos();

	writer.store_word(0);
	writer.store_word(song->get_pattern(p_pattern)->get_length());
	writer.store_dword(0);

        length=0;

	for (i=0;i<song->get_pattern(p_pattern)->get_length();i++) {

		for(j=0;j<PATTERN_WIDTH;j++) {

			if (!(empty_note==song->get_pattern(p_pattern)->get_note(j,i))) {
	                        // we have some data, boss

				int flags=0;
				Note src_note=song->get_pattern(p_pattern)->get_note(j,i);

				// Check for what changed, and fill cache!

				if (src_note.note!=EMPTY_FIELD) {

					if (src_note.note!=note_cache[j].note) {

						flags|=WRITE_NOTE;
						note_cache[j].note=src_note.note;

					} else {

						flags|=WRITE_NOTE_MASK;
					}
				}

				if (src_note.instrument!=EMPTY_FIELD) {

					if (src_note.instrument!=note_cache[j].instrument) {

						flags|=WRITE_INSTRUMENT;
						note_cache[j].instrument=src_note.instrument;

					} else {

						flags|=WRITE_INSTRUMENT_MASK;
					}
				}


				if (src_note.volume!=EMPTY_FIELD) {

					if (src_note.volume!=note_cache[j].volume) {

						flags|=WRITE_VOLUME;
						note_cache[j].volume=src_note.volume;

					} else {

						flags|=WRITE_VOLUME_MASK;
					}
				}

				if ((src_note.command!=EMPTY_FIELD) || (src_note.parameter!=0)){

					if ((src_note.command!=note_cache[j].command) || (src_note.parameter!=note_cache[j].parameter)) {

						flags|=WRITE_COMMAND;
						note_cache[j].command=src_note.command;
						note_cache[j].parameter=src_note.parameter;
						if ((note_cache[j].command==('Z'-'A')) && (note_cache[j].parameter<0x7F)) {
							note_cache[j].parameter*=2;
							if (note_cache[j].parameter>=0x7F)
								note_cache[j].parameter=0x7F;
						}


					} else {

						flags|=WRITE_COMMAND_MASK;
					}
				}

				//Now let's write the note

				//first the flags
				if (flags!=mask_cache[j]) {

					int tmp_chanmark;
					tmp_chanmark=(j+1)|128;
					writer.store_byte(tmp_chanmark);
					writer.store_byte(flags);
					length+=2;
					mask_cache[j]=flags;
				} else {
                                        int tmp_chanmark;
					tmp_chanmark=(j+1);
					writer.store_byte(tmp_chanmark);
					length++;
				}

				if (flags&WRITE_NOTE) {

					Uint8 dest_note;

					if (src_note.note==EMPTY_FIELD) {

                                         	dest_note=0;
					} else if (src_note.note==Note::OFF) {

                                         	dest_note=255;
					} else if (src_note.note==Note::CUT) {

                                         	dest_note=254;
					} else {

                                         	dest_note=src_note.note;
                                        }

					writer.store_byte(dest_note);
					length++;
				}

				if (flags&WRITE_INSTRUMENT) {

					writer.store_byte(src_note.instrument+1);
					length++;

				}

				if (flags&WRITE_VOLUME) {

					writer.store_byte(src_note.volume);
					length++;
				}

				if (flags&WRITE_COMMAND) {

					writer.store_byte(src_note.command+1);
					writer.store_byte(src_note.parameter);
					length+=2;
				}

			}

		}
		writer.store_byte(0);
		length++;
	}

        end_pos=writer.get_file_pos();
        writer.set_file_pos(begin_pos);
	writer.store_word(length);
        writer.set_file_pos(end_pos);
}




void Saver_IT::write_song() {

	int i;
	calculate_values();
	write_header();
	write_message();



	for (i=0;i<instrument_count;i++) {

		instrument_offsets[i]=writer.get_file_pos();


		write_instrument(i);
	}



	for (i=0;i<sample_count;i++) {

		sample_offsets[i]=writer.get_file_pos();


		write_sample(i);
	}



	for (i=0;i<pattern_count;i++) {

		pattern_offsets[i]=writer.get_file_pos();


		write_pattern(i);
	}

	writer.set_file_pos(0xC0+orders_count);




	for (i=0;i<instrument_count;i++) {


		writer.store_dword(instrument_offsets[i]);
	}
	for (i=0;i<sample_count;i++) {


		writer.store_dword(sample_offsets[i]);
	}
	for (i=0;i<pattern_count;i++) {


		writer.store_dword(pattern_offsets[i]);
	}



	writer.set_file_pos(0x38);
	writer.store_dword(message_offset);


}


int Saver_IT::save(const char *p_filename) {

	string filename=p_filename;

	if ( (filename.length()<4) || (toupper(filename[filename.length()-1])!='T') || (toupper(filename[filename.length()-2])!='I') ||  (toupper(filename[filename.length()-3])!='.') ) return UNKNOWN_TYPE;

	if (writer.open(p_filename)) {

		return SAVE_ERROR;
	}


	write_song();

	//I guess i should check for errors here...

	song->variables.filename=p_filename;
	writer.close();

	return FUNCTION_SUCCESS;

}

int Saver_IT::save_sample(const char *p_filename,int p_sample_index) {

 	string fname=p_filename,ext;
	Sample *smp;
	ext=fname.substr(fname.length()-3,3);

	if ((fname.length()<5) || ((ext!=".is") && (ext!=".IS")) ) return UNKNOWN_TYPE;

	smp=song->get_sample(p_sample_index);

	if (!smp->in_use) return SAVE_ERROR;

	if (writer.open(p_filename)) return SAVE_ERROR;

	write_sample(p_sample_index);
 	writer.close();

	return FUNCTION_SUCCESS;
}
int Saver_IT::save_instrument(const char *p_filename,int p_instrument_index) {

  	int i,j;
 	string fname=p_filename,ext;
	ext=fname.substr(fname.length()-3,3);
 	Instrument instr;
	vector<int> samples_used;
 	vector<int> sample_data_ptr_list;
  	int current_offset;

	if ((fname.length()<5) || ((ext!=".ii") && (ext!=".II")) ) return UNKNOWN_TYPE;

	instr=*song->get_instrument(p_instrument_index);

	if (writer.open(p_filename)) {

		return SAVE_ERROR;
	}

        //replace sample stuff for 1->x stuff
	for (i=0;i<Note::NOTES;i++) {

   		Uint8 sampnumb=instr.get_layer(0)->sample_number[i];
     		Uint8 newnumb=EMPTY_FIELD;
                bool has_it=false;

                //if sample slot is empty or points to an ununsed sample, skip it
                if (sampnumb==EMPTY_FIELD) continue;
                if (!song->get_sample(sampnumb)->in_use) {

			instr.get_layer(0)->sample_number[i]=EMPTY_FIELD;
   			continue;
                }


                //check if we have it cached
                for (j=0;j<(int)samples_used.size();j++)
                	if (samples_used[j]==sampnumb) {
                   		newnumb=j;
                 		has_it=true;
                   	}

                //if not, cache it
 		if (!has_it) {
     			samples_used.push_back(sampnumb);
        		newnumb=samples_used.size()-1;


          	}


		//and assign the new relative-to-zero number
  		instr.get_layer(0)->sample_number[i]=newnumb;
   	}

    	//and write the thing down!
	write_instrument_internal(&instr);

 	//now let's write samples..

	for (i=0;i<(int)samples_used.size();i++) {

		write_sample_internal(samples_used[i],false);
  		current_offset=writer.get_file_pos()-8;
	 	sample_data_ptr_list.push_back(current_offset);
   	}
 		
	for (i=0;i<(int)samples_used.size();i++) {

   		int prev_offset,aux_offset;

     		prev_offset=writer.get_file_pos();

		Sample *smp = song->get_sample(samples_used[i]);
		Mutex_Lock_Container *lock = smp->data.lock(__FILE__, __LINE__);
		ns_autoptr<Mutex_Lock_Container> ns_lock;
		ns_lock.ptr_new(lock);

		for(size_t ix=0; ix< smp->data.get_size(); ix++) {
			const sample_int_t *buffer = smp->data.get_int_sample();
			for(size_t chan=0;
			    chan< std::min<size_t>(2, smp->data.num_channels());
			    chan++) {
				if(smp->data.is_16bit())
					writer.store_word(CONVERT_TO_TYPE(Sint16, buffer[chan]));
				else
					writer.store_byte(CONVERT_TO_TYPE(Sint8, buffer[chan]));
			}
		}

  		aux_offset=writer.get_file_pos();

		writer.set_file_pos(sample_data_ptr_list[i]);
  		writer.store_dword(prev_offset);

		writer.set_file_pos(aux_offset);
  	}


	writer.close();

	return FUNCTION_SUCCESS;

}



