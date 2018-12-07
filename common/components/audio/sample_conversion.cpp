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
#include "sample_conversion.h"
#include "stdio.h"

#include <math.h>


void convert_32s_stereo_to_16s_stereo(sample_16s_t * p_dst, sample_32s_t *p_src, int p_amount) {

 	while (p_amount--) {

		*p_dst++ = *(p_src++) >> 16;	//l 
		*p_dst++ = *(p_src++) >> 16;	//r
	}
}

void convert_32s_stereo_to_16s_mono(sample_16s_t * p_dst, sample_32s_t *p_src, int p_amount) {

	sample_16s_t mono;
 	while (p_amount--) {
	
		mono =     *(p_src++) >> 17;	//l 
		mono +=     *(p_src++) >> 17;	//r
		*p_dst++ = mono;
	}

}

void convert_32s_stereo_to_8u_stereo(sample_8u_t * p_dst, sample_32s_t *p_src, int p_amount) {

 	while (p_amount--) {
	
		*p_dst++ = (*(p_src++) >> 24)+0x80;	//l 
		*p_dst++ = (*(p_src++) >> 24)+0x80;	//r

	}

}
void convert_32s_stereo_to_8u_mono(sample_8u_t * p_dst, sample_32s_t *p_src, int p_amount) {

	sample_16s_t mono;

 	while (p_amount--) {

		mono =     *(p_src++) >> 24;	//l
		mono +=     *(p_src++) >> 24;	//r
		mono >>=2;
		mono +=0x80;
		*p_dst++ = (sample_8u_t)mono;
	}

}


void convert_32s_stereo_to_16s_stereo_add(sample_16s_t * p_dst, sample_32s_t *p_src, int p_amount) {

 	while (p_amount--) {

		*p_dst++ += *(p_src++) >> 16;	//l
		*p_dst++ += *(p_src++) >> 16;	//r
	}
}

void convert_32s_stereo_to_16s_mono_add(sample_16s_t * p_dst, sample_32s_t *p_src, int p_amount) {

	sample_16s_t mono;
 	while (p_amount--) {

		mono =     *(p_src++) >> 17;	//l
		mono +=     *(p_src++) >> 17;	//r
		*p_dst++ += mono;
	}

}

void convert_32s_stereo_to_8u_stereo_add(sample_8u_t * p_dst, sample_32s_t *p_src, int p_amount) {

 	while (p_amount--) {

		*p_dst++ += (*(p_src++) >> 24)+0x80;	//l
		*p_dst++ += (*(p_src++) >> 24)+0x80;	//r

	}

}
void convert_32s_stereo_to_8u_mono_add(sample_8u_t * p_dst, sample_32s_t *p_src, int p_amount) {

	sample_16s_t mono;

 	while (p_amount--) {

		mono =     *(p_src++) >> 24;	//l
		mono +=     *(p_src++) >> 24;	//r
		mono >>=2;
		mono +=0x80;
		*p_dst++ += (sample_8u_t)mono;
	}

}










void convert_32f_stereo_to_16s_stereo_add(sample_16s_t * p_dst, sample_32f_t *p_src, int p_amount) {

 	while (p_amount--) {

		*p_dst++ += lrintf( *(p_src++) * 32767.0 );
		*p_dst++ += lrintf( *(p_src++) * 32767.0 );
	}
}

void convert_32f_stereo_to_16s_mono_add(sample_16s_t * p_dst, sample_32f_t *p_src, int p_amount) {

	sample_16s_t mono;
 	while (p_amount--) {

		mono = lrintf( *(p_src++) * 16383.0 );
		mono += lrintf( *(p_src++) * 16383.0 );
		
		*p_dst++ += mono;
	}

}

void convert_32f_stereo_to_8u_stereo_add(sample_8u_t * p_dst, sample_32f_t *p_src, int p_amount) {

 	while (p_amount--) {

		*p_dst++ += lrintf( (*(p_src++)+1.0) * 127.5 );
		*p_dst++ += lrintf( (*(p_src++)+1.0) * 127.5 );

	}

}
void convert_32f_stereo_to_8u_mono_add(sample_8u_t * p_dst, sample_32f_t *p_src, int p_amount) {

	sample_16s_t mono;

 	while (p_amount--) {

		mono =   lrintf( (*(p_src++)+1.0) * 64.25 );
		mono +=   lrintf( (*(p_src++)+1.0) * 64.25 );
		
		*p_dst++ += (sample_8u_t)mono;
	}

}

void convert_32s_stereo_to_float_stereo_doubled(float *p_dst_l,float *p_dst_r,sample_32s_t *p_src ,int p_amount) {

 	while (p_amount--) {

		*p_dst_l++ = (float)(*(p_src++) >> 16) / 32768.0;
		*p_dst_r++ = (float)(*(p_src++) >> 16) / 32768.0;
	}


}

void convert_deinterleaved_float_stereo(float *p_dst_l,float *p_dst_r,sample_t *p_src ,int p_amount) {
	
	
 	while (p_amount--) {

		*p_dst_l++ = *(p_src++);
		*p_dst_r++ = *(p_src++);
	}
	
}


