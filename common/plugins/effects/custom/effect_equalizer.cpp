//
// C++ Implementation: effect_equalizer
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "effect_equalizer.h"


/* Value ranges and cohefficient computation algorithms taken from eq-xmms */

const char*  bands_6_names          [] = { "32", "100", "320", "1k", "3k" , "10k" };
const double bands_6_freqs             [] = {  32 ,  100 ,  320 ,  1e3,  3200,  10e3 };

const char*  bands_10_names          [] = { "32" , "63", "125", "250", "500", "1k", "2k", "4k", "8k", "16k" };
const double bands_10_freqs             [] = { 31.25, 62.5,  125 ,  250 ,  500 ,  1e3,  2e3,  4e3,  8e3,  16e3 };

const char*  bands_21_names          [] = { "22", "32", "45", "63", "90", "125", "175", "250", "350", "500", "700", "1k", "1.4k", "2k", "2.8k", "4k", "5.6k", "8k", "11k", "16k", "22k" };
const double bands_21_freqs             [] = {  22 ,  32 ,  44 ,  63 ,  90 ,  125 ,  175 ,  250 ,  350 ,  500 ,  700 ,  1e3,  1400 ,  2e3,  2800 ,  4e3,  5600 ,  8e3,  11e3,  16e3,  22e3 };

const char*  bands_31_names          [] = { "20", "25", "32" , "40", "50", "63", "80", "100", "125", "160", "200", "250", "315", "400", "500", "630", "800", "1k", "1.25k", "1.6k", "2k", "2.5k", "3.2k", "4k", "5k", "6.3k", "8k", "10k", "12.5k", "16k", "20k" };
const double bands_31_freqs             [] = {  20,   25,   31.5,  40 ,  50 ,  63 ,  80 ,  100 ,  125 ,  160 ,  200 ,  250 ,  315 ,  400 ,  500 ,  630 ,  800 , 1e3 ,  1250  ,  1600 ,  2e3,  2500 ,  3150 ,  4e3,  5e3,  6300 ,  8e3,  10e3,  12500 ,  16e3,  20e3 };






Effect_Equalizer::Band::Band() {

	c1=c2=c3=0;
	ht_l.a1=ht_l.a2=ht_l.a3=0;
	ht_l.b1=ht_l.b2=ht_l.b3;
	ht_r.a1=ht_r.a2=ht_r.a3=0;
	ht_r.b1=ht_r.b2=ht_r.b3;
}

static int solve_quadratic(double a,double b,double c,double *r1, double *r2) {

    double base=2*a;
    if (base == 0.0f)
	return 0;

    double squared=b*b-4*a*c;
    if (squared<0.0)
	return 0;

    squared=sqrt(squared);

    *r1=(-b+squared)/base;
    *r2=(-b-squared)/base;

    if (*r1==*r2)
	return 1;
    else
	return 2;
}


void Effect_Equalizer::reset() {

	int band_count;
	float octave_size;
	const char * const * names_ptr;
	const double * freqs_ptr;

	switch (eq_type) {

		case BANDS_6: {

			names_ptr=bands_6_names;
			freqs_ptr=bands_6_freqs;
			band_count=6;
			octave_size=1.0;
		} break;
		case BANDS_10: {

			names_ptr=bands_10_names;
			freqs_ptr=bands_10_freqs;
			band_count=10;
			octave_size=1.0;
		} break;
		case BANDS_21: {
			names_ptr=bands_21_names;
			freqs_ptr=bands_21_freqs;
			band_count=21;
			octave_size=1.0/3.0;
		} break;
		case BANDS_31: {
			names_ptr=bands_31_names;
			freqs_ptr=bands_31_freqs;
			band_count=31;
			octave_size=1.0/3.0;
		} break;
	}


        for (int i=0;i<bands.size();i++) {

		double frq_l,frq,fac;

		frq=freqs_ptr[i];
		frq_l=round(frq/pow(2.0,octave_size/2.0));

#define POW2(m_t) (m_t*m_t)


		double side_gain2=POW2(1.0/M_SQRT2);
                double th=2.0*M_PI*frq/mix_freq;
                double th_l=2.0*M_PI*frq_l/mix_freq;

		double c2a=side_gain2 * POW2(cos(th))
		- 2.0 * side_gain2 * cos(th_l) * cos(th)
		+ side_gain2
		- POW2(sin(th_l));

		double c2b=2.0 * side_gain2 * POW2(cos(th_l))
		+ side_gain2 * POW2(cos(th))
		- 2.0 * side_gain2 * cos(th_l) * cos(th)
		- side_gain2
		+ POW2(sin(th_l));

		double c2c=0.25 * side_gain2 * POW2(cos(th))
		- 0.5 * side_gain2 * cos(th_l) * cos(th)
		+ 0.25 * side_gain2
                - 0.25 * POW2(sin(th_l));

		//printf("band %i, precoefs = %f,%f,%f\n",i,c2a,c2b,c2c);

		double r1,r2; //roots
        	int roots=solve_quadratic(c2a,c2b,c2c,&r1,&r2);

		if (roots==0) {
			ERROR("no roots! WTF?");
			continue;
		}

		bands[i]->c1=2.0 * ((0.5-r1)/2.0);
		bands[i]->c2=2.0 * r1;
		bands[i]->c3=2.0 * (0.5+r1) * cos(th);
		//printf("band %i, coefs = %f,%f,%f\n",i,(float)bands[i]->c1,(float)bands[i]->c2,(float)bands[i]->c3);

		bands[i]->ht_l.a1=bands[i]->ht_l.a2=bands[i]->ht_l.a3=0;
		bands[i]->ht_l.b1=bands[i]->ht_l.b2=bands[i]->ht_l.b3=0;
		bands[i]->ht_r.a1=bands[i]->ht_r.a2=bands[i]->ht_r.a3=0;
		bands[i]->ht_r.b1=bands[i]->ht_r.b2=bands[i]->ht_r.b3=0;
	}
}

void Effect_Equalizer::process_buffer(sample_t* p_buffer,int p_length){

	if (mix_freq!=get_sampling_rate()) {

		mix_freq=get_sampling_rate();
		reset();
	}

#define INTERNAL_BUFF_SIZE 8192

	static sample_t buf_in_l[INTERNAL_BUFF_SIZE];
	static sample_t buf_in_r[INTERNAL_BUFF_SIZE];
	static sample_t buf_out_l[INTERNAL_BUFF_SIZE];
	static sample_t buf_out_r[INTERNAL_BUFF_SIZE];

	while (p_length>INTERNAL_BUFF_SIZE) {

		process_buffer(p_buffer,INTERNAL_BUFF_SIZE);
		p_length-=INTERNAL_BUFF_SIZE;
		p_buffer+=INTERNAL_BUFF_SIZE*2;
	}


	for (int i=0;i<p_length;i++) {

		buf_in_l[i]=p_buffer[i*2];
		buf_in_r[i]=p_buffer[i*2+1];
		buf_out_l[i]=buf_in_l[i];
		buf_out_r[i]=buf_in_r[i];
	}

	for (int i=0;i<bands.size();i++) {

		sample_t *buf=p_buffer;
		int count=p_length;
		
		Band& band=*bands[i];
		float level=parameters->get_param_float(i);

                for (int j=0;j<p_length;j++) {

			band.ht_l.a1=buf_in_l[j];
			band.ht_l.b1=	band.c1 * ( band.ht_l.a1 - band.ht_l.a3 )
					+ band.c3 * band.ht_l.b2
					- band.c2 * band.ht_l.b3;
			buf_out_l[j] += band.ht_l.b1 * level;

			band.ht_r.a1=buf_in_r[j];
			band.ht_r.b1=	band.c1 * ( band.ht_r.a1 - band.ht_r.a3 )
					+ band.c3 * band.ht_r.b2
					- band.c2 * band.ht_r.b3;
			buf_out_r[j] += band.ht_r.b1 * level;

			//could optimize this..
			band.ht_l.a3=band.ht_l.a2;
			band.ht_l.a2=band.ht_l.a1;
			band.ht_l.b3=band.ht_l.b2;
			band.ht_l.b2=band.ht_l.b1;
			band.ht_r.a3=band.ht_r.a2;
			band.ht_r.a2=band.ht_r.a1;
			band.ht_r.b3=band.ht_r.b2;
			band.ht_r.b2=band.ht_r.b1;
		}
	}

	for (int i=0;i<p_length;i++) {

		p_buffer[i*2]=buf_out_l[i];
		p_buffer[i*2+1]=buf_out_r[i];
	}
}


Effect_Equalizer::Effect_Equalizer(Effect::Parameters *p_parameters) : Effect(p_parameters) {

	
	
	parameters=dynamic_cast<Custom_FX_Param_List*>(p_parameters);
	
	if (parameters==NULL) {
		ERROR("This will CRASH HOW DID IT HAPPEN???!!!!!!!");
		return;
	}
	

	bands.resize( parameters->get_param_count() );
	
	switch ( bands.size() ) {
		
		case 6:eq_type=BANDS_6; break;
		case 10:eq_type=BANDS_10; break;
		case 21:eq_type=BANDS_21; break;
		case 31:eq_type=BANDS_31; break;
	}

	for (int i=0;i<bands.size();i++) 
		bands[i]=new Band;		
	mix_freq=-1;
}
        

Effect::Parameters * Effect_Equalizer::create_parameters(Band_Count p_count) {
	

	Band_Count eq_type=p_count;
        int bandcount;
	const char * const * names_ptr;

	switch (eq_type) {
		case BANDS_6: {
			names_ptr=bands_6_names;
			bandcount=6;
		} break;
		case BANDS_10: {
			names_ptr=bands_10_names;
			bandcount=10;
		} break;
		case BANDS_21: {
			names_ptr=bands_21_names;
			bandcount=21;
		} break;
		case BANDS_31: {
			names_ptr=bands_31_names;
			bandcount=31;
		} break;
	}

	
	Custom_FX_Param_List * params= new Custom_FX_Param_List(itostr(bandcount) + " Bands EQ",(string)"INTERNAL: eq_" +itostr(bandcount)+"_bands");
	
	
	for (int i=0;i<bandcount;i++) {

		params->add_float_param((string)"Band "+names_ptr[i]+(string)"hz",(string)"band_"+names_ptr[i],-1.0,2.0,0.0);		
	}
	
	return params;
		
}


Effect_Equalizer::~Effect_Equalizer() {

	for (int i=0;i<bands.size();i++) {

		delete bands[i];
	}
}



