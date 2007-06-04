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
                          sample.cpp  -  description
                             -------------------
    begin                : Sat Jan 20 2001
    copyright            : (C) 2001 by c0del4rvi4
    email                : reduz@anime.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sample.h"

static const char * vibratos[4]={"Sine","Saw","Square","Random"};

Sample::Sample() :

	prop_name("Name",&name),
	prop_filename("File Name",&filename),


	prop_def_volume("Default Volume",&def_volume,0,64),
	prop_glb_volume("Global Volume",&glb_volume,0,64),

	prop_def_panning_on("Enable Default Pan",&def_panning_on),
	prop_def_panning("Default Panning",&def_panning,0,64),

	prop_vibrato_type("Vibrato Type",&vibrato_type, vibratos,4),
	prop_vibrato_speed("Vibrato Speed",&vibrato_speed,0,64),
	prop_vibrato_depth("Vibrato Depth",&vibrato_depth,0,32),
	prop_vibrato_rate("Vibrato Rate",&vibrato_rate,0,255)
{
	in_use=false;
	reset(false);
}

void Sample::reset(bool try_to_free_data) {

	name="";
	filename="";

	def_volume=64;
	glb_volume=64;

	def_panning_on=false;
	def_panning=32;

	vibrato_type=0;
	vibrato_speed=0;
	vibrato_depth=0;
	vibrato_rate=0;


//	data.sustain_loop_on=false;

	data.set_loop_begin(0);
	data.set_loop_end(0);

	//data.sustain_loop_begin=0;
	//data.sustain_loop_end=5;

	data.set_c5_freq(44100);
	if (in_use) {

		data.set_size(0);
		data.set_num_channels(0);
	} 
	
	in_use=false;



}

const Sample& Sample::operator=(const Sample& r_sample) {

	name=r_sample.name;
	filename=r_sample.filename;

	def_volume=r_sample.def_volume;
	glb_volume=r_sample.glb_volume;

	def_panning_on=r_sample.def_panning_on;
	def_panning=r_sample.def_panning;

	vibrato_type=r_sample.vibrato_type;
	vibrato_speed=r_sample.vibrato_speed;
	vibrato_depth=r_sample.vibrato_depth;
	vibrato_rate=r_sample.vibrato_rate;

	data=r_sample.data;
	in_use=r_sample.in_use;

	return *this;
}


