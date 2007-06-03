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
                          cpp  -  description
                             -------------------
    begin                : Wed Jul 17 2002
    copyright            : (C) 2002 by red
    email                : red@server
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "chorus.h"
#include "math.h"



Chorus::Chorus(int p_ring_buffer_size){

	ringbuffer.resize(p_ring_buffer_size);

 	delay=3;
   	lfo_speed=2;
     	lfo_depth=1;
       	feedback=64;
 	width=-10;
  	index=0;
	ring_buffer_index=0;
}

void Chorus::set_chorus_params(float p_delay, float p_lfo_speed, float p_lfo_depth, float p_feedback, float p_width) {

	delay=p_delay;
	lfo_speed=p_lfo_speed;
 	lfo_depth=p_lfo_depth;
  	feedback=p_feedback;
   	width=p_width;

}

void Chorus::get_chorus_params(float &p_delay, float &p_lfo_speed, float &p_lfo_depth, float &p_feedback, float &p_width) {

	p_delay=delay;
	p_lfo_speed=lfo_speed;
 	p_lfo_depth=lfo_depth;
  	p_feedback=feedback;
   	p_width=width;

}
void Chorus::set_mixfreq(int p_mixfreq) {

	mixfreq=p_mixfreq;

}

void Chorus::process(Sint32 *p_dest,Sint32 *p_source,int p_amount) {


	float delay_samples = (delay/1000.0) * (float)mixfreq;
	float depth_samples = (lfo_depth/1000.0) * (float)mixfreq;

        int copy_amount=p_amount;
        int aux_idx=0;

        int real_feedback=feedback*127.0/100.0;
        int real_width=width*127.0/100.0;


        copy_amount*=2;

        while (copy_amount--) {

        	ringbuffer[(ring_buffer_index+aux_idx) % ringbuffer.size()] = *(p_source++);
        	aux_idx++;
        }


  	copy_amount=p_amount;


        while (copy_amount--) {

               float read_index=((float)index*lfo_speed)*256.0/(float)mixfreq;
                int final_index=delay_samples+(depth_samples+sin(read_index*M_PI/256.0)*depth_samples);


                int val = ringbuffer[ (ringbuffer.size()+(ring_buffer_index-final_index*2)) % ringbuffer.size() ] >> 11;//take left val

                int left=(val*real_feedback>>7); //apply the feedback thing

                val = ringbuffer[ (ringbuffer.size()+(ring_buffer_index-final_index*2)+1) % ringbuffer.size()  ] >> 11; //take right val

                int right=(val*real_feedback>>7); //apply the feedback thing

                val=right;
                right=(right*(127-real_width)>>7) + (left*(real_width)>>7);
                left=(val*(real_width)>>7) + (left*(127-real_width)>>7);
                right<<=11;
                left<<=11;

		*p_dest++ += left;
		*p_dest++ += right;

  		index++;

		ring_buffer_index+=2;
		ring_buffer_index%=ringbuffer.size();
        }



}



Chorus::~Chorus(){
}
