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
                          player_data_nna.cpp  -  description
                             -------------------
    begin                : Fri Apr 6 2001
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
#include "stdio.h"

void Player_Data::process_NNAs() {

	int i;



	for (i=0;i<PATTERN_WIDTH;i++) {

		Channel_Control *aux_chn_ctrl = &control.channel[i];

		if (!aux_chn_ctrl->kicking)
			continue;
		aux_chn_ctrl->kicking=false;

                for (int j=0;j<Instrument::MAX_LAYERS;j++) {

			bool k=false;

			if (aux_chn_ctrl->layers[j].slave_voice!=NULL) {

				Voice_Control *aux_voc_ctrl;

				aux_voc_ctrl=aux_chn_ctrl->layers[j].slave_voice;

				if (aux_voc_ctrl->layer.NNA_type != Instrument::NNA_NOTE_CUT) {
					/* Make sure the old MP_VOICE channel knows it has no
					master now ! */
					aux_chn_ctrl->layers[j].slave_voice=NULL;
					/* assume the channel is taken by NNA */

					switch (aux_voc_ctrl->layer.NNA_type) {
						case Instrument::NNA_NOTE_CONTINUE: {
							//very descriptive :)
						} break;
						case Instrument::NNA_NOTE_OFF: {


							aux_voc_ctrl->note.end_flags|=END_NOTE_OFF;

							if (!aux_voc_ctrl->volume.envelope_ctrl.active || aux_voc_ctrl->layer.ptr->volume.envelope.is_loop_enabled()) {

								aux_voc_ctrl->note.end_flags|=END_NOTE_KILL;
							}
						} break;
						case Instrument::NNA_NOTE_FADE: {

							aux_voc_ctrl->note.end_flags|=END_NOTE_FADE;
						} break;
					}
				} else {

					aux_chn_ctrl->layers[j].slave_voice->sudden_end_requested=true;
					aux_chn_ctrl->layers[j].slave_voice=NULL;

				}
			}

			if (aux_chn_ctrl->layers[j].duplicate_check_type!=Instrument::DCT_DISABLED) {
//				int i;

				for (Voice_List::iterator I=voice_list.begin();I!=voice_list.end();I++)
					if (!(*I)->mixing.voice.has_stopped()&&
					((*I)->master_channel.ptr==aux_chn_ctrl)&&
					(aux_chn_ctrl->layers[j].layer_ptr==(*I)->layer.ptr)) {

						Voice_Control *aux_voc_ctrl;

						aux_voc_ctrl=*I;

						k=false;
						switch (aux_chn_ctrl->layers[j].duplicate_check_type) {
							case Instrument::DCT_NOTE:
								if (aux_chn_ctrl->layers[j].note==aux_voc_ctrl->note.current)
									k=true;
								break;
							case Instrument::DCT_SAMPLE:
								if (aux_chn_ctrl->layers[j].sample_ptr==aux_voc_ctrl->sample.ptr)
									k=true;
								break;
							case Instrument::DCT_INSTRUMENT:
								k=true;
								break;
						}
						if (k)
							switch (aux_chn_ctrl->layers[j].duplicate_check_action) {
								case Instrument::DCA_NOTE_CUT: {
									aux_voc_ctrl->volume.fadeout=0;
								} break;
								case Instrument::DCA_NOTE_OFF: {

									aux_voc_ctrl->note.end_flags|=END_NOTE_OFF;

									if (!aux_voc_ctrl->volume.envelope_ctrl.active || aux_voc_ctrl->layer.ptr->volume.envelope.is_loop_enabled()) {

										aux_voc_ctrl->note.end_flags|=END_NOTE_KILL;
									}

								} break;
								case Instrument::DCA_NOTE_FADE: {
									aux_voc_ctrl->note.end_flags|=END_NOTE_FADE;
								} break;
							}
					}
			}

		}
	}
}
