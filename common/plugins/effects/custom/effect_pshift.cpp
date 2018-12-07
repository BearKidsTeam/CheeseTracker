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
#include "effect_pshift.h"
#include "math.h"


void Effect_PShift::reset() {

	memset(&buffer[0],0,buffer.size()*sizeof(sample_t));
	buffer_pos=0;

}
inline static double get_samples_from_msecs(double p_msecs,double p_mixfreq) {

	return (p_msecs*p_mixfreq)/1000.0;

}
void Effect_PShift::check_buffer_length() {

        wrap_size=buffer.size();
}


#define STONES_2_RATE(m_stones) pow(2.0,(m_stones)/12.0)

/*
static inline void resample_one_double_chunk_interleave(
				double& p_ofs,double &p_ofs_to,double &p_left_to_trans,
					 const int& p_sample_counter,const int& p_cur_bsize,
                                         const float& p_rate,float& p_fade_threshold
				  ){


		double size=(double)p_cur_bsize;
		double counter=(double)(p_sample_counter % p_cur_bsize);


                int parts=(int)p_rate+1);
                double partsize=size/(double)parts;

		int part=counter/partsize;
                double part_chunk_size=

                float part_offset=(size-partsize)/(double)part;





		if (counter>(size/2.0)) {


		    p_ofs=(size-counter)*p_rate;
		    p_left_to_trans=size-counter;
		    p_ofs_to=p_ofs-(size-p_rate*size);


		} else {



		}


                p_ofs-=(size-counter);
                p_ofs_to-=(size-counter);


}

*/

static inline void resample_one_chunk_interleave(
				double& p_ofs,double &p_ofs_to,double &p_left_to_trans,
					 const int& p_sample_counter,const int& p_cur_bsize,
                                         const float& p_rate,float& p_fade_threshold
				  ){


		double size=(double)p_cur_bsize;
		double counter=(double)(p_sample_counter % p_cur_bsize);


		p_ofs=(size-counter)*p_rate;
		p_left_to_trans=size-counter;
                p_ofs_to=p_ofs-(size-p_rate*size);

                p_ofs-=(size-counter);
                p_ofs_to-=(size-counter);


}

static inline void resample_one_pingpong(
				double& p_ofs,double &p_ofs_to,double &p_left_to_trans,
					 const int& p_sample_counter,const int& p_cur_bsize,
                                         const float& p_rate,float& p_fade_threshold
				  ){


		double ofs_bw; // backwards reading pos
		double ofs_fw; //forward reading pos

                double rev_factor=(2.0*p_rate-2.0)/(4.0*p_rate); //magic equation for obtaining this constant
		double size=(double)p_cur_bsize;


                double bw_size=rev_factor*size;
		double counter=(double)(p_sample_counter % p_cur_bsize);

		ofs_bw=size+counter*p_rate;
                ofs_fw=(size-counter)*p_rate;

		if (counter<bw_size) {
		    //go back
			p_ofs=ofs_bw;
			p_ofs_to=ofs_fw; //when backwards fade to forward
			p_left_to_trans=(size*rev_factor)-counter;
			if (bw_size<p_fade_threshold)
                            p_fade_threshold=bw_size;
		} else {
		    //catch up
		    p_ofs=ofs_fw;
		    p_left_to_trans=size-counter;

		    if (p_rate<1) {

                        p_ofs_to=p_ofs-(size-p_rate*size);

		    } else {
			p_ofs_to=-ofs_fw;
			if (p_rate<2) {

			   // p_fade_threshold*=p_rate-1.0;
			}
			if ((size-bw_size)<p_fade_threshold)
			    p_fade_threshold=size-bw_size;
		    }
		}
		//adjust relative

                p_ofs-=(size-counter);
                p_ofs_to-=(size-counter);

}


void Effect_PShift::process_buffer(sample_t* p_buffer,int p_length) {

	check_buffer_length();


	float rate=parameters->get_param_float(PARAM_SHIFT);
	int bsize=(int)get_samples_from_msecs(parameters->get_param_float(PARAM_BLOCKSIZE), get_sampling_rate());
	float trans_threshold=get_samples_from_msecs(parameters->get_param_float(2),get_sampling_rate()); //bleh
        float TD_thresh_dB=parameters->get_param_float(3); //bleh
        int TD_max_steps=parameters->get_param_int(4); //bleh
        float TD_floor_dB=parameters->get_param_float(5); //bleh

	sample_t *dst_buffer=&buffer[0];

        if (cur_bsize<0)
	    cur_bsize=bsize;
	if (cur_shift<0)
            cur_shift=STONES_2_RATE(rate);
        if (fade_threshold<0)
            fade_threshold=trans_threshold;
        
        for (int i=0;i<p_length;i++) {



		sample_t val_l=p_buffer[i*2];
		sample_t val_r=p_buffer[i*2+1];
		dst_buffer[buffer_pos]=val_l;
		dst_buffer[buffer_pos+1]=val_r;
		buffer_pos+=2;
		buffer_pos%=wrap_size;



		double ofs; // position we read from
                double ofs_to; //position we must fade to
                double left_to_trans; //samples remaining until direction switch
                float aux_fade_threshold=fade_threshold;

		//resample_one_pingpong(ofs,ofs_to,left_to_trans,sample_counter,cur_bsize,cur_shift,aux_fade_threshold);
                resample_one_chunk_interleave(ofs,ofs_to,left_to_trans,sample_counter,cur_bsize,cur_shift,aux_fade_threshold);

//                printf("left_to_tr %i \t ofs %i \t ofs_to %i, ofs dc is %i\n\n",left_to_trans,ofs,ofs_to,ofs_dc_offset);

		float iofs_frac=ofs-(float)lrint(ofs);

		int iofs=(wrap_size+buffer_pos)-(lrintf(ofs)*2+bsize*2);
		iofs%=wrap_size;
		int iofs_next=iofs+2;
                iofs_next%=wrap_size;

		float iofs_to_frac=ofs_to-(float)lrint(ofs_to);
		int iofs_to=(wrap_size+buffer_pos)-(lrintf(ofs_to)*2+bsize*2);
		iofs_to%=wrap_size;
		int iofs_to_next=iofs_to+2;
                iofs_to_next%=wrap_size;

                sample_t res_l=dst_buffer[iofs]*(1.0-iofs_frac)+dst_buffer[iofs_next]*iofs_frac;
                sample_t res_r=dst_buffer[iofs+1]*(1.0-iofs_frac)+dst_buffer[iofs_next+1]*iofs_frac;

		if (left_to_trans<aux_fade_threshold) {

			float ratio=((float)trans_threshold-(float)left_to_trans)/(float)trans_threshold;

			res_l=(1.0-ratio)*res_l;
			res_r=(1.0-ratio)*res_r;
			res_l+=ratio*( dst_buffer[iofs_to]*(1.0-iofs_to_frac)+dst_buffer[iofs_to_next]*(iofs_to_frac) );
			res_r+=ratio*( dst_buffer[iofs_to+1]*(1.0-iofs_to_frac)+dst_buffer[iofs_to_next+1]*(iofs_to_frac) );

		}


		p_buffer[i*2]=res_l;
		p_buffer[i*2+1]=res_r;

		sample_counter++;

		if ((sample_counter % cur_bsize)==0) {
		    //end of block reached, perform scan and find cur_bsize
		    int safe_divisor=1;
		    bool threshold_ok;
		    float new_min_dB=0,new_max_dB=0;

		    do {

			threshold_ok=true;

                        new_min_dB=0,new_max_dB=0;

                        for (int j=0;j<(bsize*2/safe_divisor);j++) {

			    int t_ofs=wrap_size+buffer_pos-bsize*2+j;
                            t_ofs%=wrap_size;

			    if (dst_buffer[t_ofs]>new_max_dB)
                                new_max_dB=dst_buffer[t_ofs];
			    if (dst_buffer[t_ofs]<new_min_dB)
				new_min_dB=dst_buffer[t_ofs];


			}

			new_min_dB=10*log(fabs(new_min_dB))/log(2);
			new_max_dB=10*log(fabs(new_max_dB))/log(2);
			if (new_min_dB<TD_floor_dB)
			    new_min_dB=TD_floor_dB;
			if (new_max_dB<TD_floor_dB)
			    new_max_dB=TD_floor_dB;


			if ( (fabs(new_min_dB-pre_peak_dB_min)>=TD_thresh_dB) ||
			    (fabs(new_max_dB-pre_peak_dB_max)>=TD_thresh_dB)
			   ) {
			    //oops threshold exceeded!

			    if (safe_divisor<TD_max_steps) {
				threshold_ok=false;
				safe_divisor*=2;
			    }

			}

		    } while (!threshold_ok);

		    pre_peak_dB_max=new_max_dB;
		    pre_peak_dB_min=new_min_dB;

		    cur_bsize=bsize/safe_divisor;
		    cur_shift=STONES_2_RATE(rate);
		    sample_counter=0;
                    fade_threshold=trans_threshold;
//                    printf("new divisor %i\n",safe_divisor);

		}

	}
}

Effect_PShift::Effect_PShift(Effect::Parameters *p_parameters) : Effect(p_parameters) {

	parameters=dynamic_cast<Custom_FX_Param_List*>(p_parameters);
	
	if (parameters==NULL) {
		ERROR("This will CRASH HOW DID IT HAPPEN???!!!!!!!");
	}

	buffer.resize(BUFFER_SIZE*2);
	check_buffer_length();
	buffer_pos=0;
	sample_counter=0;
	pre_peak_dB_max=-50;
	pre_peak_dB_min=-50;
	cur_bsize=-1;
	cur_shift=-1;
        fade_threshold=-1;
	
}

Effect_PShift::~Effect_PShift() {

}

Effect::Parameters * Effect_PShift::create_parameters() {
	
	Custom_FX_Param_List * params= new Custom_FX_Param_List("Pitch Shift","INTERNAL: pshift");
	params->add_float_param("Shift Rate(semitones)","shift_rate",-36,36,0);
	params->add_float_param("BlockSize(ms)","block_size",1,250,40);
	params->add_float_param("BlockFade(ms)","transition",0,250,5);
	params->add_float_param("TD Thresh dB","td_thresh_db",0.1,50.0,6.0);
	params->add_int_param("TD Max Steps","td_max_steps",1,16,1);
	params->add_float_param("TD Floor dB","td_floor_db",-60,0,-40);
	return params;	
	
	
}

