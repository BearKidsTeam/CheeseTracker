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
                          player_data_effects.cpp  -  description
                             -------------------
    begin                : Fri Mar 16 2001
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

/**********************
   complex effects
***********************/

void Player_Data::do_vibrato(int p_track,bool fine) {

        Uint8 q;
        Uint16 temp=0;

	if ((control.ticks_counter==0) && control.channel[p_track].row_has_note) control.channel[p_track].vibrato_position=0;

        q=(control.channel[p_track].vibrato_position>>2)&0x1f;

        switch (control.channel[p_track].vibrato_type) {
                case 0: /* sine */
                        temp=vibrato_table[q];
                        break;
                case 1: /* square wave */
                	temp=255;
                        break;
                case 2: /* ramp down */
                        q<<=3;
                        if (control.channel[p_track].vibrato_position<0) q=255-q;
                        temp=q;
                        break;
                case 3: /* random */
                        temp=rand() %256;//getrandom(256);
                        break;
        }

        temp*=control.channel[p_track].vibrato_depth;

	if (song->variables.old_effects) {

	        temp>>=7;
	} else {

	        temp>>=8;
	}

	if (!fine) temp<<=2;

	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		if (control.channel[p_track].vibrato_position>=0) {

			control.channel[p_track].layers[i].period=control.channel[p_track].layers[i].aux_period+temp;
		} else {

			control.channel[p_track].layers[i].period=control.channel[p_track].layers[i].aux_period-temp;
		}
	}

        if (!song->variables.old_effects || control.ticks_counter) control.channel[p_track].vibrato_position+=control.channel[p_track].vibrato_speed;
}


void Player_Data::do_pitch_slide_down(int p_track,Uint8 inf) {

	Uint8 hi,lo;

        if (inf) control.channel[p_track].pitch_slide_info=inf;
        else inf=control.channel[p_track].pitch_slide_info;

        hi=inf>>4;
        lo=inf&0xf;

	for (int i=0;i<Instrument::MAX_LAYERS;i++) {


		if (hi==0xf) {

			if (!control.ticks_counter) control.channel[p_track].layers[i].aux_period+=(Uint16)lo<<2;
		} else if (hi==0xe) {

			if (!control.ticks_counter) control.channel[p_track].layers[i].aux_period+=lo;
		} else {

			if (control.ticks_counter) control.channel[p_track].layers[i].aux_period+=(Uint16)inf<<2;
		}
	}
}

void Player_Data::do_pitch_slide_up(int p_track,Uint8 inf) {

	Uint8 hi,lo;

        if (inf) control.channel[p_track].pitch_slide_info=inf;
        else inf=control.channel[p_track].pitch_slide_info;

        hi=inf>>4;
        lo=inf&0xf;

	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		if (hi==0xf) {

			if (!control.ticks_counter) control.channel[p_track].layers[i].aux_period-=(Uint16)lo<<2;
		} else if (hi==0xe) {

			if (!control.ticks_counter) control.channel[p_track].layers[i].aux_period-=lo;
		} else {

			if (control.ticks_counter) control.channel[p_track].layers[i].aux_period-=(Uint16)inf<<2;
		}
	}
}

void Player_Data::do_pitch_slide_to_note(int p_track) {

        if (control.ticks_counter) {
                int dist;

                /* We have to slide a->period towards a->wantedperiod, compute the
                   difference between those two values */
		for (int i=0;i<Instrument::MAX_LAYERS;i++) {

			dist=control.channel[p_track].layers[i].period-control.channel[p_track].layers[i].slide_to_period;

		/* if they are equal or if portamentospeed is too big... */
			if ((!dist)||((control.channel[p_track].portamento_speed<<2)>abs(dist))) {
				/* ... make tmpperiod equal tperiod */
				control.channel[p_track].layers[i].aux_period=control.channel[p_track].layers[i].period=control.channel[p_track].layers[i].slide_to_period;
			} else {

				if (dist>0) {

					control.channel[p_track].layers[i].aux_period-=control.channel[p_track].portamento_speed<<2;
					control.channel[p_track].layers[i].period-=control.channel[p_track].portamento_speed<<2; /* dist>0 slide up */
				} else {
					control.channel[p_track].layers[i].aux_period+=control.channel[p_track].portamento_speed<<2;
					control.channel[p_track].layers[i].period+=control.channel[p_track].portamento_speed<<2; /* dist<0 slide down */
				}
			}
		}
        } else {

		for (int i=0;i<Instrument::MAX_LAYERS;i++)
	                control.channel[p_track].layers[i].aux_period=control.channel[p_track].layers[i].period;
	}
}

void Player_Data::do_tremor(int p_track) {

        Uint8 on,off,inf;

	inf=control.channel[p_track].current_parameter;

        if (inf) {
                control.channel[p_track].tremor_info=inf;
        } else {
                inf= control.channel[p_track].tremor_info;
                if (!inf) return;
        }

        //if (!control.ticks_counter) return;

        on=(inf>>4);
        off=(inf&0xf);

        control.channel[p_track].tremor_position%=(on+off);

	for (int i=0;i<Instrument::MAX_LAYERS;i++)
	        control.channel[p_track].layers[i].volume=(control.channel[p_track].tremor_position<on)?control.channel[p_track].layers[i].aux_volume:0;
        control.channel[p_track].tremor_position++;
}

void Player_Data::do_pan_slide(int p_track) {

        Uint8 lo,hi,inf;
        Sint16 pan;

	inf=control.channel[p_track].current_parameter;

        if (inf) control.channel[p_track].channel_pan_slide_info=inf;
        else inf=control.channel[p_track].channel_pan_slide_info;

        lo=inf&0xf;
        hi=inf>>4;
	for (int i=0;i<Instrument::MAX_LAYERS;i++) {
		pan=(control.channel[p_track].layers[i].panning==PAN_SURROUND)?PAN_CENTER:control.channel[p_track].layers[i].panning;

		if (!hi)
			pan+=lo<<2;
		else
		if (!lo) {
			pan-=hi<<2;
		} else
		if (hi==0xf) {
			if (!control.ticks_counter) pan+=lo<<2;
		} else
		if (lo==0xf) {
			if (!control.ticks_counter) pan-=hi<<2;
		}
		//this sets both chan & voice paning
		control.channel[p_track].layers[i].panning=(pan<PAN_LEFT)?PAN_LEFT:(pan>PAN_RIGHT?PAN_RIGHT:pan);
	}
	//this is for compatibility, i guess it doesnt really matter.
	control.channel[p_track].channel_panning=control.channel[p_track].layers[0].panning;
}

void Player_Data::do_volume_slide(int p_track,int inf) {

	Uint8 hi,lo;

	lo=inf&0xf;
	hi=inf>>4;

	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		if (!lo) {

			if ((control.ticks_counter)) control.channel[p_track].layers[i].aux_volume+=hi;

		} else if (!hi) {

			if ((control.ticks_counter)) control.channel[p_track].layers[i].aux_volume-=lo;

		} else if (lo==0xf) {

			if (!control.ticks_counter) control.channel[p_track].layers[i].aux_volume+=(hi?hi:0xf);
		} else if (hi==0xf) {

			if (!control.ticks_counter) control.channel[p_track].layers[i].aux_volume-=(lo?lo:0xf);
		} else continue;

		if (control.channel[p_track].layers[i].aux_volume<0) {

			control.channel[p_track].layers[i].aux_volume=0;
		} else if (control.channel[p_track].layers[i].aux_volume>64) {

			control.channel[p_track].layers[i].aux_volume=64;
		}
	}
}

void Player_Data::do_channel_volume_slide(int p_track) {

        Uint8 lo, hi,inf;

	inf=control.channel[p_track].current_parameter;

        if (inf) control.channel[p_track].channel_volume_slide_info=inf;
        inf=control.channel[p_track].channel_volume_slide_info;

        lo=inf&0xf;
        hi=inf>>4;

        if (!hi)
                control.channel[p_track].channel_volume-=lo;
        else
          if (!lo) {
                control.channel[p_track].channel_volume+=hi;
        } else
          if (hi==0xf) {
                if (!control.ticks_counter) control.channel[p_track].channel_volume-=lo;
        } else
          if (lo==0xf) {
                if (!control.ticks_counter) control.channel[p_track].channel_volume+=hi;
        }

        if (control.channel[p_track].channel_volume<0) control.channel[p_track].channel_volume=0;
        if (control.channel[p_track].channel_volume>64) control.channel[p_track].channel_volume=64;
}

void Player_Data::do_tremolo(int p_track) {

        Uint8 q;
        Sint16 temp=0;

	if ((control.ticks_counter==0) && control.channel[p_track].row_has_note) control.channel[p_track].tremolo_position=0;

        q=(control.channel[p_track].tremolo_position>>2)&0x1f;

        switch (control.channel[p_track].tremolo_type) {
                case 0: /* sine */
                        temp=vibrato_table[q];
                        break;
                case 1: /* ramp down */
                        q<<=3;
                        if (control.channel[p_track].tremolo_position<0) q=255-q;
                        temp=q;
                        break;
                case 2: /* square wave */
                        temp=255;
                        break;
                case 3: /* random */
                        temp=rand() % 256;//getrandom(256);
                        break;
        }

        temp*=control.channel[p_track].tremolo_depth;
        temp>>=7;

	for (int i=0;i<Instrument::MAX_LAYERS;i++) {


		if (control.channel[p_track].tremolo_position>=0) {


			control.channel[p_track].layers[i].volume=control.channel[p_track].layers[i].aux_volume+temp;
			if (control.channel[p_track].layers[i].volume>64) control.channel[p_track].layers[i].volume=64;
		} else {

			control.channel[p_track].layers[i].volume=control.channel[p_track].layers[i].aux_volume-temp;
			if (control.channel[p_track].layers[i].volume<0) control.channel[p_track].layers[i].volume=0;
		}
	}

        /*if (control.ticks_counter)*/ control.channel[p_track].tremolo_position+=control.channel[p_track].tremolo_speed;

}

void Player_Data::do_arpegio(int p_track) {

        Uint8 note,dat;
	//note=control.channel[p_track].note;
	note=0;

	if (control.channel[p_track].current_parameter) {

		control.channel[p_track].arpegio_info=control.channel[p_track].current_parameter;
	}

	dat=control.channel[p_track].arpegio_info;

	if (dat) {

		switch (control.ticks_counter%3) {

			case 1: {

	                        note+=(dat>>4);

			} break;
			case 2: {

                                note+=(dat&0xf);
			} break;
		}

		for (int i=0;i<Instrument::MAX_LAYERS;i++) {

			if (song->variables.use_linear_slides) {
				int peroidDelta = get_period((Uint16)46,0,song->variables.use_linear_slides)-get_period((Uint16)44,0,song->variables.use_linear_slides);
				control.channel[p_track].layers[i].period = control.channel[p_track].layers[i].aux_period - abs(peroidDelta)*note;
			} else { //@TODO fix this in some non-hacking way

				control.channel[p_track].layers[i].period=get_period( (((Uint16)control.channel[p_track].layers[i].note)+note)<<1,0,song->variables.use_linear_slides);
			}
		}
        	control.channel[p_track].has_own_period=true;
        }


}


void Player_Data::do_retrig(int p_track) {

	Uint8 inf;

	inf=control.channel[p_track].current_parameter;

        if (inf) {

       		control.channel[p_track].retrig_volslide=inf>>4;
                control.channel[p_track].retrig_speed=inf&0xf;
        }

        /* only retrigger if low nibble > 0 */
        if ( control.channel[p_track].retrig_speed>0) {

                if ( !control.channel[p_track].retrig_counter ) {
                        /* when retrig counter reaches 0, reset counter and restart the
                           sample */
			control.channel[p_track].retrig_counter=control.channel[p_track].retrig_speed;

			for (int i=0;i<Instrument::MAX_LAYERS;i++) {
				//if (control.channel[p_track].layers[i].kick!=KICK_NOTE)
				//	control.channel[p_track].layers[i].kick=KICK_NOTEOFF; //

				control.channel[p_track].layers[i].kick=KICK_NOTE;


				if ((control.ticks_counter)/*||(pf->flags&UF_S3MSLIDES)*/) {
					switch (control.channel[p_track].retrig_volslide) {
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							control.channel[p_track].layers[i].aux_volume-=(1<<(control.channel[p_track].retrig_volslide-1));
							break;
						case 6:
							control.channel[p_track].layers[i].aux_volume=(2*control.channel[p_track].layers[i].aux_volume)/3;
							break;
						case 7:
							control.channel[p_track].layers[i].aux_volume>>=1;
							break;
						case 9:
						case 0xa:
						case 0xb:
						case 0xc:
						case 0xd:
							control.channel[p_track].layers[i].aux_volume+=(1<<(control.channel[p_track].retrig_volslide-9));
							break;
						case 0xe:
							control.channel[p_track].layers[i].aux_volume=(3*control.channel[p_track].layers[i].aux_volume)>>1;
							break;
						case 0xf:
							control.channel[p_track].layers[i].aux_volume=control.channel[p_track].layers[i].aux_volume<<1;
							break;
					}
					if (control.channel[p_track].layers[i].aux_volume<0) control.channel[p_track].layers[i].aux_volume=0;
					else if (control.channel[p_track].layers[i].aux_volume>64) control.channel[p_track].layers[i].aux_volume=64;
				}
                        }
                }
                control.channel[p_track].retrig_counter--; /* countdown  */
        }
}

void Player_Data::do_global_volume_slide(int p_track) {

        Uint8 lo,hi,inf;

	inf=control.channel[p_track].current_parameter;

        if (inf) control.channel[p_track].global_volume_slide_info=inf;
        inf=control.channel[p_track].global_volume_slide_info;

        lo=inf&0xf;
        hi=inf>>4;

        if (!lo) {
                if (control.ticks_counter) control.global_volume+=hi;
        } else
          if (!hi) {
                if (control.ticks_counter) control.global_volume-=lo;
        } else
          if (lo==0xf) {
                if (!control.ticks_counter) control.global_volume+=hi;
        } else
          if (hi==0xf) {
                if (!control.ticks_counter) control.global_volume-=lo;
        }

        if (control.global_volume<0) control.global_volume=0;
        if (control.global_volume>128) control.global_volume=128;
}

void Player_Data::do_panbrello(int p_track) {

        Uint8 q;
        Sint32 temp=0;

        q=control.channel[p_track].panbrello_position;

        switch (control.channel[p_track].panbrello_type) {
                case 0: {/* sine */
                        temp=panbrello_table[q];
                } break;
                case 1: {/* square wave */
                        temp=(q<0x80)?64:0;
                } break;
                case 2: {/* ramp down */
                        q<<=3;
                        temp=q;
                } break;
                case 3: {/* random */
                        if (control.channel[p_track].panbrello_position>=control.channel[p_track].panbrello_speed) {
                                control.channel[p_track].panbrello_position=0;
                                temp=rand()%256;//getrandom(256);
			}
                } break;
        }


        temp=temp*(int)control.channel[p_track].panbrello_depth/0xF;
        temp<<=1;
	temp+=control.channel[p_track].channel_panning;

	for (int i=0;i<Instrument::MAX_LAYERS;i++)
		if (control.channel[p_track].layers[i].panning!=PAN_SURROUND)
		        control.channel[p_track].layers[i].panning=(temp<PAN_LEFT)?PAN_LEFT:(temp>PAN_RIGHT?PAN_RIGHT:temp);

        control.channel[p_track].panbrello_position+=control.channel[p_track].panbrello_speed;
}

/******************
      S effect
*******************/


void Player_Data::do_effect_S(int p_track) {

        Uint8 inf,c,dat;

	dat=control.channel[p_track].current_parameter;

        inf=dat&0xf;
        c=dat>>4;

        if (!dat) {
                c=control.channel[p_track].current_S_effect;
                inf=control.channel[p_track].current_S_data;
        } else {
                control.channel[p_track].current_S_effect=c;
                control.channel[p_track].current_S_data=inf;
        }

        switch (c) {
                case 1: {/* S1x set glissando voice */
		// this is unsupported in IT!

			//@TODO add something
  			//control.channel[p_track].chorus_send=inf*0xFF/0xF;

                }break;
                case 2: /* S2x set finetune */
		//Also not supported!
                        break;
                case 3: /* S3x set vibrato waveform */
			if (inf<4) control.channel[p_track].vibrato_type=inf;
                        break;
                case 4: /* S4x set tremolo waveform */
			if (inf<4) control.channel[p_track].tremolo_type=inf;
                        break;
                case 5: /* S5x panbrello */
			if (inf<4) control.channel[p_track].panbrello_type=inf;
                        break;
                case 6: {/* S6x delay x number of frames (patdly) */

			if (control.ticks_counter) break;
                        if (!control.pattern_delay_2) control.pattern_delay_1=inf+1; /* only once, when vbtick=0 */

                } break;
                case 7: /* S7x instrument / NNA commands */
                        //DoNNAEffects(inf);
                        break;
                case 8: {/* S8x set panning position */

//			if (pf->panflag) {
                                if (inf<=8) inf<<=4;
                                else inf*=17;
				control.channel[p_track].channel_panning=inf;
				for (int i=0;i<Instrument::MAX_LAYERS;i++)
	                                control.channel[p_track].layers[i].panning=inf;
//                        }
                } break;

                case 9: { /* S9x set surround sound */
                        //if (pf->panflag)

				for (int i=0;i<Instrument::MAX_LAYERS;i++)
	                                control.channel[p_track].layers[i].panning=control.channel[p_track].channel_panning=PAN_SURROUND;
                } break;
                case 0xA:{ /* SAy set high order sample offset yxx00h */

				if (control.channel[p_track].current_parameter) control.channel[p_track].hi_offset=(Uint32)inf<<16;
				control.channel[p_track].sample_start_index=control.channel[p_track].hi_offset|control.channel[p_track].lo_offset;
                } break;
                case 0xB: { /* SBx pattern loop */
                        if (control.ticks_counter) break;

                        if (inf) { /* set reppos or repcnt ? */
                                /* set repcnt, so check if repcnt already is set, which means we
                                   are already looping */
                                if (control.channel[p_track].pattern_loop_count>0)
                                        control.channel[p_track].pattern_loop_count--; /* already looping, decrease counter */
                                else {
                                        control.channel[p_track].pattern_loop_count=inf; /* not yet looping, so set repcnt */
                                }

                                if (control.channel[p_track].pattern_loop_count>0) { /* jump to reppos if repcnt>0 */

					control.position=control.previous_position; // This will also anulate any Cxx or break..

					control.position.current_row=control.channel[p_track].pattern_loop_position;
					control.position.forbid_jump=true;
				}

                        } else  {


				control.channel[p_track].pattern_loop_position=control.position.current_row-1;
			}

                } break;
                case 0xC: { /* SCx notecut */

			if (control.ticks_counter>=inf) {

				for (int i=0;i<Instrument::MAX_LAYERS;i++)
					control.channel[p_track].layers[i].aux_volume=0;
			}
                } break;
                case 0xD: {/* SDx notedelay */

			if (!control.ticks_counter) {

                                control.channel[p_track].note_delay=inf;

                        } else if (control.channel[p_track].note_delay) {

                                control.channel[p_track].note_delay--;
                        }

                } break;
                case 0xF: {/* SEx patterndelay */

			if (control.ticks_counter) break;
                        if (!control.pattern_delay_2) control.pattern_delay_1=inf+1; /* only once, when vbtick=0 */

                } break;
        }
}








/*********************
    volume effects
**********************/

void Player_Data::run_volume_column_effects(int p_track) {

	Uint8 param=control.channel[p_track].current_volume_parameter;


	switch ('A'+control.channel[p_track].current_volume_command) {

		case 'A': {

			if (param>0) control.channel[p_track].volcol_volume_slide=param;
			else param=control.channel[p_track].volcol_volume_slide;

   			do_volume_slide(p_track,param*0x10+0xF);

		} break;
		case 'B': {

			if (param>0) control.channel[p_track].volcol_volume_slide=param;
			else param=control.channel[p_track].volcol_volume_slide;

			do_volume_slide(p_track,0xF0+param);

		} break;
		case 'C': {

			if (param>0) control.channel[p_track].volcol_volume_slide=param;
			else param=control.channel[p_track].volcol_volume_slide;

			do_volume_slide(p_track,param*0x10);
		} break;
		case 'D': {

			if (param>0) control.channel[p_track].volcol_volume_slide=param;
			else param=control.channel[p_track].volcol_volume_slide;
			do_volume_slide(p_track,param);

		} break;
		case 'E': {

			do_pitch_slide_down(p_track,param<<2);
		} break;
		case 'F': {

			do_pitch_slide_up(p_track,param<<2);
		} break;
		case 'G': {

             	        const Uint8 slide_table[]={0,1,4,8,16,32,64,96,128,255};
			if (param) {

				control.channel[p_track].portamento_speed=slide_table[param];
			}

			if ( control.channel[p_track].old_note<=120) {

				for (int i=0;i<Instrument::MAX_LAYERS;i++) {

					if (!control.channel[p_track].layers[i].period)
						continue;
					if ( (!control.ticks_counter) && (control.channel[p_track].layers[i].sample_changed) ){


						control.channel[p_track].layers[i].kick=KICK_NOTE;
						control.channel[p_track].sample_start_index=0;

					} else {

						control.channel[p_track].layers[i].kick=(control.channel[p_track].layers[i].kick==KICK_NOTE)?KICK_ENVELOPE:KICK_NOTHING;
						control.channel[p_track].kicking=false;

					}
				}

				do_pitch_slide_to_note(p_track);
				control.channel[p_track].has_own_period=true;
			}

		} break;
		case 'H': {


			if (!control.ticks_counter) {
				if (param&0x0f) control.channel[p_track].vibrato_depth=param;
			}
			control.channel[p_track].doing_vibrato=true;
                        if (control.external_vibrato) break;

			//if (control.channel[p_track].layers[i].period) {

				do_vibrato(p_track,false);
				control.channel[p_track].has_own_period=true;
			//}

		} break;
	}
}
/*********************
        table
**********************/


void Player_Data::run_effects(int p_track) {

	switch ('A'+control.channel[p_track].current_command) {

		case 'A': {

			if ((control.ticks_counter>0) || (control.pattern_delay_2>0)) break;

                        int new_speed;

			new_speed=control.channel[p_track].current_parameter % 128;

			if (new_speed>0) {
				control.speed=new_speed;
				control.ticks_counter=0;
	        	}
		} break;
		case 'B': {

       			int next_order;

                        if (control.ticks_counter || control.position.forbid_jump) break;

			control.position.current_row=0;

			if (control.play_mode==PLAY_PATTERN) break;

       			next_order=song->get_next_order((int)control.channel[p_track].current_parameter-1);

       			if (next_order!=-1) {
       				// Do we have a "next order?"
       				control.position.current_pattern=song->get_order(next_order);
       				control.position.force_next_order=next_order;

       			} else {
       				// no, probably the user deleted the orderlist.
       				control.play_mode=PLAY_NOTHING;
       				reset();
       			}
		} break;
		case 'C': {

			int next_order;

                        if (control.ticks_counter || control.position.forbid_jump) break;

			control.position.current_row=control.channel[p_track].current_parameter;

			if (control.play_mode==PLAY_PATTERN) {

				if (control.position.current_row>=song->get_pattern(control.position.current_pattern)->get_length()) {

					control.position.current_row=0;
				}

				break;
			}

       			next_order=song->get_next_order(control.position.current_order);

       			if (next_order!=-1) {
       				// Do we have a "next order?"
       				control.position.current_pattern=song->get_order(next_order);

				if (control.position.current_row>=song->get_pattern(song->get_order(next_order))->get_length()) {

					control.position.current_row=0;
				}

       				control.position.force_next_order=next_order;

       			} else {
       				// no, probably the user deleted the orderlist.
       				control.play_mode=PLAY_NOTHING;
       				reset();
       			}

		} break;
		case 'D': {

			Uint8 inf ;
			//explicitslides=1;
			inf=control.channel[p_track].current_parameter;

			if (inf) control.channel[p_track].volume_slide_info=inf;
			else inf=control.channel[p_track].volume_slide_info;

			do_volume_slide(p_track,inf);

		} break;
		case 'E': {

		        Uint8 inf;

			inf=control.channel[p_track].current_parameter;
			do_pitch_slide_down(p_track,inf);

		} break;
		case 'F': {

		        Uint8 inf;

			inf=control.channel[p_track].current_parameter;
			do_pitch_slide_up(p_track,inf);

		} break;
		case 'G': {

			if (control.channel[p_track].current_parameter) {

				control.channel[p_track].portamento_speed=control.channel[p_track].current_parameter;
			}


			if ( control.channel[p_track].old_note<=120) {

				for (int i=0;i<Instrument::MAX_LAYERS;i++) {

					if (!control.channel[p_track].layers[i].period)
						continue;
					if ( (!control.ticks_counter) && (control.channel[p_track].layers[i].sample_changed) ){


						control.channel[p_track].layers[i].kick=KICK_NOTE;
						control.channel[p_track].sample_start_index=0;

					} else {

						control.channel[p_track].layers[i].kick=(control.channel[p_track].layers[i].kick==KICK_NOTE)?KICK_ENVELOPE:KICK_NOTHING;
						control.channel[p_track].kicking=false;

					}
				}

				do_pitch_slide_to_note(p_track);
				control.channel[p_track].has_own_period=true;
			}

		} break;
      		case 'H': {

			Uint8 dat;

			control.channel[p_track].doing_vibrato=true;

			dat=control.channel[p_track].current_parameter;

			if (!control.ticks_counter) {
				if (dat&0x0f) control.channel[p_track].vibrato_depth=dat&0xf;
				if (dat&0xf0) control.channel[p_track].vibrato_speed=(dat&0xf0)>>2;
			}

                        if (control.external_vibrato) break;

			//if (control.channel[p_track].period) {

			do_vibrato(p_track,false);
			control.channel[p_track].has_own_period=true;
			//}

		} break;
		case 'I': {

                        do_tremor(p_track);
			control.channel[p_track].has_own_volume=true;
		} break;
		case 'J': {

			do_arpegio(p_track);
		} break;
		case 'K': {

			Uint8 inf ;
			//explicitslides=1;
			inf=control.channel[p_track].current_parameter;

			control.channel[p_track].doing_vibrato=true;


			if (inf) control.channel[p_track].volume_slide_info=inf;
			else inf=control.channel[p_track].volume_slide_info;

			do_volume_slide(p_track,inf);

                        if (control.external_vibrato) break;

			//if (control.channel[p_track].period) {

				do_vibrato(p_track,false);
				control.channel[p_track].has_own_period=true;
			//}

		} break;
		case 'L': {


			Uint8 inf ;
			//explicitslides=1;
			inf=control.channel[p_track].current_parameter;

			if (inf) control.channel[p_track].volume_slide_info=inf;
			else inf=control.channel[p_track].volume_slide_info;

			do_volume_slide(p_track,inf);

			if ( control.channel[p_track].old_note<=120) {

				for (int i=0;i<Instrument::MAX_LAYERS;i++) {

					if (!control.channel[p_track].layers[i].period)
						continue;
					if ( (!control.ticks_counter) && (control.channel[p_track].layers[i].sample_changed) ){


						control.channel[p_track].layers[i].kick=KICK_NOTE;
						control.channel[p_track].sample_start_index=0;

					} else {

						control.channel[p_track].layers[i].kick=(control.channel[p_track].layers[i].kick==KICK_NOTE)?KICK_ENVELOPE:KICK_NOTHING;
						control.channel[p_track].kicking=false;
					}
				}

				do_pitch_slide_to_note(p_track);
				control.channel[p_track].has_own_period=true;
			}


		} break;
		case 'M': {
		                control.channel[p_track].channel_volume=control.channel[p_track].current_parameter;
                                if (control.channel[p_track].channel_volume>64) control.channel[p_track].channel_volume=64;
                                else if (control.channel[p_track].channel_volume<0) control.channel[p_track].channel_volume=0;
		} break;
		case 'N': {

			do_channel_volume_slide(p_track);
		}
		case 'O': {

			if (!control.ticks_counter) {

				if (control.channel[p_track].current_parameter) control.channel[p_track].lo_offset=(Uint16)control.channel[p_track].current_parameter<<8;
				control.channel[p_track].sample_start_index=control.channel[p_track].hi_offset|control.channel[p_track].lo_offset;

				//if ((control.channel[p_track].sample_ptr!=NULL)&&(control.channel[p_track].sample_start_index>control.channel[p_track].sample_ptr->data.size)) {
					//TODO, O effect
					//a->start=a->s->flags&(SF_LOOP|SF_BIDI)?a->s->loopstart:a->s->length;
                                //}
			}
		} break;
		case 'P': {

			do_pan_slide(p_track);
		} break;
		case 'Q': {
			do_retrig(p_track);

		} break;
		case 'R': {


			Uint8 dat;

			if (control.channel[p_track].current_parameter) {

				control.channel[p_track].tremolo_info=control.channel[p_track].current_parameter;
			}

			dat=control.channel[p_track].tremolo_info;

			if (!control.ticks_counter && dat) {

				if (dat&0x0f) control.channel[p_track].tremolo_depth=dat&0xf;
				if (dat&0xf0) control.channel[p_track].tremolo_speed=(dat&0xf0)>>2;
			}

			do_tremolo(p_track);
			control.channel[p_track].has_own_volume=true;

		} break;
		case 'S': {

			do_effect_S(p_track);
		} break;
		case 'T': {
                        Uint8 dat;
		        Sint16 temp=control.tempo;

			if (control.pattern_delay_2) return;

			if (control.channel[p_track].current_parameter) {

				control.channel[p_track].tempo_slide_info=control.channel[p_track].current_parameter;
			}

			dat=control.channel[p_track].tempo_slide_info;

			if (dat>=0x20) {

				if (control.ticks_counter) break;
				control.tempo=dat;
			} else {

				if (!control.ticks_counter) break;

				if (dat&0x10) {

					temp+=(dat&0x0f);
				} else {

			                temp-=dat;
				}
				control.tempo=(temp>255)?255:(temp<0x20?0x20:temp);
		        }

		} break;
		case 'U': {

			Uint8 dat;

			dat=control.channel[p_track].current_parameter;
			control.channel[p_track].doing_vibrato=true;
			if (!control.ticks_counter) {
				if (dat&0x0f) control.channel[p_track].vibrato_depth=dat&0xf;
				if (dat&0xf0) control.channel[p_track].vibrato_speed=(dat&0xf0)>>2;
			}

                        if (control.external_vibrato) break;

			//if (control.channel[p_track].period) {

				do_vibrato(p_track,true);
				control.channel[p_track].has_own_period=true;
			//}
		} break;
		case 'V': {

			control.global_volume=control.channel[p_track].current_parameter;
			if (control.global_volume>128) control.global_volume=128;
		} break;
		case 'W': {
                       do_global_volume_slide(p_track);
		} break;
		case 'X': {
			//sets both channel and current
			control.channel[p_track].channel_panning=control.channel[p_track].current_parameter;
			for (int i=0;i<Instrument::MAX_LAYERS;i++)
				control.channel[p_track].layers[i].panning=control.channel[p_track].current_parameter;
		} break;
		case 'Y': {

			Uint8 dat;

			if (control.channel[p_track].current_parameter) {

				control.channel[p_track].panbrello_info=control.channel[p_track].current_parameter;
			}

			dat=control.channel[p_track].panbrello_info;

			if (!control.ticks_counter) {

				if (dat&0x0f) control.channel[p_track].panbrello_depth=(dat&0xf);
				if (dat&0xf0) control.channel[p_track].panbrello_speed=(dat&0xf0)>>4;
			}

			//if (pf->panflag)
			do_panbrello(p_track);

		} break;
		case 'Z': {
			//I DO! cuttoff!
			Uint16 dat=control.channel[p_track].current_parameter;

			for (int i=0;i<Instrument::MAX_LAYERS;i++) {

				if (dat<0x80) {

					control.channel[p_track].layers[i].filter.it_cutoff=dat*2;
					if (control.channel[p_track].layers[i].filter.it_cutoff>0x80)
						control.channel[p_track].layers[i].filter.it_cutoff++;
				} else if (dat<0x90) {

					control.channel[p_track].layers[i].filter.it_reso=(dat-0x80)*0x10;
				} else {

					//control.channel[p_track].reverb_send=(dat-0x90)*255/0x6F;
				}
			}

		} break;

	}

}

void Player_Data::pre_process_effects() {


	int i;

        for (i=0;i<PATTERN_WIDTH;i++) {

		control.channel[i].has_own_period=false;
		control.channel[i].has_own_volume=false;
		control.channel[i].doing_vibrato=false;

		if (control.ticks_counter<control.speed) {

			run_effects(i);
			run_volume_column_effects(i);
		}

                /* continue volume slide if necessary for XM and IT */
                //if (pf->flags&UF_BGSLIDES) {
                 //       if (!explicitslides && a->sliding)
                 //               DoS3MVolSlide(0);
                 //       else if (a->tmpvolume) a->sliding=explicitslides;
                //}


		if (!control.channel[i].has_own_period)
			for (int j=0;j<Instrument::MAX_LAYERS;j++)
				control.channel[i].layers[j].period=control.channel[i].layers[j].aux_period;

		if (!control.channel[i].has_own_volume)
			for (int j=0;j<Instrument::MAX_LAYERS;j++)
				control.channel[i].layers[j].volume=control.channel[i].layers[j].aux_volume;


                for (int j=0;j<Instrument::MAX_LAYERS;j++) {

			if ((control.channel[i].layers[j].sample_ptr!=NULL) && (control.channel[i].layers[j].layer_ptr!=NULL)) {


				control.channel[i].layers[j].output_volume=
					(control.channel[i].layers[j].volume*control.channel[i].layers[j].sample_ptr->glb_volume*control.channel[i].layers[j].layer_ptr->volume.global_amount)/2048;
				control.channel[i].layers[j].output_volume=control.channel[i].layers[j].output_volume*control.channel[i].layers[j].random_volume_variation/100;


				if (control.channel[i].layers[j].output_volume>256) {

					control.channel[i].layers[j].output_volume=256;

				} else if (control.channel[i].layers[j].output_volume<0) {

					control.channel[i].layers[j].output_volume=0;
				}
			}
		}

        }
}
