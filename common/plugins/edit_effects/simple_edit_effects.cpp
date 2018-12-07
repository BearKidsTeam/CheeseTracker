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

#include <cstring>
#include "ns_autoptr.h"
#include "simple_edit_effects.h"

/***********************************
              REVERSE
************************************/


/* Processing */
void Edit_Effect_Reverse::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{
	size_t i;
	size_t size=p_end-p_begin;
	size_t channels = p_data->num_channels();
	size++;
	sample_int_t *aux_val;
	ns_autoptr<sample_int_t> ns_aux_val;

	aux_val = new sample_int_t[channels];
	ns_aux_val.arr_new(aux_val);

	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);

	// Reverse the sample data.
	//
	for (i=0;i<size/2;i++) {

		memcpy(aux_val, p_data->get_data_value(i+p_begin), channels*sizeof(sample_int_t));
		p_data->put_data_value(i+p_begin, p_data->get_data_value(p_end-i));
		p_data->put_data_value(p_end-i, aux_val);
	}


	// Fuck with the loop pointers.
	if  ( (p_data->get_loop_begin()>=p_begin) && (p_data->get_loop_begin()<=p_end) ) {

		size_t loop_rel=p_data->get_loop_begin() - p_begin;
		p_data->set_loop_begin( p_end - loop_rel );
	}

	if  ( (p_data->get_loop_end()>=p_begin) && (p_data->get_loop_end()<=p_end) ) {

		size_t loop_rel=p_data->get_loop_end() - p_begin;
		p_data->set_loop_end( p_end - loop_rel );
	}

}

void Edit_Effect_SelToLoop::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{


	if (!p_data->get_size())
		return;
	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);

	p_data->set_loop_enabled(true);
	p_data->set_loop_begin(p_begin);
	p_data->set_loop_end(p_end);
}

void Edit_Effect_Toggle_Sign::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{


	if (!p_data->get_size())
		return;

	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);

	p_data->change_sign();
}

void Edit_Effect_FadeIn::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{
	if (!p_data->get_size())
		return;

	size_t channels=p_data->num_channels();
	float *aux_val;
	ns_autoptr<float> ns_aux_val;

	aux_val = new float[channels];
	ns_aux_val.arr_new(aux_val);

	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);

	for (ptrdiff_t i=p_begin; i < p_end; i++) {
		p_data->get_sample(i, aux_val);
		for(size_t chan=0; chan<channels; chan++) {
			aux_val[chan] *= (float)(i-p_begin)/(float)(p_end-p_begin) ;
		}
		
		p_data->set_sample(i, aux_val);
	}

}

void
Edit_Effect_FadeOut::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{

	if (!p_data->get_size())
		return;

	size_t channels=p_data->num_channels();
	float *aux_val;
	ns_autoptr<float> ns_aux_val;

	aux_val = new float[channels];
	ns_aux_val.arr_new(aux_val);

	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);

	for (ptrdiff_t i=p_begin; i < p_end; i++) {
		p_data->get_sample(i, aux_val);
		for(size_t chan=0; chan<channels; chan++) {
			aux_val[chan] *=  (float)(p_end-i)/(float)(p_end-p_begin) ;
		}
		
		p_data->set_sample(i, aux_val);
	}

}

list<Property_Bridge*> Edit_Effect_Tunner::get_property_list()
{

 	list<Property_Bridge*> props;
	props.push_back(&cycle_prop);
	return props;

}

void Edit_Effect_Tunner::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{


	if (!p_data->get_size())
		return;

	float len=float(p_end-p_begin);
	if (len<3.0f) //avoid stupidity
		return;


        float count=float(cycles);
        float c5_freq=261.6255f;

	float cycle_len=len/count;

	cycle_len*=c5_freq;



	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	p_data->set_c5_freq(int(cycle_len));
	delete p_data_lock;
}

Edit_Effect_Tunner::Edit_Effect_Tunner() : cycle_prop("Cycles Selected:",(int*)&cycles,1,100)
{

	cycles=4;
}


void Edit_Effect_Normalize::selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{

	float max=1.0f;

	ptrdiff_t i;

	size_t channels=p_data->num_channels();
	float *aux_val;
	ns_autoptr<float> ns_aux_val;

	aux_val = new float[channels];
	ns_aux_val.arr_new(aux_val);


	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);

	for (i = p_begin; i < p_end; i++) {

		p_data->get_sample(i, aux_val);

		for(size_t chan=0; chan<channels; chan++) {
			if ((i==p_begin) || (fabs(aux_val[chan])>max)) {
				max=fabs(aux_val[chan]);
			}
		}
	}

	if (max==0.0f) max=0.0001f;
        max=1.0f/max;
	max*=100.0f;
	//printf("decided mult %f\n",max);
	amp=int(max);
	// if (amp>MULT_MAX) amp=MULT_MAX;
}
void Edit_Effect_Normalize::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{


	if (!p_data->get_size())
		return;

	float mult=amp;
	mult/=100.0f;


	size_t channels=p_data->num_channels();
	float *aux_val;
	ns_autoptr<float> ns_aux_val;

	aux_val = new float[channels];
	ns_aux_val.arr_new(aux_val);


	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);

	for (ptrdiff_t i=p_begin; i < p_end; i++) {
		p_data->get_sample(i, aux_val);
		for(size_t chan=0; chan<channels; chan++) {
			aux_val[chan] *= mult;
		}
		p_data->set_sample(i, aux_val);
	}
}

Edit_Effect_Normalize::Edit_Effect_Normalize() : amp_prop("Amplify(%):",&amp,0,MULT_MAX)
{

	amp=100;
}

list<Property_Bridge*> Edit_Effect_Normalize::get_property_list()
{

	list<Property_Bridge*> props;
	props.push_back(&amp_prop);
	return props;
}

void Edit_Effect_Toggle_Depth::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{


	if (!p_data->get_size())
		return;

	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);
	p_data->toggle_quality();
}

void Edit_Effect_PostLoop_Cut::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{

	if (!p_data->is_loop_enabled())
		return;
	if (!p_data->get_size())
		return;
	if (p_data->get_loop_end()<=0)
		return;

	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);
	p_data->seek(p_data->get_loop_end()+1);
	p_data->truncate();
}

void Edit_Effect_Center::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{

	if (!p_data->is_loop_enabled())
		return;
	if (!p_data->get_size())
		return;

	float max=-2.0,min=2.0;
	float real_max=1.0,real_min=-1.0;
	float diff;


	size_t channels=p_data->num_channels();
	float *aux_val;
	ns_autoptr<float> ns_aux_val;

	aux_val = new float[channels];
	ns_aux_val.arr_new(aux_val);

	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);

	ptrdiff_t i;
	for (i=p_begin; i < p_end; i++) {

		p_data->get_sample(i, aux_val);

		for(size_t chan=0; chan<channels; chan++) {
			if (aux_val[chan]>max) max=aux_val[chan];
			if (aux_val[chan]<min) min=aux_val[chan];
		}
	}

	diff=(real_max-max)-(((real_max-real_min)-(max-min))/2.0);

	for (i=p_begin; i < p_end; i++) {
		p_data->get_sample(i, aux_val);
		for(size_t chan=0; chan<channels; chan++) {
			aux_val[chan] += diff;
		}
		p_data->set_sample(i,aux_val);
	}
}

void Edit_Effect_PreLoop_Cut::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)
{


	if (!p_data->is_loop_enabled())
		return;
	if (p_data->get_loop_end()<=0)
		return;
	if (p_data->get_loop_begin()<=0)
		return;

	if (!p_data->get_size())
		return;

	// Copy the data after the start of the loop into a temporary buffer.

	size_t new_size = p_data->get_size() - p_data->get_loop_begin();
	sample_int_t *new_data = new sample_int_t[new_size*p_data->num_channels()];

	Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
	ns_p_data_lock.ptr_new(p_data_lock);

	p_data->seek(p_data->get_loop_begin());
	p_data->get_sample_array(new_data, new_size);
	
	// Correct the loop pointers first so that truncate() doesn't do it for us
	// the wrong way.

	p_data->set_loop_end( p_data->get_loop_end() - p_data->get_loop_begin() );
	p_data->set_loop_begin( 0 );

	// Paste the data at the beginning of the sample and truncate the sample.

	p_data->seek(0);
	p_data->put_sample_array(new_data, new_size);
	delete new_data;
	p_data->truncate();

}


void Edit_Effect_Trim::process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {
  //Trim sample to selection
  //Like using pre+post loop cut, but
  //all in one operation and doesn't need looping to be
  //enabled and disabled

  if (!p_data->get_size()) //Sample length 0
    return;
  if (p_end-p_begin<=0) //No selection
    return;

  //Work out how big the new sample will be and make space for it
  int new_size=1+p_end-p_begin;
  sample_int_t * new_data = new sample_int_t[new_size*p_data->num_channels()];


  //Lock
  Mutex_Lock_Container *p_data_lock = p_data->lock(__FILE__, __LINE__);
  ns_autoptr<Mutex_Lock_Container> ns_p_data_lock;
  ns_p_data_lock.ptr_new(p_data_lock);

  //Move to the start of the selection and grab a selection's length
  p_data->seek(p_begin);
  p_data->get_sample_array(new_data, new_size);

  //Move to 0 and paste the data
  p_data->seek(0);
  p_data->put_sample_array(new_data, new_size);
  delete new_data;

  //Paste should leave the position at the end of the pasted data,
  //so remove all data after that point
  p_data->truncate();

  //I couldn't think of anything clever to do with loop points
  //in this situation, so I've set them to start and end.
  p_data->set_loop_end( new_size );
  p_data->set_loop_begin( 0 );

}
