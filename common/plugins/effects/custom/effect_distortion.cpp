//
// C++ Implementation: effect_distortion
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "effect_distortion.h"

inline static double get_samples_from_msecs(double p_msecs,double p_mixfreq) {

	return (p_msecs*p_mixfreq)/1000.0;
}


  void Effect_Distortion::reset() {


}


void Effect_Distortion::process_buffer(sample_t* p_buffer,int p_length) {

	float drive=params->get_param_float(PARAM_DRIVE);
	int type=params->get_param_int(PARAM_TYPE);

	for (int i=0;i<p_length;i++) {

		float in_l=p_buffer[i*2];
		float in_r=p_buffer[i*2+1];
		
		float out_l,out_r;
		
		switch (type) {
			
			
			case POWER: {

				float mult=drive*drive*drive*20.0+0.0001;
				out_l= (mult*in_l-pow(mult*in_l,3.0)*3.0)/mult;
				out_r= (mult*in_r-pow(mult*in_r,3.0)*3.0)/mult;
				
			} break;
			case ATAN: {
				
				float mult=pow(10,drive*drive*3.0)-1.0+0.001;
				float div=1.0+drive*8;
				out_l=atan(in_l*mult)/atan(mult);
				out_r=atan(in_r*mult)/atan(mult);
				out_l/=div;
				out_r/=div;
			} break;
			case LIMIT: {
				
				float limitval=1.0-drive;
						
				if (in_l>0)
					out_l = (in_l>limitval) ? limitval+(in_l-limitval)/8 : in_l;
				else if (in_l<0)
					out_l = (in_l<-limitval) ? -limitval-(in_l+limitval)/8 : in_l;
				
				if (in_r>0)
					out_r = (in_r>limitval) ? limitval+(in_r-limitval)/8 : in_r;
				else if (in_r<0)
					out_r = (in_r<-limitval) ? -limitval-(in_r+limitval)/8 : in_r;
				
			} break;
			case ILIMIT: {
				
				float limitval=1.0-drive;
						
				if (in_l>0)
					out_l = (in_l>limitval) ? limitval-(in_l-limitval) : in_l;
				else if (in_l<0)
					out_l = (in_l<-limitval) ? -limitval+(in_l+limitval) : in_l;
				
				if (in_r>0)
					out_r = (in_r>limitval) ? limitval-(in_r-limitval) : in_r;
				else if (in_r<0)
					out_r = (in_r<-limitval) ? -limitval+(in_r+limitval) : in_r;
				
			} break;
			case DIFF: { //kind of a highpass if you think of it, but it's nice
				
				out_l=(in_l-old_l)*drive + in_l*(1.0-drive);
				out_r=(in_r-old_r)*drive + in_l*(1.0-drive);
				
			} break;
			case DECIMATOR: { //kind of a highpass if you think of it, but it's nice
				
#define DECIMATE2(m_out,m_in) \
{                              				\
	float sign=(m_in>0) ? -1 : 1;			\
	int int_val=lrintf(fabs(m_in)*65535.0);         \
	int modparts=lrintf(1.0+(1.0-drive)*256.0);	\
	int module=65535/modparts;			\
	int out=int_val - int_val % module;		\
	m_out=(float)out / 65535.0 * sign;		\
}

				DECIMATE2(out_l,in_l);
				DECIMATE2(out_r,in_r);
#undef DECIMATE2
			} break;
			
			default: {
				
				out_l=0;
				out_r=0;				
			} break;
		};

		p_buffer[i*2] = out_l;
		p_buffer[i*2+1] = out_r;
		
		old_l=in_l;
		old_r=in_r;
	}

}


Effect_Distortion::Effect_Distortion(Effect::Parameters *p_params) : Effect(p_params) {

	params=dynamic_cast<Custom_FX_Param_List*>(p_params);
	
	if (params==NULL) {
		ERROR("This will CRASH HOW DID IT HAPPEN???!!!!!!!");
	}

	old_r=old_l=0;	
}


Effect_Distortion::~Effect_Distortion() {
}


Effect::Parameters * Effect_Distortion::create_parameters() {
	
    Custom_FX_Param_List * params= new Custom_FX_Param_List("Distortion","INTERNAL: distortion");
    static const char* distortion_types[]={"Power","ArcTan","Limiter","Inv. Limiter","Difference","Decimator"};


    params->add_float_param("Ovedrive","overdrive",0.0,1.0,0.0);
    params->add_options_param("Type","type",distortion_types,MAX_DISTORTIONS,1);

    return params;
}
