//
// C++ Implementation: freeverb_effect
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "freeverb_effect.h"






void Freeverb_Effect::check_params_changed() {

	if (
		(current_parm.room_size==bridge_parm.room_size) &&
		(current_parm.damp==bridge_parm.damp) &&
		(current_parm.wet==bridge_parm.wet) &&
		(current_parm.width==bridge_parm.width) &&
		(current_parm.dry==bridge_parm.dry) )
			return;


	current_parm=bridge_parm;


	float room=current_parm.room_size;
//	room*=(float)current_parm.old_freq/44100.0f; //make it independent to sampling rate? This just helps a bit i think
	freeverb.setroomsize(room);
        freeverb.setdamp((float)current_parm.damp);
	freeverb.setwet((float)current_parm.wet);
	freeverb.setdry((float)current_parm.dry);
	freeverb.setwidth((float)current_parm.width);

//	freeverb.mute();

}
void Freeverb_Effect::reset() {

	freeverb.mute();
}

void Freeverb_Effect::process_buffer(sample_t* p_buffer,int p_length) {

	bridge_parm.room_size=parameters->get_param_float(PARAM_ROOM_SIZE);
	bridge_parm.damp=parameters->get_param_float(PARAM_DAMP);
	bridge_parm.wet=parameters->get_param_float(PARAM_WET);
	bridge_parm.dry=parameters->get_param_float(PARAM_DRY);
	bridge_parm.width=parameters->get_param_float(PARAM_WIDTH);
	
	check_params_changed();
        float soft=0.65f;
	sample_t* buffer=p_buffer;

	while (p_length) {

		int amount=(p_length>INTERNAL_BUFFER_SIZE)?INTERNAL_BUFFER_SIZE:p_length;

		float *buf_in_r_ptr=&buffer_in_r[0];
		float *buf_in_l_ptr=&buffer_in_l[0];
		float *buf_out_r_ptr=&buffer_out_r[0];
		float *buf_out_l_ptr=&buffer_out_l[0];
		sample_t* read_ptr=buffer;
		for (int i=0;i<amount;i++) {

			buf_in_l_ptr[i]=soft * *(read_ptr++);
			buf_in_r_ptr[i]=soft * *(read_ptr++);
		}

		freeverb.processreplace(buf_in_l_ptr, buf_in_r_ptr,buf_out_l_ptr, buf_out_r_ptr,amount,1);

		for (int i=0;i<amount;i++) {

			*buffer++ =buf_out_l_ptr[i];
			*buffer++ =buf_out_r_ptr[i];
		}
		p_length-=amount;
	}
}



Freeverb_Effect::Freeverb_Effect(Effect::Parameters *p_parameters) : Effect(p_parameters) {

	
	parameters=dynamic_cast<Custom_FX_Param_List*>(p_parameters);
	
	if (parameters==NULL) {
		ERROR("This will CRASH HOW DID IT HAPPEN???!!!!!!!");
	}
	
        buffer_in_r.resize(INTERNAL_BUFFER_SIZE);
        buffer_in_l.resize(INTERNAL_BUFFER_SIZE);
        buffer_out_r.resize(INTERNAL_BUFFER_SIZE);
        buffer_out_l.resize(INTERNAL_BUFFER_SIZE);

	current_parm.room_size=-1;
	current_parm.damp=-1;
	current_parm.wet=-1;
	current_parm.dry=-1;
	current_parm.width=-1;
	
	freeverb.setmode(0);
}




Freeverb_Effect::~Freeverb_Effect() {


}

Effect::Parameters * Freeverb_Effect::create_parameters() {
	
	
	Custom_FX_Param_List * params= new Custom_FX_Param_List("Reverb","INTERNAL: freeverb");
	params->add_float_param("Room Size","room_size",0.0,1.0,0.8);
	params->add_float_param("Damping","damping",0.0,1.0,0.5);
	params->add_float_param("Wet Level","wet",0.0,1.0,0.3);
	params->add_float_param("Dry Level","dry",0.0,1.0,0.9);
	params->add_float_param("Width","width",0.0,1.0,0.5);
	
	return params;	
}


