//
// C++ Implementation: effect_echo
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
#include "effect_echo.h"


void Effect_Echo::reset() {

	memset(&buffer[0],0,buffer.size()*sizeof(sample_t));
	buffer_pos=0;

}
inline static double get_samples_from_msecs(double p_msecs,double p_mixfreq) {

	return (p_msecs*p_mixfreq)/1000.0;

}
void Effect_Echo::check_buffer_length() {

	int desired_length=get_samples_from_msecs(parameters->get_param_float(PARAM_DELAY),get_sampling_rate()); //stereo
	wrap_size=(desired_length>=BUFFER_SIZE) ? BUFFER_SIZE : desired_length;
	wrap_size*=2;
}

void Effect_Echo::process_buffer(sample_t* p_buffer,int p_length) {

	check_buffer_length();

	int buflength=wrap_size;

	sample_t width=parameters->get_param_float(PARAM_SEPARATION);
	
	sample_t width_inv=1.0-parameters->get_param_float(PARAM_SEPARATION);
	sample_t aux;
	width*=parameters->get_param_float(PARAM_FEEDBACK);
	width_inv*=parameters->get_param_float(PARAM_FEEDBACK);

	sample_t *dst_buffer=&buffer[0];
	
        for (int i=0;i<p_length;i++) {

		sample_t val_l=dst_buffer[buffer_pos];
		sample_t val_r=dst_buffer[buffer_pos+1];
		sample_t buf_l=p_buffer[i*2];
		sample_t buf_r=p_buffer[i*2+1];
		
		p_buffer[i*2]+=val_l;
		p_buffer[i*2+1]+=val_r;

		val_l+=buf_l;
		val_r+=buf_r;

		aux=val_r;

		val_r=val_r*width_inv +val_l*width;
		val_l=aux*width +val_l*width_inv;

		dst_buffer[buffer_pos]=val_l;
		dst_buffer[buffer_pos+1]=val_r;

		buffer_pos+=2;
		buffer_pos%=buflength;
	}
}

Effect_Echo::Effect_Echo(Effect::Parameters *p_parameters) : Effect(p_parameters) {

	parameters=dynamic_cast<Custom_FX_Param_List*>(p_parameters);
	
	if (parameters==NULL) {
		ERROR("This will CRASH HOW DID IT HAPPEN???!!!!!!!");
	}

	buffer.resize(BUFFER_SIZE*2);
	check_buffer_length();
	buffer_pos=0;
	
}

Effect_Echo::~Effect_Echo() {

}

Effect::Parameters * Effect_Echo::create_parameters() {
	
	Custom_FX_Param_List * params= new Custom_FX_Param_List("Echo","INTERNAL: echo");
	params->add_float_param("Delay(ms)","delay",0.1,600.0,250.0);
	params->add_float_param("Feedback","feedback",0.0,1.0,0.4);
	params->add_float_param("Separation","separation",-0.5,1.0,0.4);
	
	return params;	
	
	
}

