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
                          sample_data.cpp  -  description
                             -------------------
    begin                : Mon Feb 5 2001
    copyright            : (C) 2001 by Juan Linietsky
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

#include "sample_data.h"

Sample_Data::Sample_Data(){

	data_ptr=NULL;
 	size=0;
}

Sample_Data::~Sample_Data(){

}

/* Misc internal Editing Utils */

int Sample_Data::get_data_value(int p_pos) {

	if (data_ptr==NULL) return 0;

	if ((p_pos<0) || (p_pos>size)) return 0;

        if (is_16bits) {

		return data_ptr[p_pos];
	} else {

		Sint8 *sample_8bits_ptr=(Sint8*)data_ptr;
		return sample_8bits_ptr[p_pos];

	}

}
void Sample_Data::put_data_value(int p_pos,int p_val) {

	if (data_ptr==NULL) return;

	if ((p_pos<0) || (p_pos>size)) return;

        if (is_16bits) {

		p_val=BOUND(p_val,-32768,32767);
		data_ptr[p_pos]=p_val;

	} else {

		p_val=BOUND(p_val,-128,127);

		Sint8 *sample_8bits_ptr=(Sint8*)data_ptr;
		sample_8bits_ptr[p_pos]=p_val;

	}

}

void Sample_Data::reverse() {

	if (data_ptr==NULL) return;

	int i,aux_val;
	
	for (i=0;i<size/2;i++) {

		aux_val=get_data_value(i);
		put_data_value(i,get_data_value((size-i)-1));
		put_data_value((size-i)-1,aux_val);		
	}

	aux_val=loop_end;	
	loop_end=(size-loop_begin);
	loop_begin=(size-aux_val);

	aux_val=sustain_loop_end;	
	sustain_loop_end=(size-sustain_loop_begin);
	sustain_loop_begin=(size-aux_val);
}

void Sample_Data::post_loop_cut() {

	if (data_ptr==NULL) return;

	Uint32 new_size;

	if (!loop_on && !sustain_loop_on) {

		return;

	} else if (loop_on && !sustain_loop_on) {

		new_size=loop_end;

		if (sustain_loop_begin>new_size) sustain_loop_begin=new_size-1;
		if (sustain_loop_end>new_size) sustain_loop_end=new_size;

	} else if (!loop_on && sustain_loop_on) {

		new_size=sustain_loop_end;

		if (loop_begin>new_size) loop_begin=new_size-1;
		if (loop_end>new_size) loop_end=new_size;
	
	} else {

		new_size=(loop_end>sustain_loop_end)?loop_end:sustain_loop_end;
	}

	
	size=new_size;

	realloc((void*)data_ptr,new_size*(is_16bits?2:1));
		
}

void Sample_Data::pre_loop_cut() {

	if (data_ptr==NULL) return;

	reverse();
	post_loop_cut();
	reverse();
	
}

void Sample_Data::amplify(int p_amount) {

	if (data_ptr==NULL) return;

	int i;

	for (i=0;i<size;i++) put_data_value(i,(get_data_value(i)*p_amount)/100);

}

void Sample_Data::change_sign() {

	if (data_ptr==NULL) return;

	int i;

	for (i=0;i<size;i++) {

		if (is_16bits) {

			data_ptr[i]+=32768;
		} else {

      			Sint8 *sample_8bits_ptr=(Sint8*)data_ptr;
	                sample_8bits_ptr[i]+=128;
		}
	}
}

void Sample_Data::toggle_quality() {

	if (data_ptr==NULL) return;

	Sint8 *new_sample_8bits;
	Sint16 *new_sample_16bits;
	int i;

	if (is_16bits) {

		(void*)new_sample_8bits=malloc(size);
		if (new_sample_8bits==NULL) {

			ERROR("Cannot convert sample! not enough memory?");
			return;
		}
		for (i=0;i<size;i++) new_sample_8bits[i]=data_ptr[i] >> 8;
		free(data_ptr);
		data_ptr=(Sint16*)new_sample_8bits;
		is_16bits=false;

	} else {

		Sint8 *sample_8bits_ptr=(Sint8*)data_ptr;
		(void*)new_sample_16bits=malloc(size*2);
		if (new_sample_16bits==NULL) {

			ERROR("Cannot convert sample! not enough memory?");
			return;
		}

		for (i=0;i<size;i++) new_sample_16bits[i]=(Sint16)sample_8bits_ptr[i] << 8;
		free(data_ptr);
		data_ptr=new_sample_16bits;
		is_16bits=true;
	}
		

}

void Sample_Data::resize(int p_new_size) {

	if (data_ptr==NULL) return;

	if (p_new_size<1) return;

	Sint8 *new_sample_8bits;
	Sint16 *new_sample_16bits;
        int i;

	if (is_16bits) {

		(void*)new_sample_16bits=malloc(p_new_size*2);
		if (new_sample_16bits==NULL) {

			ERROR("Cannot resize sample! not enough memory?");
			return;
		}
        	for (i=0;i<p_new_size;i++) new_sample_16bits[i]=get_data_value((Uint64)i*(Uint64)size/(Uint64)p_new_size);
		free(data_ptr);
		data_ptr=new_sample_16bits;
	
	} else {
		(void*)new_sample_8bits=malloc(p_new_size);
		if (new_sample_8bits==NULL) {

			ERROR("Cannot resize sample! not enough memory?");
			return;
		}

        	for (i=0;i<p_new_size;i++) new_sample_8bits[i]=get_data_value((Uint64)i*(Uint64)size/(Uint64)p_new_size);
		free(data_ptr);
		data_ptr=(Sint16*)new_sample_8bits;
	}


	import_frequency(((Uint64)export_frequency()*(Uint64)p_new_size)/(Uint64)size);
	size=p_new_size;
}

void Sample_Data::resize_interpolated(int p_new_size) {

	if (data_ptr==NULL) return;

	if (p_new_size<1) return;

	Sint8 *new_sample_8bits;
	Sint16 *new_sample_16bits;
        int avg_acc=0,avg_count=0,i,j;
        int old_pos=-1;

	if (is_16bits) {

		(void*)new_sample_16bits=malloc(p_new_size*2);
		if (new_sample_16bits==NULL) {

			ERROR("Cannot resize sample! not enough memory?");
			return;
		}
	} else {
		(void*)new_sample_8bits=malloc(p_new_size);

		if (new_sample_8bits==NULL) {

			ERROR("Cannot resize sample! not enough memory?");
			return;
		}
	}

        for (i=0;i<size;i++) {

		int current_pos=(Uint64)i*(Uint64)p_new_size/(Uint64)size;

		if (current_pos>old_pos) {

			if ( (old_pos>=0) && (current_pos>=0) && (current_pos<p_new_size) ) {

				for (j=0;j<(current_pos-old_pos);j++) {

					if (is_16bits) {

						new_sample_16bits[old_pos+j]=avg_acc/avg_count;
					} else {

						new_sample_8bits[old_pos+j]=avg_acc/avg_count;
					}
				}
               		}
			avg_count=0;
			avg_acc=0;
			old_pos=current_pos;
		}

		avg_count++;
		avg_acc+=get_data_value(i);
	}
	
			
	if (is_16bits) {

		free(data_ptr);
		data_ptr=new_sample_16bits;

	} else {

		free(data_ptr);
		data_ptr=(Sint16*)new_sample_8bits;
	}

	import_frequency(((Uint64)export_frequency()*(Uint64)p_new_size)/(Uint64)size);
	size=p_new_size;
}


void Sample_Data::centralize() {

	if (data_ptr==NULL) return;

	int max=-100000,min=100000,i;
	int real_max,real_min;
	int diff;
	
	real_max=(is_16bits)?32767:127;
	real_min=(is_16bits)?-32768:-128;

	for (i=0;i<size;i++) {
		
		int aux_val=get_data_value(i);

		if (aux_val>max) max=aux_val;
		if (aux_val<min) min=aux_val;
	}

	diff=(real_max-max)-(((real_max-real_min)-(max-min))/2);

	for (i=0;i<size;i++) {

		put_data_value(i,get_data_value(i)+diff);
	}
}

int Sample_Data::get_value_to_normalize() {

	if (data_ptr==NULL) return 0;

	int max=-100000,min=100000,i;
	int real_max,real_min;
	int diff,diff_max,diff_min;
	
	
	real_max=(is_16bits)?32767:127;
	real_min=(is_16bits)?-32768:-128;

	for (i=0;i<size;i++) {
		
		int aux_val=get_data_value(i);

		if (aux_val>max) max=aux_val;
		if (aux_val<min) min=aux_val;
	}

	diff_max=abs(real_max-max);
	diff_min=abs(real_min-min);

	if (diff_max<diff_min) {

		diff=(real_max*100)/(max?max:1);
	} else {

		diff=(real_min*100)/(min?min:1);
	}		
	
	return diff;
}

void Sample_Data::import_frequency(int p_freq) {

	int ctmp=0,tmp,note=0,aux_finetune=0;

	p_freq>>=1; // Convert C5 to C4


	while((tmp=Tables::get_old_frequency(Tables::get_old_period(note<<1,0)))<p_freq) {

		ctmp=tmp;
        	note++;
	}

	if(tmp!=p_freq) {
		
		if((tmp-p_freq)<(p_freq-ctmp)) {

			while(tmp>p_freq) {

				tmp=Tables::get_old_frequency(Tables::get_old_period(note<<1,--aux_finetune));
			}
		}

		else {
			
			note--;
			while(ctmp<p_freq) {

				ctmp=Tables::get_old_frequency(Tables::get_old_period(note<<1,++aux_finetune));
			}
		}
	}

	if (note<1) {

		note=1;
		ERROR("***WARNING*** detuning note to avoid unsupported speed\n");
	}

	if (aux_finetune<-128) {

		aux_finetune=-128;
		ERROR("***WARNING*** detuning finetune to avoid unsupported speed\n");
	}


	note_offset=note-48; // 48 is the middle C

	finetune=aux_finetune;
}

int Sample_Data::export_frequency() {

 	return Tables::get_linear_frequency(Tables::get_linear_period((48+note_offset)<<1,finetune))*2;
}