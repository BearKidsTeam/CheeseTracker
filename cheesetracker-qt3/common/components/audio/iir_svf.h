//
// C++ Interface: iir_svf
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IIR_SVF_H
#define IIR_SVF_H

/**
@author Juan Linietsky

This is an IIR sort of state variable filter, except
that it's not very recommended to change it's state very much :)
It's written this way so the 3 types of filters can be implemented
using a small formula with 4 coheficits instead of the usual 6

*/

class IIR_SVF {
public:

	struct Coeffs {

		float a1,a2;
		float b0,b1,b2;

	};

	enum Mode {

                LOWPASS_HIGHRES, //lowpass, high resonance
		BANDPASS,
		HIGHPASS,
		LOWPASS,
		NOTCH
	};

private:


	float cutoff;
	float resonance;
	float sampling_rate;
	Mode mode;

	void prepare_coefficients_new(Coeffs *p_coeffs);

public:

	float get_response(float p_freq,Coeffs *p_coeffs);
	
	void set_mode(Mode p_mode);
	void set_cutoff(float p_cutoff);
	void set_resonance(float p_resonance);
	void set_sampling_rate(float p_srate);

	void prepare_coefficients(Coeffs *p_coeffs);

	IIR_SVF();

};

#endif
