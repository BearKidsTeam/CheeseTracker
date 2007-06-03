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
//
//
// C++ Implementation: simple_edit_effects
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "simple_edit_effects.h"

/***********************************
              REVERSE
************************************/


/* Processing */
void Edit_Effect_Reverse::process(Sample_Data *p_data,int p_begin,int p_end) {


	int i;

	int size=p_end-p_begin;
	size++;

	for (i=0;i<size/2;i++) {

		float aux_val=p_data->get_sample(i+p_begin);
		p_data->set_sample( i+p_begin , p_data->get_sample(p_end-i) );
		p_data->set_sample( p_end-i ,aux_val );
	}


	if  ( (p_data->get_loop_begin()>=p_begin) && (p_data->get_loop_begin()<=p_end) ) {

		int loop_rel=p_data->get_loop_begin() - p_begin;
		p_data->set_loop_begin( p_end - loop_rel );
	}

	if  ( (p_data->get_loop_end()>=p_begin) && (p_data->get_loop_end()<=p_end) ) {

		int loop_rel=p_data->get_loop_end() - p_begin;
		p_data->set_loop_end( p_end - loop_rel );
	}

}

void Edit_Effect_SelToLoop::process(Sample_Data *p_data,int p_begin,int p_end) {


	if (!p_data->get_size())
		return;

	p_data->set_loop_enabled(true);
	p_data->set_loop_begin(p_begin);
	p_data->set_loop_end(p_end);
}

void Edit_Effect_Toggle_Sign::process(Sample_Data *p_data,int p_begin,int p_end) {


	if (!p_data->get_size())
		return;

	p_data->change_sign();
}

void Edit_Effect_FadeIn::process(Sample_Data *p_data,int p_begin,int p_end) {


	if (!p_data->get_size())
		return;

	for (int i=p_begin;i<=p_end;i++) {

		p_data->set_sample(i, p_data->get_sample(i) * ( (float)(i-p_begin)/(float)(p_end-p_begin) ) );
	}

}

void Edit_Effect_FadeOut::process(Sample_Data *p_data,int p_begin,int p_end) {


	if (!p_data->get_size())
		return;

	for (int i=p_begin;i<=p_end;i++) {

		p_data->set_sample(i, p_data->get_sample(i) * ( (float)(p_end-i)/(float)(p_end-p_begin) ) );
	}



}

list<Property_Bridge*> Edit_Effect_Tunner::get_property_list() {

 	list<Property_Bridge*> props;
	props.push_back(&cycle_prop);
	return props;

}

void Edit_Effect_Tunner::process(Sample_Data *p_data,int p_begin,int p_end) {


	if (!p_data->get_size())
		return;

	float len=float(p_end-p_begin);
	if (len<3.0f) //avoid stupidity
		return;


        float count=float(cycles);
        float c5_freq=261.6255f;

	float cycle_len=len/count;

	cycle_len*=c5_freq;

	p_data->set_c5_freq(int(cycle_len));
}

Edit_Effect_Tunner::Edit_Effect_Tunner() : cycle_prop("Cycles Selected:",&cycles,1,100) {

	cycles=4;
}


void Edit_Effect_Normalize::selected_notify(Sample_Data *p_data,int p_begin,int p_end) {

	float max=1.0f;

	int i;
	for (i=p_begin;i<=p_end;i++) {

		float aux_val=fabs(p_data->get_sample(i));

		if ((i==p_begin) || (aux_val>max)) {

			max=aux_val;
		}
	}

	if (max==0.0f) max=0.0001f;
        max=1.0f/max;
	max*=100.0f;
	//printf("decided mult %f\n",max);
	amp=int(max);
	// if (amp>MULT_MAX) amp=MULT_MAX;
}
void Edit_Effect_Normalize::process(Sample_Data *p_data,int p_begin,int p_end) {


	if (!p_data->get_size())
		return;

	float mult=amp;
	mult/=100.0f;

	for (int i=p_begin;i<=p_end;i++) {

		p_data->set_sample(i, p_data->get_sample(i) * mult );
	}
}

Edit_Effect_Normalize::Edit_Effect_Normalize() : amp_prop("Amplify(%):",&amp,0,MULT_MAX) {

	amp=100;
}

list<Property_Bridge*> Edit_Effect_Normalize::get_property_list() {

	list<Property_Bridge*> props;
	props.push_back(&amp_prop);
	return props;
}

void Edit_Effect_Toggle_Depth::process(Sample_Data *p_data,int p_begin,int p_end) {


	if (!p_data->get_size())
		return;

	p_data->toggle_quality();
}

void Edit_Effect_PostLoop_Cut::process(Sample_Data *p_data,int p_begin,int p_end) {

	if (!p_data->is_loop_enabled())
		return;
	if (!p_data->get_size(0))
		return;
	if (p_data->get_loop_end()<=0)
		return;

	for(size_t chan=0; chan<p_data->num_channels(); chan++) {
		p_data->seek(chan, p_data->get_loop_end()+1);
		p_data->truncate(chan);
	}
}

void Edit_Effect_Center::process(Sample_Data *p_data,int p_begin,int p_end) {

	if (!p_data->is_loop_enabled())
		return;
	if (!p_data->get_size())
		return;

	float max=-2.0,min=2.0;
	float real_max=1.0,real_min=-1.0;
	float diff;

	int i;
	for (i=p_begin;i<=p_end;i++) {

		float aux_val=p_data->get_sample(i);

		if (aux_val>max) max=aux_val;
		if (aux_val<min) min=aux_val;
	}

	diff=(real_max-max)-(((real_max-real_min)-(max-min))/2.0);

	for (i=p_begin;i<=p_end;i++) {

		p_data->set_sample(i,p_data->get_sample(i)+diff);
	}
}

void Edit_Effect_PreLoop_Cut::process(Sample_Data *p_data,int p_begin,int p_end) {

	if (!p_data->is_loop_enabled())
		return;
	if (p_data->get_loop_end()<=0)
		return;
	if (p_data->get_loop_begin()<=0)
		return;

	for(size_t chan=0; chan<p_data->num_channels(); chan++) {
		if (!p_data->get_size(chan))
			continue;
		size_t new_size = p_data->get_size(chan) - p_data->get_loop_begin();
		sample_int_t *new_data = new sample_int_t[new_size];

		p_data->seek(chan, p_data->get_loop_begin());
		p_data->get_sample_array(chan, new_data, new_size);
		p_data->seek(chan, 0);
		p_data->put_sample_array(chan, new_data, new_size);
		delete new_data;
		p_data->truncate(chan);
	}
	p_data->set_loop_end( p_data->get_loop_end() - p_data->get_loop_begin() );
	p_data->set_loop_begin( 0 );
}

