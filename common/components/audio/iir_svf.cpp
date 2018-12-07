//
// C++ Implementation: iir_svf
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "iir_svf.h"
#include <math.h>
#include <stdio.h>


void IIR_SVF::set_mode(Mode p_mode) {

	mode = p_mode;
}
void IIR_SVF::set_cutoff(float p_cutoff) {

	cutoff=p_cutoff;
}
void IIR_SVF::set_resonance(float p_resonance) {

	resonance=p_resonance;
}


void IIR_SVF::set_sampling_rate(float p_srate) {

	sampling_rate=p_srate;
}


void IIR_SVF::prepare_coefficients(Coeffs *p_coeffs) {

	switch (mode) {

	case LOWPASS_HIGHRES: { ///this crazy highly resonant lowpass seems to be missed by some people, so i'm setting it back

	    //Just make sure filter doesnt go to heck

	    float Q=resonance;

	    if (Q>=1.0) Q=0.99;
            Q=1.0-Q;

	    double final_cutoff=(cutoff>=(sampling_rate/3))?(sampling_rate/3-1):cutoff;

	    final_cutoff= final_cutoff*((M_PI*2.0)/sampling_rate);
	    double amp = (1.0-Q) * final_cutoff;
	    if (amp > 2.0) amp = 2.0; //clip
	    amp = (Q - amp) / final_cutoff;
	    double e = 1.0 / (final_cutoff*final_cutoff);
	    double div=(1+amp+e);

	    p_coeffs->b0 = 1.0 / div;
	    p_coeffs->b1 = 0.0; //no need for this one
	    p_coeffs->b2 = 0.0; //no need for this one
	    p_coeffs->a1 = (amp+e*2)/div;
	    p_coeffs->a2 = (-e)/div;

	} break;
	default: {
	    prepare_coefficients_new(p_coeffs);
	} break;
	}
}


void IIR_SVF::prepare_coefficients_new(Coeffs *p_coeffs) {

	
	double final_cutoff=(cutoff>=(sampling_rate/4))?(sampling_rate/4-1):cutoff;
	
		
	double omega=2.0*M_PI*final_cutoff/sampling_rate;
	
	double sin_v=sin(omega);
	double cos_v=cos(omega);
	
	float Q=resonance;
	if (Q<=0.0) { 
		Q=0.0001;
	}
	
	if (Q>=1.0) {
		Q=1;
	}

        Q*=3;
	
	double alpha = sin_v/(2*Q);
	
	double a0 = 1.0 + alpha;
	
	switch (mode) {


		case LOWPASS: {	    
	
			p_coeffs->b0=  (1.0 - cos_v)/2.0 ;
			p_coeffs->b1=   1.0 - cos_v    ;
			p_coeffs->b2=  (1.0 - cos_v)/2.0 ;
			p_coeffs->a1=  -2.0*cos_v      ;
			p_coeffs->a2=   1.0 - alpha  ;
		} break;
	
	
		case HIGHPASS: {
	
			p_coeffs->b0 =  (1.0 + cos_v)/2.0;
			p_coeffs->b1 = -(1.0 + cos_v);
			p_coeffs->b2 =  (1.0 + cos_v)/2.0;
			p_coeffs->a1 =  -2.0*cos_v;
			p_coeffs->a2 =   1.0 - alpha;
		} break;
		
		case BANDPASS: {
	
			p_coeffs->b0 =  alpha*sqrt(Q+1);
			p_coeffs->b1 =  0.0 ;
			p_coeffs->b2 =  -alpha*sqrt(Q+1);
			p_coeffs->a1 =  -2.0*cos_v;
			p_coeffs->a2 =   1.0 - alpha;
		} break;
	
		case NOTCH: {
	
			p_coeffs->b0 =   1.0;
			p_coeffs->b1 =  -2.0*cos_v;
			p_coeffs->b2 =   1.0;
			p_coeffs->a1 =  -2.0*cos_v;
			p_coeffs->a2 =   1.0 - alpha;
		} break;
    
    };

    p_coeffs->b0/=a0;
    p_coeffs->b1/=a0;
    p_coeffs->b2/=a0;
    p_coeffs->a1/=0.0-a0;
    p_coeffs->a2/=0.0-a0;
    
}

//found this algorithm in zynaddsubfx


float IIR_SVF::get_response(float p_freq,Coeffs *p_coeffs) {
	
	float freq=p_freq / sampling_rate * M_PI * 2.0f;
	
	float cx=p_coeffs->b0,cy=0.0;

	cx += cos(freq) * p_coeffs->b1;
	cy -= sin(freq) * p_coeffs->b1;
	cx += cos(2*freq) * p_coeffs->b2;
	cy -= sin(2*freq) * p_coeffs->b2;
        
    
	float H=cx*cx+cy*cy;
	cx=1.0;
	cy=0.0;
    

	cx -= cos(freq) * p_coeffs->a1;
	cy += sin(freq) * p_coeffs->a1;
	cx -= cos(2*freq) * p_coeffs->a2;
	cy += sin(2*freq) * p_coeffs->a2;

    
	H=H/(cx*cx+cy*cy);
	return H;
}


IIR_SVF::IIR_SVF() {


	sampling_rate=44100;
	resonance=0.5;
	cutoff=5000;
	mode=LOWPASS;
}


