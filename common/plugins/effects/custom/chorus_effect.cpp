//
// C++ Implementation: chorus_effect
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <cstring>
#include "chorus_effect.h"


#define MIX_CALCULATE_INCREMENT_INTERVAL 8


inline static double get_msecs_from_samples(double p_samples,double p_mixfreq) {

	return (p_samples/p_mixfreq)*1000.0;


}

inline static double get_samples_from_msecs(double p_msecs,double p_mixfreq) {

	return (p_msecs*p_mixfreq)/1000.0;

}

void Chorus_Effect::reset() {


	memset(&ringbuffer[0],0,ringbuffer.size()*sizeof(sample_t));
}


void Chorus_Effect::process_buffer(sample_t* p_buffer,int p_length) {


        unsigned aux_buffer_index=buffer_index;
	double aux_counter=counter;

	float dry_mult=params->get_param_float(PARAM_DRY);

	sample_t* ringbuffer_ptr=&ringbuffer[0];

	for (int j=0;j<(p_length*2);j+=2) {

		aux_buffer_index=(aux_buffer_index-2) & (RINGBUFFER_MASK<<1);

		ringbuffer_ptr[aux_buffer_index]=p_buffer[j];
		ringbuffer_ptr[aux_buffer_index+1]=p_buffer[j+1];

		p_buffer[j]*=dry_mult;
		p_buffer[j+1]*=dry_mult;
	}

	for (int i=0;i<params->get_param_int(PARAM_VOICES);i++) {

		sample_t src_l,src_r,aux,next_l,next_r;
		int todo=p_length;

		double divisor=(M_PI*2.0) / (1000.0/params->get_param_float(PARAM_SPEED));
		double increment=get_msecs_from_samples(1.0,get_sampling_rate());

		aux_counter=counter;
		double depth_multipler=get_samples_from_msecs((double)params->get_param_float(PARAM_DEPTH)/100.0,get_sampling_rate());
		double delay=get_samples_from_msecs(params->get_param_float(PARAM_DELAY),get_sampling_rate());

		float voice_width=params->get_param_float(PARAM_WIDTH);
		if (i%2)
			voice_width=-voice_width;

		float width=voice_width;
		float width_inv=1.0-(float)voice_width;
		width*=params->get_param_float(PARAM_WET);
		width_inv*=params->get_param_float(PARAM_WET);

		aux_buffer_index=buffer_index;


		for (int j=0;j<(p_length*2);j+=2) {

                        double src_idx=((double)i+1.0) * delay;
			src_idx+=depth_multipler+sin( aux_counter * divisor ) * depth_multipler;

			aux_counter+=increment;
			int src_idx_int=lrint(src_idx);
			float src_fixed=(double)(src_idx-(double)src_idx_int);

   			aux_buffer_index=(aux_buffer_index-2) & (RINGBUFFER_MASK<<1);

			src_idx_int=(aux_buffer_index + src_idx_int*2) & (RINGBUFFER_MASK<<1);

			src_l=ringbuffer_ptr[src_idx_int];
			src_r=ringbuffer_ptr[src_idx_int+1];

			src_idx_int=(src_idx_int+2)&(RINGBUFFER_MASK<<1);

			next_l=ringbuffer_ptr[src_idx_int];
			next_r=ringbuffer_ptr[src_idx_int+1];

			src_l=src_l+((next_l-src_l)*src_fixed);
			src_r=src_r+((next_r-src_r)*src_fixed);

			aux=src_r;

			src_r=src_r*width_inv +src_l*width;
			src_l=aux*width +src_l*width_inv;

			p_buffer[j]+=src_l;
			p_buffer[j+1]+=src_r;
		}

	}

	counter=aux_counter; //wont count if no voices.. that wont hurt anyway
	buffer_index=aux_buffer_index;

}


Chorus_Effect::Chorus_Effect(Effect::Parameters *p_params) : Effect(p_params) {

	params=dynamic_cast<Custom_FX_Param_List*>(p_params);
	
	if (params==NULL) {
		ERROR("This will CRASH HOW DID IT HAPPEN???!!!!!!!");
	}
	
	

	counter=0;
	buffer_index=0;
	ringbuffer.resize(RINGBUFFER_LEN*2);

}

Chorus_Effect::~Chorus_Effect() {

}


Effect::Parameters * Chorus_Effect::create_parameters() {
	
	Custom_FX_Param_List * params= new Custom_FX_Param_List("Stereo Chorus","INTERNAL: stereo_chorus");

	params->add_int_param("Voices","voices",1,5,3);
	params->add_float_param("delay(ms)","delay",0.4,500.0,18.0);
	params->add_float_param("LFO Speed(hz)","lfo_speed",0.001,50.0,0.5);
	params->add_float_param("LFO Depth(ms)","lfo_depth",0.0,200.0,2.0);
	params->add_float_param("Wet Level","wet_level",0.0,1.0,0.5);
	params->add_float_param("Dry Level","dry_level",0.0,1.0,0.9);
	params->add_float_param("Width","width",-1.0,1.0,0.5);
	
	return params;	
}
