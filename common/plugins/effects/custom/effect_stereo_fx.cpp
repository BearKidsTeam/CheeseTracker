//
// C++ Implementation: effect_stereo_fx
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <cstring>
#include "effect_stereo_fx.h"


inline static double get_samples_from_msecs(double p_msecs,double p_mixfreq) {

	return (p_msecs*p_mixfreq)/1000.0;

}
void Effect_Stereo_FX::check_buffer_length() {

	//int desired_length=get_samples_from_msecs(parameters->get_param_float(PARAM_FEEDBACK_DELAY_TIME),get_sampling_rate()); //stereo
	//wrap_size=(desired_length>=BUFFER_SIZE) ? BUFFER_SIZE : desired_length;
	wrap_size=buffer.size();
}

void Effect_Stereo_FX::reset() {

	memset(&buffer[0],0,buffer.size()*sizeof(sample_t));
	buffer_pos=0;
		
}

void Effect_Stereo_FX::process_buffer(sample_t* p_buffer,int p_length) {
	

    	sample_t *dst_buffer=p_buffer;
	
	float intensity=parameters->get_param_float(PARAM_DRY_SEPARATION);
	float control_gain=parameters->get_param_float(PARAM_DRY_COMPENSATION);
	float level=parameters->get_param_float(PARAM_DRY_LEVEL);
	
	
	bool stereo=parameters->get_param_bool(PARAM_FEEDBACK_INPUT_STEREO);
	int input =parameters->get_param_int(PARAM_FEEDBACK_INPUT_TYPE);
	
	int delay_samples=get_samples_from_msecs( parameters->get_param_float(PARAM_FEEDBACK_DELAY_TIME) , get_sampling_rate() );
	float feedback_level = parameters->get_param_float( PARAM_FEEDBACK_LEVEL );
        int separation_samples= get_samples_from_msecs( parameters->get_param_float(PARAM_FEEDBACK_DELAY_SEPARATION) , get_sampling_rate() );

	//ugly formula for nice gain control		
	float gain_control=(intensity<1.0f)?1.0f:((1.0f+intensity)/2.0f);
	gain_control=(1.0f-control_gain)+control_gain*gain_control;
	
	for (int i=0;i<p_length;i++) {
	
		float val_l=dst_buffer[i*2];
		float val_r=dst_buffer[i*2+1];
		
		
		buffer[buffer_pos]= stereo ? (val_l-val_r) : (val_l + val_r);
		buffer[buffer_pos]/=2;
			
		
	
		float center=(val_l+val_r)/2.0f;
	
		val_l=( center+(val_l-center)*intensity ) / gain_control;
		val_r=( center+(val_r-center)*intensity ) / gain_control;
	
		if (val_l>1.0f) val_l=1.0f;
		else if (val_l<-1.0f) val_l=-1.0f;
	
		if (val_r>1.0f) val_r=1.0f;
		else if (val_r<-1.0f) val_r=-1.0f;
	
		dst_buffer[i*2]=val_l*level;
		dst_buffer[i*2+1]=val_r*level;

		switch (input) {

		case 1: { // surround
			int ofs=((buffer_pos+wrap_size)-delay_samples) % wrap_size;
			sample_t val=buffer[ofs]*feedback_level;
			
			dst_buffer[i*2]+=val;
			dst_buffer[i*2+1]+=-val;
		} break;
		case 2: { // flange

			int ofs_l=((buffer_pos+wrap_size)-delay_samples) % wrap_size;
			int ofs_r=((buffer_pos+wrap_size)-(delay_samples+separation_samples)) % wrap_size;

			dst_buffer[i*2]+=buffer[ofs_l]*feedback_level;
			dst_buffer[i*2+1]+=buffer[ofs_r]*feedback_level;;

		} break;
		}
		
		
		buffer_pos=(buffer_pos+1)%wrap_size;
	}
}


Effect::Parameters * Effect_Stereo_FX::create_parameters() {
	
    Custom_FX_Param_List * params= new Custom_FX_Param_List("Stereo FX","INTERNAL: stereo_fx");
    static const char * feedback_modes [3]={"None","Surround","Stereo Flanger"};

	
	params->add_float_param("Dry Separation","dry_separation",0,6,1.2);
	params->add_float_param("Dry Compensation","dry_compensation",0.0,1.0,1.0);
	params->add_float_param("Dry Level","dry_level",0.0,1.0,1.0);
	params->add_options_param("Feedback Mode:","feedback_mode",feedback_modes,3,0);
	params->add_bool_param("Stereo (L-R)","stereo_feedback",false);

	params->add_float_param("Feedback Level","feedback_level",0.0,1.0,0.8);
	params->add_float_param("Feedback Delay","feedback_delay",0.01,200,1);
	params->add_float_param("Feedback Delay Separation","feedback_delay_separation",0.0,200,5);
	
	return params;
}


//Effect_Stereo_FX::Effect_Stereo_FX() {
	
	
	
//}

Effect_Stereo_FX::Effect_Stereo_FX(Effect::Parameters *p_parameters) :Effect(p_parameters){

	parameters=dynamic_cast<Custom_FX_Param_List*>(p_parameters);
	
	if (parameters==NULL) {
		ERROR("This will CRASH HOW DID IT HAPPEN???!!!!!!!");
	}

	buffer.resize(BUFFER_SIZE);
	check_buffer_length();
	buffer_pos=0;	
	reset();
		
}

