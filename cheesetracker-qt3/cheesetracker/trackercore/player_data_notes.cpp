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
                          player_data_notes.cpp  -  description
                             -------------------
    begin                : Fri Feb 2 2001
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

#include "player_data.h"
#include <stdio.h>

void Player_Data::process_new_note(int p_track,Uint8 p_note) { // if there's really a new note....

	if (control.channel[p_track].real_note!=255) {
		control.channel[p_track].old_note=control.channel[p_track].real_note;

	}

	control.channel[p_track].real_note=p_note;

	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		control.channel[p_track].layers[i].kick=KICK_NOTE;
	}

	control.channel[p_track].sample_start_index=0;
	control.channel[p_track].sliding=0;
	control.channel[p_track].row_has_note=true;

	if (control.channel[p_track].panbrello_type) control.channel[p_track].panbrello_position=0;
}

void Player_Data::process_new_instrument(int p_track,Uint8 p_instrument) {

	if (p_instrument>=MAX_INSTRUMENTS) {

		ERROR("warning: track " << p_track <<" attempted to play an undefined instrument:" << p_instrument);
		return;
	}

	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		control.channel[p_track].layers[i].layer_ptr=song->get_instrument(p_instrument)->get_layer(i);
	}
	control.channel[p_track].retrig_counter=0;
	control.channel[p_track].tremor_position=0;
	control.channel[p_track].sample_offset_fine=0;
	control.channel[p_track].instrument_index=p_instrument;
	control.channel[p_track].instrument_ptr=song->get_instrument(p_instrument);


}


 // returns if it was able to process
bool Player_Data::process_note_and_instrument(int p_track,int p_note,int p_instrument) {

	bool aux_result;
	aux_result=false;

	control.channel[p_track].row_has_note=false; // wise man says.. "we dont have a note... until we really know we have a note".
	control.channel[p_track].new_instrument=false;

	if ( (p_note<0) && (p_instrument<0) ) return aux_result; // nothing to do here
	if ( (p_note==255) && (p_instrument==255) ) return aux_result;

	if ( (p_note>=0) && (p_note<120) ) {

		process_new_note(p_track,p_note);

	} else if (p_note==Note::CUT) {

		for (int i=0;i<Instrument::MAX_LAYERS;i++)
			control.channel[p_track].layers[0].aux_volume=0;
		return aux_result;

	} else if (p_note==Note::OFF) {

		for (int i=0;i<Instrument::MAX_LAYERS;i++) {

			if (control.channel[p_track].layers[i].layer_ptr!=NULL) {

				control.channel[p_track].layers[i].note_end_flags|=END_NOTE_OFF;

				if (!control.channel[p_track].layers[i].layer_ptr->volume.envelope.is_enabled() || control.channel[p_track].layers[i].layer_ptr->volume.envelope.is_loop_enabled()) {

					control.channel[p_track].layers[i].note_end_flags|=END_NOTE_KILL;
				}
			}
		}
		return aux_result;
	} else return aux_result; // invalid note!


	if ( (p_instrument>=0) && (p_instrument<MAX_INSTRUMENTS)) process_new_instrument(p_track,p_instrument);

	if (control.channel[p_track].instrument_index==255) {

		for (int i=0;i<Instrument::MAX_LAYERS;i++) {

			control.channel[p_track].layers[i].kick=KICK_NOTHING;
		}
		return aux_result;
	}

	control.channel[p_track].kicking=true;
	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		Sample *aux_sample; // current sample
		int dest_sample_index;

		if ( control.channel[p_track].layers[i].layer_ptr->sample_number[control.channel[p_track].real_note] >= MAX_SAMPLES) {

//			ERROR("Instrument " << p_instrument << " attempts to play undefined sample" <<  (int)control.channel[p_track].instrument_ptr->sample_number[control.channel[p_track].real_note] << " at note/sample table pos " << (int)control.channel[p_track].real_note);
			control.channel[p_track].layers[i].kick=KICK_NOTHING;
			continue;

		} else {
			dest_sample_index=control.channel[p_track].layers[i].layer_ptr->sample_number[control.channel[p_track].real_note];
			control.channel[p_track].layers[i].note=control.channel[p_track].layers[i].layer_ptr->note_number[control.channel[p_track].real_note];
		}

		/* channel or instrument determined panning ? */
		aux_sample=song->get_sample(dest_sample_index);

		if (control.channel[p_track].layers[i].sample_ptr!=aux_sample) {

			control.channel[p_track].layers[i].sample_changed=(control.channel[p_track].layers[i].period>0);
			control.channel[p_track].layers[i].sample_ptr=aux_sample;
			control.channel[p_track].layers[i].sample_index=dest_sample_index;
		} else control.channel[p_track].layers[i].sample_changed=false;
		
		if (!control.channel[p_track].layers[i].sample_ptr)
			control.channel[p_track].layers[i].sample_index=-1;

		control.channel[p_track].layers[i].panning=control.channel[p_track].channel_panning;

		/* set filter,if any ? */


		if (aux_sample->def_panning_on) {

			control.channel[p_track].layers[i].panning=(int)aux_sample->def_panning*255/64;

		} else if ( control.channel[p_track].layers[i].layer_ptr->panning.use_default ) {

			control.channel[p_track].layers[i].panning=(int)control.channel[p_track].layers[i].layer_ptr->panning.default_amount*255/64;
		}
        }




	//control.channel[p_track].base_speed=0;
	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		if (control.channel[p_track].layers[i].kick==KICK_NOTHING)
			continue;

                /* Pitch-Pan Separation */
		if ((control.channel[p_track].layers[i].layer_ptr->panning.pitch_separation!=0) && (control.channel[p_track].channel_panning!=PAN_SURROUND)){

			control.channel[p_track].layers[i].panning+=((control.channel[p_track].real_note-control.channel[p_track].layers[i].layer_ptr->panning.pitch_center)*control.channel[p_track].layers[i].layer_ptr->panning.pitch_separation)/8;

			if (control.channel[p_track].layers[i].panning<PAN_LEFT) control.channel[p_track].layers[i].panning=PAN_LEFT;
			if (control.channel[p_track].layers[i].panning>PAN_RIGHT) control.channel[p_track].layers[i].panning=PAN_RIGHT;
		}

		/* Random Volume Variation */
		if (control.channel[p_track].layers[i].layer_ptr->volume.random_variation>0) {

			control.channel[p_track].layers[i].random_volume_variation=100-(rand() % control.channel[p_track].layers[i].layer_ptr->volume.random_variation);

		} else {

			control.channel[p_track].layers[i].random_volume_variation=100;
		}


		/* Random Pan Variation */
		if ((control.channel[p_track].layers[i].layer_ptr->panning.random_variation>0) && (control.channel[p_track].layers[i].panning!=PAN_SURROUND)){

			int aux_pan_modifier;

			aux_pan_modifier=(rand() % (control.channel[p_track].layers[i].layer_ptr->panning.random_variation << 2));
			if ((rand() % 2)==1) aux_pan_modifier=0-aux_pan_modifier; /* it's 5am, let me sleep :) */

			control.channel[p_track].layers[i].panning+=aux_pan_modifier;

			if (control.channel[p_track].layers[i].panning<PAN_LEFT) control.channel[p_track].layers[i].panning=PAN_LEFT;
			if (control.channel[p_track].layers[i].panning>PAN_RIGHT) control.channel[p_track].layers[i].panning=PAN_RIGHT;


		}

		/*filter*/

		if (control.channel[p_track].layers[i].layer_ptr->pitch.use_default_cutoff) {

			control.channel[p_track].layers[i].filter.it_cutoff=control.channel[p_track].layers[i].layer_ptr->pitch.default_cutoff*2;

		}

		if (control.channel[p_track].layers[i].layer_ptr->pitch.use_default_Q) {

			control.channel[p_track].layers[i].filter.it_reso=control.channel[p_track].layers[i].layer_ptr->pitch.default_Q*2;

		}

		/*envelopes*/

		control.channel[p_track].layers[i].volume_envelope_on=control.channel[p_track].layers[i].layer_ptr->volume.envelope.is_enabled();
		control.channel[p_track].layers[i].panning_envelope_on=control.channel[p_track].layers[i].layer_ptr->panning.envelope.is_enabled();
		control.channel[p_track].layers[i].pitch_envelope_on=control.channel[p_track].layers[i].layer_ptr->pitch.envelope.is_enabled();
		control.channel[p_track].layers[i].NNA_type=control.channel[p_track].layers[i].layer_ptr->NNA_type;
		control.channel[p_track].layers[i].duplicate_check_type=control.channel[p_track].layers[i].layer_ptr->duplicate_check_type;
		control.channel[p_track].layers[i].duplicate_check_action=control.channel[p_track].layers[i].layer_ptr->duplicate_check_action;

		if (p_instrument<100) { // instrument change
					/* IT random volume variations: 0:8 bit fixed, and one bit for
					sign. */
			control.channel[p_track].layers[i].volume=control.channel[p_track].layers[i].aux_volume=control.channel[p_track].layers[i].sample_ptr->def_volume;
		}

		control.channel[p_track].layers[i].note_end_flags=END_NOTE_NOTHING; /* clears flags */

		int note_offset,finetune;

	//	note_offset=control.channel[p_track].sample_ptr->data.note_offset;
	//	finetune=control.channel[p_track].sample_ptr->data.finetune;

		note_offset=0; //middle-C
		finetune=0;

		control.channel[p_track].layers[i].slide_to_period=control.channel[p_track].layers[i].aux_period=get_period((Uint16)(control.channel[p_track].layers[i].note+note_offset)<<1,finetune,song->variables.use_linear_slides);

	}



	return true;
}

void Player_Data::process_volume_column(int p_track,Uint8 p_volume) {

	control.channel[p_track].current_volume_command=EMPTY_FIELD;
	control.channel[p_track].current_volume_parameter=EMPTY_FIELD;

	if (p_volume<65) {

		for (int i=0;i<Instrument::MAX_LAYERS;i++)
			control.channel[p_track].layers[i].aux_volume=p_volume;
	} else if (p_volume<125) {

		control.channel[p_track].current_volume_command=(p_volume-65) / 10;
		control.channel[p_track].current_volume_parameter=(p_volume-65) % 10;
	} else if (p_volume<193) {

		for (int i=0;i<Instrument::MAX_LAYERS;i++)
	        	control.channel[p_track].layers[i].panning=(p_volume-128)*PAN_RIGHT/64;

	} else if (p_volume<213) {

		control.channel[p_track].current_volume_command=((p_volume-193) / 10)+6;
		control.channel[p_track].current_volume_parameter=(p_volume-193) % 10;
	}
}


void Player_Data::process_note(int p_track,Note p_note) {


	process_note_and_instrument(p_track,p_note.note,p_note.instrument);
	process_volume_column(p_track,p_note.volume);
	control.channel[p_track].current_command=p_note.command;
	control.channel[p_track].current_parameter=p_note.parameter;

}


