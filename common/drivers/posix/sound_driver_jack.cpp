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
                          sound_driver_jack.cpp  -  description
                             -------------------
    begin                : Sun Jan 28 2001
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

#include "sound_driver_jack.h"
#include "components/audio/sample_conversion.h"



#ifdef JACK_ENABLED

//#include <jack/cycles.h>



void Sound_Driver_JACK::flush() {
	
	/* if (blocking_mode)
		write(sound_fd,audiobuffer,current_buffer_size);  */
}


int Sound_Driver_JACK::process_jack_callback(jack_nframes_t nframes,void *arg) {

	unsigned int i;

	Sound_Driver_JACK *sdj = (Sound_Driver_JACK*) arg;
	sdj->jack_callback_nframes = nframes;
	sdj->callback.emit();
	return 0;
}

Sound_Driver::Status Sound_Driver_JACK::process() {

	if (!active)
		return DISABLED;
	if (!get_mixer_list() || !get_mixer_list()->size())
		return IDLE;

	int nframes = jack_callback_nframes;

	jack_default_audio_sample_t *outL = (jack_default_audio_sample_t *) jack_port_get_buffer(output_port_l, nframes);
	jack_default_audio_sample_t *outR = (jack_default_audio_sample_t *) jack_port_get_buffer(output_port_r, nframes);
	
	for (int j=0; j<nframes; j++) {
		outL[j] = outR[j] = 0.0;
	}

	for (int i=0; i<get_mixer_list()->size(); i++) {

		Mixer_Base *mixer=(*get_mixer_list())[i];
		mixer->set_mix_frequency(real_mixfreq);

		int samples_todo = nframes;
		int samples_mixed;
		while(samples_todo > 0) {
			samples_mixed=mixer->write_samples(samples_todo);
			////printf("b4:%i\t", outL);
			convert_deinterleaved_float_stereo((sample_32f_t*)outL,(sample_32f_t*)outR, mixer->get_mixdown_buffer_ptr(),samples_mixed);
			outL += samples_mixed;
			outR += samples_mixed;
			////printf("after:%i\t", outL);
			samples_todo -= samples_mixed;
		}
		if (samples_todo != 0)
			printf ("hmm, too many samples mixed!");

		////printf("i:%i  samples_mixed:%i\t mixfreq:%i", i, samples_mixed, mix_frequency);

		/*
		convert_32s_stereo_to_float_stereo_doubled((sample_32f_t*)outL, 
												   (sample_32f_t*)outR,
												   mixer->get_mixdown_buffer_ptr(),
												   nframes);
		*/
	}
	////printf("\n");

	////printf("Sound_Driver_JACK::process()\t");
	return ACTIVE;
}


int Sound_Driver_JACK::jack_set_mixfreq(jack_nframes_t nframes,void *arg) {

	Sound_Driver_JACK *instance = (Sound_Driver_JACK*)arg;
	instance->set_ext_mixfreq(nframes);
	return 0;
}
int Sound_Driver_JACK::jack_set_buffsize(jack_nframes_t nframes,void *arg) {

	Sound_Driver_JACK *instance = (Sound_Driver_JACK*)arg;
	instance->set_ext_mixbuffsize(nframes);
	return 0;
}
void Sound_Driver_JACK::jack_shutdown(void *arg) {

	Sound_Driver_JACK *instance = (Sound_Driver_JACK*)arg;
	instance->set_ext_jackdied();
}
void Sound_Driver_JACK::set_ext_mixfreq(int p_mixfreq) {

	real_mixfreq=p_mixfreq;

}
void Sound_Driver_JACK::set_ext_mixbuffsize(int p_mixbuffsize) {

	real_buffersize=p_mixbuffsize;
}
void Sound_Driver_JACK::set_ext_process(int p_samples) {

	process_amount=p_samples;
	callback.emit();
}
void Sound_Driver_JACK::set_ext_jackdied() {

	active=false;
	callback.emit(); //so the parent knows
}


int Sound_Driver_JACK::init() {

	real_buffersize = BUFFERSIZE;
	//(void*)audiobuffer = calloc(1, buffersize);

//	Mixer_Base *mixer=(*get_mixer_list())[0];

	/* try to become a client of the JACK server */
	if (active) return FUNCTION_FAILURE; //nothing to do here

	//printf("JACK: Creating client...\n");
	if ((client = jack_client_new (client_name.c_str())) == 0) {
		ERROR("JACK server doesn't seem to be running...");
		return FUNCTION_FAILURE;
	}

	/* tell the JACK server to call `process_jack_callback()' whenever
	   there is work to be done.
	*/
	//printf("JACK: Setup Callbacks...\n");

	/* Sound_Driver_JACK::process is the JACK callback */
	jack_set_process_callback (client, process_jack_callback, this);

	/* tell the JACK server to call `bufsize()' whenever
	   the maximum number of frames that will be passed
	   to `process()' changes
	*/

	jack_set_buffer_size_callback (client, jack_set_buffsize, this);

	/* tell the JACK server to call `srate()' whenever
	   the sample rate of the system changes.
	*/


	jack_set_sample_rate_callback (client, jack_set_mixfreq, this);

	/* tell the JACK server to call `jack_shutdown()' if
	   it ever shuts down, either entirely, or if it
	   just decides to stop calling us.
	*/

	jack_on_shutdown (client, jack_shutdown, this);

	/* display the current sample rate. once the client is activated
	   (see below), you should rely on your own sample rate
	   callback (see above) for this value.
	*/
	//printf("JACK: Get Data...\n");

	real_mixfreq=jack_get_sample_rate (client);

	printf ("engine sample rate: %lu\n", jack_get_sample_rate (client));

	/* create ports */

	//printf("JACK: Register Ports...\n");

	output_port_l = jack_port_register (client, "L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	output_port_r = jack_port_register (client, "R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	/* tell the JACK server that we are ready to roll */

	if (jack_activate (client)) {
		ERROR("cannot activate client");
		jack_client_close (client);
		return FUNCTION_FAILURE;
	}

	/* connect the ports. Note: you can't do this before
	   the client is activated, because we can't allow
	   connections to be made to clients that aren't
	   running.
	*/

	//printf("JACK: Connect Ports...\n");

	if (jack_connect (client, jack_port_name (output_port_l), "alsa_pcm:playback_1")) {
		ERROR("cannot connect output ports\n");
		//jack_client_close (client);
		//return FUNCTION_FAILURE;
	}
	if (jack_connect (client, jack_port_name (output_port_r), "alsa_pcm:playback_2")) {
		ERROR("cannot connect output ports\n");
		//jack_client_close (client);
		//return FUNCTION_FAILURE;
	}

	//printf("JACK: DONE!\n");

	jack_active=true;
	active=true;

	//mixer->set_mix_stereo(true);
	//mixer->set_mix_16bits(true);

	return FUNCTION_SUCCESS;
}

bool Sound_Driver_JACK::finish() {



	if (jack_active) {

		//printf("JACK: Disconnecting...\n");

		jack_port_unregister(client,output_port_l);
		jack_port_unregister(client,output_port_r);
		jack_client_close (client);

	}
	
	jack_active=false;		
        active=false;
	return true;
}


bool Sound_Driver_JACK::update() {

//	cycles_t aux_cycles;

	if (!active) return false;
	mixer->set_mix_frequency(real_mixfreq);
	
	jack_default_audio_sample_t *out_l = (jack_default_audio_sample_t *) jack_port_get_buffer (output_port_l,process_amount);
	jack_default_audio_sample_t *out_r = (jack_default_audio_sample_t *) jack_port_get_buffer (output_port_r,process_amount);
	
	
	int write_count=0;
	//mixer->set_dont_mix_back(true); // dont mix for ux
	
 	while (write_count<process_amount) {

		Sint32 *sample;
		int processed;
		float *auxf;

		//processed=mixer->write_bytes(NULL,process_amount-write_count);
		
		write_count+=processed;

		sample=0; //mixer->get_mix_buf_ptr();
		while (processed--) {

			*out_l++ = (float)*(sample++)/(float)0x7FFFFFFF;
			*out_r++ = (float)*(sample++)/(float)0x7FFFFFFF;
		}
  	}
	return true;
}

int Sound_Driver_JACK::get_mix_frequency() {

 	return real_mixfreq;
}
bool Sound_Driver_JACK::get_mix_stereo() {

 	return true;
}

bool Sound_Driver_JACK::get_mix_16bits() {

	return true;

}
int Sound_Driver_JACK::get_mix_buffer_size() {

	return mix_buffersize;
}

void Sound_Driver_JACK::set_client_name(string p_name) {
	
	client_name=p_name;	
}

Sound_Driver_JACK::Sound_Driver_JACK(){

	active=false;
	jack_active=false;
	process_mode=CALLBACK;
	mix_frequency=44100;
	mix_16bits=true;
	mix_stereo=true;
	mix_buffersize=4096; //jack sets all this anyway
	real_mixfreq=mix_frequency; //just so it works at the begining
	
	client_name="CheeseTronic";
}
Sound_Driver_JACK::~Sound_Driver_JACK(){

}

#endif

