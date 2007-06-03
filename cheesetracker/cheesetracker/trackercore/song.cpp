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
                          song.cpp  -  description
                             -------------------
    begin                : Sat Jan 20 2001
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

#include "song.h"
#include <stdio.h>
#include <cassert>


Song::Song_Variables::Song_Variables() :
	prop_name("Name",&name,25),
	prop_message("Message:",&message),

	prop_filename("File Name",&filename),

	prop_hilight_major("Row Hilight Major",&row_highlight_major,1,200),
	prop_hilight_minor("Row Hilight Minor",&row_highlight_minor,1,200),

	prop_mixing_volume("Mixing Volume",&mixing_volume,0,128),
	prop_stereo_separation("Stereo Separation",&stereo_separation,0,128),

	prop_use_stereo("Stereo",&use_stereo),
	prop_use_linear_slides("Linear Slides",&use_linear_slides),

	prop_old_effects("Old Effects",&old_effects),
	prop_compatible_gxx("Compatible GXX",&compatible_gxx)

{


}

Song::Initial_Variables::Initial_Variables() :
 
	prop_global_volume("Global Volume",&global_volume,0,128),
	prop_speed("Speed",&speed,1,255),
	prop_tempo("Tempo",&tempo,31,255)
	
 {


}

Song::Initial_Variables::Channel_State::Channel_State() :
 
	prop_pan("Pan",&pan,0,64),
	prop_volume("Volume",&volume,0,64),

	prop_surround("Surround",&surround),
	prop_mute("Mute",&mute)

 {


}

Song::Song(){

	reset_data();

}
Song::~Song(){

	reset_data();
}

Pattern* Song::get_pattern(int p_pattern) {

	assert(p_pattern<MAX_PATTERNS);
	return &pattern[p_pattern];
}

Sample* Song::get_sample(int p_sample) {

	assert(p_sample < MAX_SAMPLES);
	return &sample[p_sample];

}

Instrument* Song::get_instrument(int p_instrument) {

	assert(p_instrument < MAX_INSTRUMENTS);
	return &instrument[p_instrument];
}

Order Song::get_order(int p_position) {

	return order[p_position];

}
void Song::set_order(int p_position, Order p_order) {

	order[p_position]=p_order;

}


void Song::reset_data() {

	int i;

	for (i=0;i<MAX_PATTERNS;i++) {

		pattern[i].clear();
		pattern[i].set_length(64);
	}

	for (i=0;i<MAX_INSTRUMENTS;i++) {

		instrument[i].reset();

	}

	for (i=0;i<MAX_SAMPLES;i++) {

		sample[i].reset();
	}

 	for (i=0;i<MAX_ORDERS;i++) {

		order[i]=EMPTY_FIELD;
	}

	//printf("resetting name %p\n",&variables.name);
	const char *wh="";
	variables.name=wh;
	variables.filename="";
	variables.message="";

	variables.row_highlight_minor=4;
	variables.row_highlight_major=16;
	
	variables.mixing_volume=48;
	variables.stereo_separation=128;

	variables.use_stereo=true;
	variables.use_linear_slides=true;

	variables.old_effects=false;
	variables.compatible_gxx=false;

	initial_variables.global_volume=128;
	initial_variables.speed=6;
	initial_variables.tempo=150;

	for (i=0;i<PATTERN_WIDTH;i++) {

		initial_variables.channel[i].pan=32;
		initial_variables.channel[i].volume=64;
		initial_variables.channel[i].surround=false;
		initial_variables.channel[i].mute=false;
	}

}


int Song::find_sample_in_instrument(int p_sample) {

	int i;

	for (i=0;i<MAX_INSTRUMENTS;i++) {
		int j;

		for (j=0;j<Note::NOTES;j++) {

			for (int k=0;k<Instrument::MAX_LAYERS;k++)
				if (instrument[i].get_layer(k)->sample_number[j]==p_sample)
					return i;
		}

	}

	return p_sample;
}

int Song::find_unused_instrument() {

	int i,j;

	bool found;

	for (i=0;i<MAX_INSTRUMENTS;i++) {

		found=false;
		for (j=0;j<Note::NOTES;j++) {

			for (int k=0;k<Instrument::MAX_LAYERS;k++)
				if ((instrument[i].get_layer(k)->sample_number[j]<MAX_SAMPLES) && sample[instrument[i].get_layer(k)->sample_number[j]].in_use) {

					found=true;
				}
		}
		if (!found) return i;
	}

	return -1;
}


void Song::make_instrument_out_of_sample_at(int p_sample_pos,int p_instr_pos) {

        instrument[p_instr_pos].reset();
	instrument[p_instr_pos].name=sample[p_sample_pos].name;

	for (int j=0;j<Note::NOTES;j++) {

		instrument[p_instr_pos].get_layer(0)->sample_number[j]=p_sample_pos;
		instrument[p_instr_pos].get_layer(0)->note_number[j]=j;
	}


}

void Song::make_instrument_out_of_sample(int p_sample_pos) {

	int at_pos=find_unused_instrument();
	if (at_pos!=-1)
		make_instrument_out_of_sample_at(p_sample_pos,at_pos);
}


int Song::find_first_sample_in_instrument(int p_instrument) {

	int i,j;

	for (i=0;i<MAX_INSTRUMENTS;i++) {


		for (j=0;j<Note::NOTES;j++) {

			for (int k=0;k<Instrument::MAX_LAYERS;k++)
				if (instrument[p_instrument].get_layer(k)->sample_number[j]!=EMPTY_FIELD) {

					return instrument[p_instrument].get_layer(k)->sample_number[j];
				}
		}
	}

	return 0;
}

Order Song::get_next_order(int p_position) {

        int baseorder,order_counter;

        order_counter=-1;

        baseorder=p_position;

        do {

                baseorder++;
                if ( baseorder>(MAX_ORDERS-1)  ||
		     get_order(baseorder) == EMPTY_FIELD )
			return 0;
                order_counter++;

        } while ( ( order[baseorder]>(MAX_PATTERNS-1) ) && (order_counter<MAX_ORDERS) );


        if (order_counter==MAX_ORDERS) {

                return -1;

        } else {

                return baseorder;
        }


}

void Song::clear_unused_samples() {
	//this is buggy, but works for me, a proper one
	// should go thru orders and remember last used notes/instruments
	// in any case, it is very unlikely for this to not work.

	bool used_array[MAX_SAMPLES];
	memset(used_array,0,sizeof(bool)*MAX_SAMPLES);

	for (int i=0;i<MAX_PATTERNS;i++) {

		if (!pattern[i].in_use())
			continue;
		for (int x=0;x<PATTERN_WIDTH;x++) {
			for (int y=0;y<pattern[i].get_length();y++) {

				Note note=pattern[i].get_note(x,y);
				if (note.note>=Note::NOTES)
					continue;
				if (note.instrument>=MAX_INSTRUMENTS)
					continue;
				for (int l=0;l<Instrument::MAX_LAYERS;l++) {

					int sample_idx=instrument[note.instrument].get_layer(l)->sample_number[note.note];
					if ((sample_idx<0) || (sample_idx>=MAX_SAMPLES))
						continue;
					used_array[sample_idx]=true;
				}
			}
		}
	}

	for (int i=0;i<MAX_SAMPLES;i++) {

		if (sample[i].in_use && !used_array[i])
			sample[i].reset();
	}
}

void Song::clear_all_patterns() {

	for (int i=0;i<MAX_PATTERNS;i++) {
		pattern[i].clear();
	}
}
void Song::clear_all_instruments() {

	for (int i=0;i<MAX_INSTRUMENTS;i++) {
		instrument[i].reset();
	}

}
void Song::clear_all_orders() {

	for (int i=0;i<MAX_ORDERS;i++) {
		order[i]=ORDER_EMPTY;
	}

}
void Song::recreate_all_instruments() {

	for (int i=0;i<MAX_SAMPLES;i++) {
		if (sample[i].in_use)
			make_instrument_out_of_sample_at(i,i);
	}

}
void Song::clear_default_sample_pannings() {

	for (int i=0;i<MAX_SAMPLES;i++) {
		sample[i].def_panning_on=false;
	}

}
void Song::clear_default_instrument_pannings() {

	for (int i=0;i<MAX_INSTRUMENTS;i++) {
		for (int j=0;j<Instrument::MAX_LAYERS;j++) {

			instrument[i].get_layer(j)->panning.use_default=false;
		}
	}

}


int Song::find_pattern_in_orderlist(int p_pattern) {

	int i;

	for (i=0;i<MAX_ORDERS;i++) {

		if (p_pattern==order[i]) return i;

        }

	return -1;
}


int Song::get_patterns_used() {

	int i,max=0;

	for (i=0;i<MAX_PATTERNS;i++) {

   		//this seems to fix a bug in gdb? makes no sense.. but wtf.
   		if ((i<0) || (i>=MAX_PATTERNS)) {

     			//printf("******WARNING******* If you see this, please, _PLEASE_ mail coding@reduz.com.ar and let him know of your compiler version, libc version, etc\n");
       			return 50;
       		}
     			

		if (pattern[i].in_use()) {

			max=i+1;
		}
	}

	return max;

}

int Song::get_used_orders_amount() {

        int i;

	for (i=0;i<MAX_ORDERS;i++) {

         	if (order[i]==ORDER_EMPTY) return i;

	}
	return 256;
}

int Song::find_unused_sample_slot() {

	int i;

	for (i=0;i<MAX_SAMPLES;i++) {

		if (!sample[i].in_use) return i;
	}

	return -1;
}


