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
                          mixer.cpp  -  description
                             -------------------
    begin                : Sun Jan 27 2002
    copyright            : (C) 2002 by Juan Linietsky
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

#include <cstring>
#include "mixer.h"
#include "math.h"

#include "cycles.h"
#include "components/midi/midi_controls.h"

#define CYCMSEC(m_parm) (float)( ((double)m_parm/950000000.0) * 1000.0 )

static cycles_t cyc,cycfin,cyctot;

static float history[10];





Mixer::Buffer::Buffer() {

	last_used_in_frame=used_in_frame=false;
	output_list.clear();
	Output new_out;
	new_out.send_level=127;
	new_out.send_control=-1;
	new_out.buffer_index=MIXDOWN_BUFFER_INDEX;
	output_list.push_back(new_out);
	buffer.resize(MIX_BUFFER_SIZE*2);
	idle_count=0;
	controls=NULL;
	
	effect_chain_parameters = new Effect_Parameter_Chain;
	effect_chain = new Effect_Chain;
	effect_chain->set_chain_params(effect_chain_parameters);
	patch_effect_chain = new Effect_Chain;
}


Mixer::Buffer::~Buffer() {
	
	delete effect_chain;
	delete patch_effect_chain;
	delete effect_chain_parameters;
}


void Mixer::set_channel_midi_controls(int p_chan,MIDI_Controls *p_controls) {
	
	if (p_chan<0 || p_chan>=channel.size())
		return;
	
	channel[p_chan].controls=p_controls;
	
}






 /*****************************/
int Mixer::get_channel_count() {

	return channel.size();
}



void Mixer::set_channel_patch_effect_chain(int p_chan,Effect_Parameter_Chain *p_chain) {
	
	if (p_chan<0 || p_chan>=channel.size())
		return;
	
	channel[p_chan].patch_effect_chain->set_chain_params(p_chain);	
	
}

Effect_Parameter_Chain * Mixer::get_channel_effect_parameter_chain(int p_chan) {

	if (p_chan<0 || p_chan>=channel.size())
		return NULL;
	return channel[p_chan].effect_chain_parameters;
}

static inline float get_level(sample_t* p_src, int p_amount) { //return level from 0 to 256

	sample_t val,max=0;
	//not as slow as it might look like, gcc optimizes this kind of sructure really well
	p_amount*=2;
	while (p_amount--) {

		val=fabs( *(p_src++) ); //avoids from wrapping
		if (val>max)
			max=val;
	}

	max;

	return max;
}

static inline void add_samples(sample_t* p_dst, sample_t *p_src, int p_amount) {

	//not as slow as it might look like, gcc optimizes this kind of sructure really well
	while (p_amount--) {

         	*p_dst++ += *p_src; //Left
          	p_src++;
         	*p_dst++ += *p_src; //Right
          	p_src++;
	}
}

static inline void add_samples(sample_t* p_dst, sample_t *p_src, int p_amount, float p_send_level /* 0 - 1 */) {


	//not as slow as it might look like, gcc optimizes this kind of sructure really well
	
	while (p_amount--) {

         	*p_dst++ += *p_src * p_send_level; //Left
          	p_src++;
         	*p_dst++ += *p_src * p_send_level; //Right
          	p_src++;
	}
}

int Mixer::write_samples(int p_samples) {

 	int buffer_size=MIX_BUFFER_SIZE/2;

	cyctot=0;

	//first dont let it mix more than what we can
	if (p_samples>buffer_size) p_samples=buffer_size;


	//then just dont bother if no callback interval, this makes things faster.
	if (config.process_callback_interval==0) {

		return write_samples_internal(p_samples);
	};

	//process using interval callback
	int mixed;

	if (interval_remaining<p_samples) {

		p_samples=interval_remaining;
	}

	mixed=write_samples_internal(p_samples);
	interval_remaining-=mixed;

	if (interval_remaining<=0) {

		if (interval_remaining<0) {

			ERROR("Some problem here!");
		}

		process_callback.emit(); //processing callback
		interval_remaining=config.process_callback_interval;
	}

	for (int i=0;i<9;i++) {

		history[i]=history[i+1];
	}
	history[9]=CYCMSEC(cyctot);

        float avg=0;

	for (int i=0;i<9;i++) {

		avg+=history[i];
	}

	avg/=10;
//	//printf("PARSE %f cycles\n",avg);

	return mixed;
}


void Mixer::process_buffer_effects_and_outputs(Buffer* p_buffer,int p_samples) {


	//check idleness
	double mix_msecs=((double)p_samples/(double)config.mix_freq)*1000.0;

	if ( (p_buffer->idle_count-mix_msecs) > (double)SILENCE_SHUTDOWN_THRESHOLD)
		return; //pointless to mix, ignoring the rest and save cpu

	if (!p_buffer->used_in_frame) {
		p_buffer->used_in_frame=true;
		memset(&p_buffer->buffer[0],0,p_samples*sizeof(sample_t)*2);
	}
	//apply per patch effects
	p_buffer->patch_effect_chain->process_buffer(&p_buffer->buffer[0],p_samples);
	//apply global effects
	p_buffer->effect_chain->process_buffer(&p_buffer->buffer[0],p_samples);

	if (!p_buffer->used_in_frame) {
		//no voices or ringbuffer happened, check wether the effects or other buffers did anything
		float level=get_level(&p_buffer->buffer[0],p_samples);
		if (level>config.buffer_level_mute_threshold) {
			p_buffer->idle_count=0;
		} 
	}

	for (int i=0;i<p_buffer->output_list.size();i++) {

		int mixbuffer_idx=p_buffer->output_list[i].buffer_index;

	
		int send_level;
		if ( (p_buffer->output_list[i].send_control!=-1) &&
		      (p_buffer->controls) &&
		      (p_buffer->controls->get_control(p_buffer->output_list[i].send_control)!=MIDI_Controls::CONTROL_USE_DEFAULT_VALUE)
		    )				 
		        send_level=p_buffer->controls->get_control(p_buffer->output_list[i].send_control);
		else 
			send_level=p_buffer->output_list[i].send_level;
		
        	if (mixbuffer_idx==MIXDOWN_BUFFER_INDEX) {

			

			if (send_level>126) //just add, faster
				add_samples(&mixdown_buffer[0],&p_buffer->buffer[0],p_samples);
			else
				add_samples(&mixdown_buffer[0],&p_buffer->buffer[0],p_samples,(float)send_level/127.0);
		} else {

			if ((mixbuffer_idx<0) || (mixbuffer_idx>=buffer.size())) {
				ERROR("Something wrong mixing this buffer!");
				continue; //try this output

			} else {
				if (!buffer[mixbuffer_idx].used_in_frame) {

					memset(&buffer[mixbuffer_idx].buffer[0],0,p_samples*sizeof(sample_t)*2);
					buffer[mixbuffer_idx].used_in_frame=true;
					buffer[mixbuffer_idx].idle_count=0;
				}
       			}

			if (send_level>126) //just add, faster
				add_samples(&buffer[mixbuffer_idx].buffer[0],&p_buffer->buffer[0],p_samples);
			else
				add_samples(&buffer[mixbuffer_idx].buffer[0],&p_buffer->buffer[0],p_samples,(float)send_level/127.0);
		}
	}
}

/* This writes samples to the stereo buffers */
int Mixer::write_samples_internal(int p_samples) {


	/* Remember We mix stereo! */
 	int buffer_size=MIX_BUFFER_SIZE/2;
        int aux_active_voices=0;
	int aux_active_buffers=0;

        /* If what we are requested to mix is less than the buffer size (most probably :) */
        /* Just mix what we are requested! */
	if (p_samples<buffer_size) buffer_size=p_samples;


	//clear buffer, we cannot avoid this, so let's hope malloc does a better job at this
 	memset(&mixdown_buffer[0],0,buffer_size*sizeof(sample_t)*2);

  	/* Init the channels to the default 'unused in this mix' state */
   	for (int i=0;i<config.channels_used;i++) {

		channel[i].used_in_frame=false; //no channel used by default
		channel[i].idle_count+=((double)p_samples/(float)config.mix_freq)*1000.0;
    	}
   	for (int i=0;i<buffer.size();i++) {

		buffer[i].used_in_frame=false; //no buffer used by default
		buffer[i].idle_count+=((double)p_samples/(float)config.mix_freq)*1000.0;
    	}


        /* Iterate throught the voices used */
	for(int i=0;i<voice.size();i++) {

         	//if this voice doesnt exist (NULL) then we dont bother
         	if (voice[i]==NULL) continue;
		if (!voice[i]->can_mix()) {
			voice[i]->mix(buffer_size,&thrash_buffer[0]); //it needs to know how much time did it pass anyway
			continue;
		}

          	int chan; //check chan

           	chan=voice[i]->get_channel(); //channel to mix
            	if (chan>=config.channels_used) continue; //channel invalid

    		voice[i]->set_preamp(preamp); //pre volume amplification before mixing
	      	voice[i]->set_mix_frequency(config.mix_freq); //in case mixfreq changed, notify voice


            	if (!channel[chan].used_in_frame) {
			/* To avoid zeroing all channels before mixing (cpu usage)
			   we only do it if it's going to be used
			*/

			//we zero it
			memset(&channel[chan].buffer[0],0,buffer_size*sizeof(sample_t)*2);
   			channel[chan].used_in_frame=true; //and we set it as "used in frame"
			channel[chan].idle_count=0;

             	}

		/* Finally, mix the voice to its corresponding channel buffer */
		cyc=get_cycles();
		voice[i]->mix(buffer_size,&channel[chan].buffer[0]);
	        cyctot+=get_cycles()-cyc;


		/* After we finished mixing, let's check what's the voice up to. */

  		aux_active_voices++; //otherwise, must sum to the active voices count
  	}

	for(int i=0;i<voice.size();i++) {
        	if (voice[i]==NULL) continue;

		if (voice[i]->get_priority()==Voice::DEAD) {

			/* The voice is in dead status, it must be removed */
			//printf("HOHO mixer killing this voice %i 'cause it's ded, sanca\n",i);
			eliminate_voice(i);

			continue; //let's go to next voice
		}
	}

	////printf("%i active voices\n",aux_active_voices);
        //This mixes back the voices into the main buffer and effect buffers... only if they were used

   	for (int i=0;i<config.channels_used;i++) {


		if (channel[i].ring_buffer.is_there_data_to_mix()) {

	            	if (!channel[i].used_in_frame) {
				//we zero it
				memset(&channel[i].buffer[0],0,buffer_size*sizeof(sample_t)*2);
   				channel[i].used_in_frame=true; //and we set it as "used in frame"
				channel[i].idle_count=0;
			}

			int samples_to_mix=channel[i].ring_buffer.get_data_remaining()/2;
			////printf("there is data to mix babee, but samples to mix are %i\n",channel[i].ring_buffer.get_data_remaining());

			if (samples_to_mix>buffer_size) {
				samples_to_mix=buffer_size;
			}

			samples_to_mix*=2; //back to individual samples

			channel[i].ring_buffer.mix_to_buffer(samples_to_mix,&channel[i].buffer[0]);
		}


		process_buffer_effects_and_outputs(&channel[i],buffer_size);

		if (channel[i].used_in_frame)
			aux_active_buffers++;
    		/* update mixer history */
    		channel[i].last_used_in_frame=channel[i].used_in_frame;
    	}

   	for (int i=(buffer.size()-1);i>=0;i--) {

		process_buffer_effects_and_outputs(&buffer[i],buffer_size);
		if (buffer[i].used_in_frame)
			aux_active_buffers++;
	}
     	/* Update active voices count */
   	active_voices=aux_active_voices;
	active_buffers=aux_active_buffers;

	/* Return, saying how many samples we did mix */
	return buffer_size;
}

/* notification when the voice is killed, we NULL the slot */
void Mixer::on_voice_kill(int p_index) {

	voice[p_index]=NULL;
}


/* This is the algorithm in charge of finding free voices */
int Mixer::find_aviable_voice() {

        int i;

	/* First check for unused slots */
        
	for(i=0;i<voice.size();i++) {

 		if (voice[i]==NULL) return i;
   	}

    	/* Then simply find the voice with less priority, we leave the voice
           in charge of definining it's priority */

	int voice_with_less_priority_index=0;
	int min_voice_priority=Voice::CRITICAL+1;
           
	for(i=0;i<voice.size();i++) {

		int voice_priority=voice[i]->get_priority();

		if (voice_priority<min_voice_priority) {

			min_voice_priority=voice_priority;
			voice_with_less_priority_index=i;
		}
	}

	/* Finally get rid of it */

	eliminate_voice(voice_with_less_priority_index);

	return voice_with_less_priority_index;
}

void Mixer::allocate_voice(Voice* p_new_voice) {

	voice[find_aviable_voice()]=p_new_voice;
	p_new_voice->set_mix_frequency(config.mix_freq);
}



int Mixer::get_voice_index(Voice* p_voice) {


	for(int i=0;i<voice.size();i++) {

		if (voice[i]==p_voice) {
			return i;
		}
	}

	return VOICE_NOT_FOUND;

}

void Mixer::replace_voice(Voice* p_old_voice,Voice* p_new_voice) {


	int voice_index=get_voice_index(p_old_voice);

	if (voice_index==VOICE_NOT_FOUND) {

		ERROR("Tryin to replace unexisting voice!");
		return;
	}

	eliminate_voice(voice_index); //adios, voice

	voice[voice_index]=p_new_voice;

}

/* wrapper version  :)*/
void Mixer::eliminate_voice(Voice* p_voice, bool use_mutex) {

	int voice_index=get_voice_index(p_voice);

	if (voice_index==VOICE_NOT_FOUND) {

		ERROR("Tryin to eliminate unexisting voice!");
		return;
	}

	eliminate_voice(voice_index, use_mutex);
}

/* real version :) */

void Mixer::eliminate_voice(int p_index, bool use_mutex) {


	//zero declick voice
	if (voice[p_index]->can_mix()) {

		memset(&declick_buffer[0],0,sizeof(sample_t)*declick_buffer.size());
		voice[p_index]->mix(declick_buffer.size()/2,&declick_buffer[0], use_mutex);

		for (int i=0;i<declick_buffer.size();i++) {

			float multiplier;
			multiplier=(((float)declick_buffer.size()-(float)i)-1.0)/(float)declick_buffer.size();

			sample_t samp;
			samp=declick_buffer[i] * multiplier;
			declick_buffer[i]=samp;


		}
		channel[voice[p_index]->get_channel()].ring_buffer.mix_from_buffer(declick_buffer.size(),&declick_buffer[0]);
	}

	////printf("there is %i data to mix babee, but samples to mix are %i\n",declick_buffer.size(),channel[voice[p_index]->get_channel()].ring_buffer.get_data_remaining());

	voice[p_index]->remove_from_mixer();
	voice[p_index]=NULL;

}

void Mixer::set_preamp(float p_preamp) {

	preamp=p_preamp;
}

void Mixer::set_process_callback_interval(int p_interval) {

 	config.process_callback_interval=p_interval;
	interval_remaining=p_interval;
}

Mixer::Mixer()  {

	/* Config Mixer */
	config.mix_freq=0;


	/* Init Voices */
        config.voices_used=128;
        voice.resize(config.voices_used);
	for (int i=0;i<voice.size();i++) {

         	voice[i]=NULL;
 	}

  	/* Init channels*/
  	config.channels_used=16;
  	channel.resize(config.channels_used);
	for (int i=0;i<channel.size();i++) {

		channel[i].name="Channel " + itostr(i);
	}

   	/* Init buffers */
	mixdown_buffer.resize(MIX_BUFFER_SIZE*2);
	thrash_buffer.resize(MIX_BUFFER_SIZE*2);

	/* Set preamp to default */
  	preamp=0.3;
	config.process_callback_interval=0;
	interval_remaining=5000000; //just in case
	declick_buffer.resize(DECLICK_BUFFER_SIZE*2);
	config.buffer_level_mute_threshold=0.001; //hope not too much?

}


Mixer::~Mixer(){
}
