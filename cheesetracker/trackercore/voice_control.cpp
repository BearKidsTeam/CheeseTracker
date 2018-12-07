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

#include <cstdlib>
#include "voice_control.h"
#include "freq_conversion.h"


#define PAN_SURROUND 512
#define PAN_RIGHT    255
#define PAN_LEFT    0
#define PAN_CENTER    128


Uint8 Voice_Control::auto_vibrato_table[128]={
	 0, 1, 3, 4, 6, 7, 9,10,12,14,15,17,18,20,21,23,
	24,25,27,28,30,31,32,34,35,36,38,39,40,41,42,44,
	45,46,47,48,49,50,51,52,53,54,54,55,56,57,57,58,
	59,59,60,60,61,61,62,62,62,63,63,63,63,63,63,63,
	64,63,63,63,63,63,63,63,62,62,62,61,61,60,60,59,
	59,58,57,57,56,55,54,54,53,52,51,50,49,48,47,46,
	45,44,42,41,40,39,38,36,35,34,32,31,30,28,27,25,
	24,23,21,20,18,17,15,14,12,10, 9, 7, 6, 4, 3, 1
};

void Voice_Control::update_info_from_master_channel() {

	Channel_Control::Layer *ctrl_layer=&master_channel.ptr->layers[master_channel.layer];

	layer.ptr=ctrl_layer->layer_ptr;
	sample.ptr=ctrl_layer->sample_ptr;


	note.current=ctrl_layer->note;
	volume.output=ctrl_layer->output_volume;

	volume.channel=master_channel.ptr->channel_volume;

	panning.current=ctrl_layer->panning;

	note.kick=ctrl_layer->kick;

	note.end_flags=ctrl_layer->note_end_flags;
	sample.period=ctrl_layer->period;
	sample.index=ctrl_layer->sample_index;

	volume.envelope_ctrl.active=ctrl_layer->volume_envelope_on;
	panning.envelope_ctrl.active=ctrl_layer->panning_envelope_on;
	pitch.envelope_ctrl.active=ctrl_layer->pitch_envelope_on;


	layer.NNA_type=ctrl_layer->NNA_type;
	effect_send.reverb=ctrl_layer->reverb_send;
	effect_send.chorus=ctrl_layer->chorus_send;

//	last_note_type=master_channel.ptr->last_note_type;

	sample.offset=master_channel.ptr->sample_start_index;
	filter=ctrl_layer->filter;

}
void Voice_Control::start_envelope(Envelope *p_envelope,Envelope_Control *p_envelope_ctrl) {

	p_envelope_ctrl->pos_index=0;
	p_envelope_ctrl->status=1;
	p_envelope_ctrl->sustain_looping=p_envelope->is_sustain_loop_enabled();
	p_envelope_ctrl->looping=p_envelope->is_loop_enabled();
	p_envelope_ctrl->terminated=false;
	p_envelope_ctrl->kill=false;
	p_envelope_ctrl->value=(int)p_envelope->get_height_at_pos(p_envelope_ctrl->pos_index);
}

void Voice_Control::process_envelope(Envelope *p_envelope,Envelope_Control *p_envelope_ctrl) {

	if (p_envelope_ctrl->active) {

		if (note.end_flags&END_NOTE_OFF) p_envelope_ctrl->sustain_looping=false;

		p_envelope_ctrl->value=int(p_envelope->get_height_at_pos(p_envelope_ctrl->pos_index));

                if (p_envelope_ctrl->value==Envelope::NO_POINT) {

			////printf("no point here dang
		}

		if (p_envelope_ctrl->value < p_envelope->get_min_value())
			p_envelope_ctrl->value=p_envelope->get_min_value();

		if (p_envelope_ctrl->value > p_envelope->get_max_value())
			p_envelope_ctrl->value=p_envelope->get_max_value();



		p_envelope_ctrl->pos_index++;

		if (p_envelope_ctrl->sustain_looping) {

			if (p_envelope_ctrl->pos_index > p_envelope->get_node_offset(p_envelope->get_sustain_loop_end())) {

				p_envelope_ctrl->pos_index=p_envelope->get_node_offset(p_envelope->get_sustain_loop_begin());
			}

		} else if (p_envelope_ctrl->looping) {

			if (p_envelope_ctrl->pos_index > p_envelope->get_node_offset(p_envelope->get_loop_end())) {

				p_envelope_ctrl->pos_index=p_envelope->get_node_offset(p_envelope->get_loop_begin());
			}

		}

		int last_node=p_envelope->get_node_count()-1;
		if (p_envelope_ctrl->pos_index > p_envelope->get_node_offset(last_node)) {

			p_envelope_ctrl->terminated=true;
			p_envelope_ctrl->pos_index=p_envelope->get_node_offset(last_node);
			if (int(p_envelope->get_node_height(last_node))==0) p_envelope_ctrl->kill=true;
		}

	} else {

		if (p_envelope->get_min_value()<0) { //hack, dirty one

			p_envelope_ctrl->value=0;
		} else {

			p_envelope_ctrl->value=p_envelope->get_max_value();
		}

	}
}

/******** superimportant part :) *************************/

bool Voice_Control::process() {


	if (master_channel.ptr->layers[master_channel.layer].slave_voice==this) {

		if (!first_processing) {

			//updating info from the master channel
			update_info_from_master_channel();
		} else {

			first_processing=false;

		}
	}
	//only update info form master channel

	int tmp_mixer_period;
	Sint32 tmp_vibrato_value,tmp_vibrato_depth,tmp_volenv_value;
	Uint64 tmpvol; // 64bits should be the only way to avoid getting notes raped out

	int filter_env=-1;

	//if voice doesnt have a sample set or size is 0.. forget it


        if ( !(note.kick==KICK_NOTE) && !mixing.voice.is_active() ) {

		return true;

	}

	if ( sudden_end_requested || mixing.voice.has_stopped() || (sample.ptr==NULL) || (sample.ptr->data.get_size()==0)) {

//		//printf("HEHE RETURNING TRUE BECAUSE stopped %i, sample ptr %p data size %i\n",mixing.voice.has_stopped() ,sample.ptr, sample.ptr->data.size);
		return true;
	}


	if (volume.envelope_ctrl.active && volume.envelope_ctrl.kill) {

	 	return true; //volume envelope is dead, no more sound
	}
	//TODO set limits somewhere else

	if (sample.period<40) {

		sample.period=40;

	} else if (sample.period>50000) {

		sample.period=50000;
	}


	if ((note.kick==KICK_NOTE)||(note.kick==KICK_NOTEOFF)) {

		int real_start_index;

		if (sample.offset==-1) {

			real_start_index=0;

		} else {

			real_start_index=sample.offset;
		}
		//???
		mixing.voice.setup_sample(&sample.ptr->data,real_start_index);
		volume.fadeout=512;
		pitch.auto_vibrato_sweep_pos=0;



	}


	/* Start Envelopes */
	if ( ((note.kick==KICK_NOTE)||(note.kick==KICK_ENVELOPE))) {

		start_envelope(&layer.ptr->volume.envelope,&volume.envelope_ctrl);
		start_envelope(&layer.ptr->panning.envelope,&panning.envelope_ctrl);
		start_envelope(&layer.ptr->pitch.envelope,&pitch.envelope_ctrl);
	}

	note.kick=KICK_NOTHING;


	process_envelope(&layer.ptr->volume.envelope,&volume.envelope_ctrl);
	process_envelope(&layer.ptr->panning.envelope,&panning.envelope_ctrl);
	process_envelope(&layer.ptr->pitch.envelope,&pitch.envelope_ctrl);

	if (volume.envelope_ctrl.terminated) {

		if (volume.envelope_ctrl.kill) {

			volume.fadeout=0;
		} else {

			note.end_flags|=END_NOTE_FADE;
		}
	}




	tmp_volenv_value=volume.envelope_ctrl.value;

	tmpvol=(Uint64)volume.fadeout;    /* max 512 */
	tmpvol*=(Uint64)volume.channel;    /* * max 64 */
	tmpvol*=(Uint64)volume.output;     /* * max 256 */
	volume.total=(tmpvol/0x4000)<<4; /* totalvolume used to determine samplevolume */
	tmpvol*=(Uint64)volume.mixing; /* max 128 */
	tmpvol*=(Uint64)volume.global; /* max 128 */
	tmpvol*=(Uint64)tmp_volenv_value; /* max 64 */
//	tmpvol>>=34;//0x200000;
	tmpvol>>=28;//0x200000;


	if (master_channel.ptr->mute) {

		mixing.voice.set_volume(0);
	} else {

		mixing.voice.set_volume(tmpvol);
//if (volume.fadeout>0) control.voices_used++;
	}


	if (!panning.mix_stereo) {

		mixing.voice.set_panning(PAN_CENTER);
	} else if (panning.current==PAN_SURROUND) {

		mixing.voice.set_panning(PAN_SURROUND);

	} else {

		int newpan,real_modifier;

		real_modifier=(panning.envelope_ctrl.value*(PAN_CENTER-abs(panning.current-PAN_CENTER)))/32;

		newpan=panning.current+real_modifier;

		newpan=(newpan<PAN_LEFT)?PAN_LEFT:(newpan>PAN_RIGHT)?PAN_RIGHT:newpan;

		mixing.voice.set_panning(newpan);
	}

	/* VIBRATO, i think this is buggy as heck */

	if ( (sample.period>0) && (sample.ptr->vibrato_depth>0) ) {

		switch (sample.ptr->vibrato_type) {
			case 0:
				tmp_vibrato_value=auto_vibrato_table[pitch.auto_vibrato_pos&127];
				if (pitch.auto_vibrato_pos & 0x80) tmp_vibrato_value=-tmp_vibrato_value;
				break;
			case 1:
				tmp_vibrato_value=64;
				if (pitch.auto_vibrato_pos & 0x80) tmp_vibrato_value=-tmp_vibrato_value;
				break;
			case 2:
				tmp_vibrato_value=63-(((pitch.auto_vibrato_pos+128)&255)>>1);
				break;
			default:
				tmp_vibrato_value=(((pitch.auto_vibrato_pos+128)&255)>>1)-64;
				break;
		}
	} else {

		tmp_vibrato_value=0;
	}


	if ((pitch.auto_vibrato_sweep_pos>>8)<sample.ptr->vibrato_depth) {

		pitch.auto_vibrato_sweep_pos+=sample.ptr->vibrato_speed; //FIXME - speed? i think so
		tmp_vibrato_depth=pitch.auto_vibrato_sweep_pos;

	} else {

		tmp_vibrato_depth=sample.ptr->vibrato_depth<<8;
	}

	tmp_vibrato_value=(tmp_vibrato_value*tmp_vibrato_depth)>>16;




	// Linear WHAT?

	sample.period-=tmp_vibrato_value;

	/* update vibrato position */
	pitch.auto_vibrato_pos=(pitch.auto_vibrato_pos+sample.ptr->vibrato_rate)&0xff;

	/* external vibrato */

	/* process pitch envelope */
	tmp_mixer_period=sample.period;

	if (pitch.envelope_ctrl.active) {

		long aux_pitch_diff;
		int pe_value=pitch.envelope_ctrl.value;

		if (!layer.ptr->pitch.use_as_filter) {

			if (((Uint16)note.current<<1)+pe_value<=0) pe_value=-(note.current<<1);

			aux_pitch_diff=get_period(((Uint16)note.current<<1)+pe_value,0,pitch.linear_slides)-get_period(((Uint16)note.current<<1),0,pitch.linear_slides);

			if ( ((int)tmp_mixer_period-aux_pitch_diff)<0 ) aux_pitch_diff=0;

			tmp_mixer_period+=aux_pitch_diff;
		} else {

			filter_env=pe_value+32; //max 64


		}
	}

	if (volume.fadeout==0) { /* check for a dead note (fadevol=0) */

		return true;

	} else {
		Uint64 freq=get_frequency(tmp_mixer_period,pitch.linear_slides);
		Uint64 tracker_c5=Tables::get_linear_frequency(Tables::get_linear_period(60<<1,0));
		Uint64 sample_c5=sample.ptr->data.get_c5_freq();
		freq=freq*sample_c5/tracker_c5;
		////printf("C-5 Constant is %i\n",Tables::get_linear_frequency(Tables::get_linear_period(60<<1,0)));

		mixing.voice.set_frequency((Uint32)freq);

		/* if keyfade, start substracting fadeoutspeed from fadevol: */
		if ((note.end_flags & END_NOTE_FADE)) {

			if (volume.fadeout>=(layer.ptr->volume.fadeout)) {

				volume.fadeout-=(layer.ptr->volume.fadeout);
			} else {

				volume.fadeout=0;
			}
		}

		/*FILTARSSSSSSSS*/



		filter.envelope_cutoff=filter_env;
		filter.type=layer.ptr->pitch.filter_type;

		filter.recalculate_coheficients(mixer->get_mix_frequency());

		if ((filter.final_cutoff<0xFF) && (use_filter)) {

			mixing.voice.set_filter(filter.coeffs,true);
		} else {

			mixing.voice.set_filter(filter.coeffs,false);
		}

		/* RAIVERV */

		//mixing.voice.set_global_effect_send(Mixer_Base::DEFAULT_EFFECT_REVERB,effect_send.reverb);

		/* CHAURUZ */

		//mixing.voice.set_global_effect_send(Mixer_Base::DEFAULT_EFFECT_REVERB,effect_send.chorus);

	}

	return false;

}

void Voice_Control::deinitialize(bool use_mutex) {

	if (master_channel.ptr->layers[master_channel.layer].slave_voice==this) {

		master_channel.ptr->layers[master_channel.layer].slave_voice=NULL;
	}
	mixer->eliminate_voice(&mixing.voice, use_mutex);
}

void Voice_Control::reset() {

	mixer=NULL;
	memset(&effect_send,0,sizeof(effect_send));
	memset(&master_channel,0,sizeof(master_channel));
	memset(&sample,0,sizeof(sample));
	memset(&layer,0,sizeof(layer));
	memset(&volume,0,sizeof(volume));
	memset(&pitch,0,sizeof(pitch));
	memset(&panning,0,sizeof(panning));
	memset(&note,0,sizeof(note));
	mixing.voice_registered=false;
	first_processing=true;
	use_filter=false;
}

bool Voice_Control::init(Mixer_Base *p_mixer) {

	mixer=p_mixer;
	mixer->allocate_voice(&mixing.voice);
	return true;
}

Voice_Control::Voice_Control() {

	sudden_end_requested=false;
	mixing.voice_registered=false;
	reset();
}

Voice_Control::~Voice_Control()
{
}
