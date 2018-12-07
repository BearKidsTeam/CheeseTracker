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
#ifndef SAMPLE_CONVERSION_H
#define SAMPLE_CONVERSION_H

#include "sample_defs.h"



void convert_32s_stereo_to_16s_stereo(sample_16s_t * p_dst, sample_32s_t *p_src, int p_amount);
void convert_32s_stereo_to_16s_mono(sample_16s_t * p_dst, sample_32s_t *p_src, int p_amount);

void convert_32s_stereo_to_8u_stereo(sample_8u_t * p_dst, sample_32s_t *p_src, int p_amount);
void convert_32s_stereo_to_8u_mono(sample_8u_t * p_dst, sample_32s_t *p_src, int p_amount);

void convert_32s_stereo_to_16s_stereo_add(sample_16s_t * p_dst, sample_32s_t *p_src, int p_amount);
void convert_32s_stereo_to_16s_mono_add(sample_16s_t * p_dst, sample_32s_t *p_src, int p_amount);

void convert_32s_stereo_to_8u_stereo_add(sample_8u_t * p_dst, sample_32s_t *p_src, int p_amount);
void convert_32s_stereo_to_8u_mono_add(sample_8u_t * p_dst, sample_32s_t *p_src, int p_amount);

void convert_32f_stereo_to_16s_stereo_add(sample_16s_t * p_dst, sample_32f_t *p_src, int p_amount);
void convert_32f_stereo_to_16s_mono_add(sample_16s_t * p_dst, sample_32f_t *p_src, int p_amount);

void convert_32f_stereo_to_8u_stereo_add(sample_8u_t * p_dst, sample_32f_t *p_src, int p_amount);
void convert_32f_stereo_to_8u_mono_add(sample_8u_t * p_dst, sample_32f_t *p_src, int p_amount);


void convert_32s_stereo_to_float_stereo_doubled(float *p_dst_l,float *p_dst_r,sample_32s_t *p_src ,int p_amount);
void convert_deinterleaved_float_stereo(float *p_dst_l,float *p_dst_r,sample_t *p_src ,int p_amount);


#endif
