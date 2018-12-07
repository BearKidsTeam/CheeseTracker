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
// C++ Implementation: cpp
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ns_autoptr.h"
#include "sample_viewer.h"
#include "Error.h"
#include <qmessagebox.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QEvent>


bool Sample_Viewer::colors_initialized;

int Sample_Viewer::color_list[Col_Max*3] = {

		255,255,255,
		0,83,150,
		0,0,0,
		255,0,0,
		83,0,150,
		0,0,0,

		0,83,150,
		245,245,245,
		150,150,150,
		83,0,150,
		255,0,0,
		0,0,0
};

QColor Sample_Viewer::colors[Col_Max];

void Sample_Viewer::init_colormap() {

	if (colors_initialized)
		return;

	int i;

	for (i=0;i<Col_Max*3;i+=3) {

		colors[i/3].setRgb(color_list[i],color_list[i+1],color_list[i+2]);
	}

	colors_initialized=true;
}

QColor * Sample_Viewer::get_color(Colors p_color) {

	if (p_color==Col_Max)
		return NULL;
	return &colors[p_color];
}


int Sample_Viewer::screen_pos_to_sample_pos(int p_pos) {

	float pos=p_pos;

	pos=(double)pos*((double)sample_data->get_size()*zoom)/(double)width();
	pos+=(float)offset;

	return (int)pos;

}

bool Sample_Viewer::event ( QEvent * e ) {

	switch (e->type()) {

		case QEvent::MouseMove: {

			if (sample_data->get_size()==0)
				break;
			if (!selection)
				break;

			QMouseEvent * m = static_cast<QMouseEvent *>(e);

			if (grabbing_mouse) {

				switch( grab_type ){

					case SET_SELECTION: {

						selection->set_point_2( screen_pos_to_sample_pos( m->x() ) );

					} break;
					case SET_SELECTION_BEGIN:
					case SET_SELECTION_END: {

						if (closer_point==1)
							selection->set_point_1( screen_pos_to_sample_pos( m->x() ) );
						else
							selection->set_point_2( screen_pos_to_sample_pos( m->x() ) );

					} break;
				}

				update();
			}
		} break;

		case QEvent::MouseButtonRelease: {
			if (sample_data->get_size()==0)
				break;
			if (!selection)
				break;

			QMouseEvent * m = static_cast<QMouseEvent *>(e);

                        if (grabbing_mouse) {

				bool ungrab=false;

				switch(grab_type) {

					case SET_SELECTION: {

					  if (m->button()==Qt::LeftButton)
							ungrab=true;

					} break;
					case SET_SELECTION_BEGIN: {

						if (m->button()==Qt::MidButton)
							ungrab=true;



					} break;
					case SET_SELECTION_END: {

						if (m->button()==Qt::RightButton)
							ungrab=true;

					} break;
				}

				if (ungrab) {

					releaseMouse();
					grabbing_mouse=false;
					if (selection->get_begin()==selection->get_end())
						selection->set_active(false);
					update();

				}

			}


		} break;


		case QEvent::MouseButtonPress: {
			if (!selection)
				break;

			if (sample_data->get_size()==0)
				break;

			if (grabbing_mouse)
				break; //ignore it

			update();
			QMouseEvent * m = static_cast<QMouseEvent *>(e);

			switch (m->button()) {

				case Qt::LeftButton: {

					grab_type=SET_SELECTION;
					selection->clear();
					selection->set_active(true);
					selection->set_max( sample_data->get_size() );
					selection->set_point_1( screen_pos_to_sample_pos( m->x() ) );
					selection->set_point_2( screen_pos_to_sample_pos( m->x() ) );
					grabbing_mouse=true;
					grabMouse();


				} break;
				case Qt::RightButton: {

					grab_type=SET_SELECTION_END;

					if (!selection->is_active())
						break;
					grabbing_mouse=true;
					grabMouse();
					closer_point=(selection->get_point_1()>selection->get_point_2())?1:2;

				} break;
				case Qt::MidButton: {
					grab_type=SET_SELECTION_BEGIN;
					if (!selection->is_active())
						break;
					grabbing_mouse=true;
					grabMouse();
					closer_point=(selection->get_point_1()<selection->get_point_2())?1:2;
				} break;

                        	default: {}
			}
			
		} break;

		default: {

		};

	}

	return QWidget::event(e);
}



void Sample_Viewer::initialize_sample_cache() {

	sample_cache.clear();

	if (sample_data==NULL) return;

	int min_target_factor=1;
	int dest_target_size;


	do {
		min_target_factor*=2;
		dest_target_size=sample_data->get_size()/min_target_factor;
	} while (dest_target_size>max_sample_cache_size);

	//ok,  we got minimum factor, now off to get max factor
	int max_target_factor=min_target_factor/2;

	do {
		max_target_factor*=2;
		dest_target_size=sample_data->get_size()/max_target_factor;
	} while (dest_target_size>sample_cache_target);

	if (max_target_factor<=min_target_factor)
		return; //nothing to do


	while (min_target_factor!=max_target_factor) {

		SampleCache new_samplecache;
		new_samplecache.factor=min_target_factor;
		sample_cache.push_back(new_samplecache);
		min_target_factor*=2;

	}


	for (int i=0;i<(int)sample_cache.size();i++) {

		sample_cache[i].min_peak_data.resize(sample_data->get_size()/sample_cache[i].factor);
		sample_cache[i].max_peak_data.resize(sample_data->get_size()/sample_cache[i].factor);
	}


	update_sample_cache(0,sample_data->get_size()-1);
}


void Sample_Viewer::update_sample_cache(size_t p_range_begin,size_t p_range_end) {

	if (sample_cache.size()==0)
		return;

	if (p_range_end>=sample_data->get_size()) p_range_end=sample_data->get_size()-1;
	int factor=sample_cache[0].factor;

	int range_begin,range_end;
	range_begin=p_range_begin/factor;
	range_end=p_range_end/factor;

	int target_size=(p_range_end/factor);

	Mutex_Lock_Container *lock = sample_data->lock(__FILE__, __LINE__);
	ns_autoptr<Mutex_Lock_Container> ns_lock;
	ns_lock.ptr_new(lock);

	for (int i=p_range_begin;i<target_size;i++) {
		float max_peak=-2.0f;
		float min_peak=2.0f;
		for (int j=0;j<factor;j++) {
			float *sample = new float[sample_data->num_channels()];
			ns_autoptr<float> ns_sample;
			ns_sample.arr_new(sample);

			sample_data->get_sample(i*factor+j, sample);
			if (sample[0]<min_peak)
				min_peak=sample[0];
			if (sample[0]>max_peak)
				max_peak=sample[0];
		}

		sample_cache[0].max_peak_data[i]=max_peak;
		sample_cache[0].min_peak_data[i]=min_peak;
	}

	for (int i=1;i<(int)sample_cache.size();i++) {

		factor=sample_cache[i].factor;
		range_begin=p_range_begin/factor;
		target_size=(p_range_end/factor);
		for (int j=range_begin;j<target_size;j++) {
			float max_a,max_b;
			max_a=sample_cache[i-1].max_peak_data[j*2];
			max_b=sample_cache[i-1].max_peak_data[j*2+1];
			sample_cache[i].max_peak_data[j]=((max_a>max_b)?max_a:max_b);

			float min_a,min_b;
			min_a=sample_cache[i-1].min_peak_data[j*2];
			min_b=sample_cache[i-1].min_peak_data[j*2+1];
			sample_cache[i].min_peak_data[j]=((min_a<min_b)?min_a:min_b);
        	}
	}
}

int Sample_Viewer::get_factor() {

	int cur_width=width();
	int factor=int (  ((double)sample_data->get_size()*(double)zoom)/(double)cur_width );
	int dest_factor_idx=-1;
	int i;
	for (i=0;i<(int)sample_cache.size();i++) {

		if (factor<sample_cache[i].factor)
			break;
		dest_factor_idx=sample_cache[i].factor;
	}

	if (dest_factor_idx==-1) {

		return -1;
        } else {
		if (i>=(int)sample_cache.size())
			i=sample_cache.size()-1;
	return  i;
	}
}

void Sample_Viewer::screen_to_sample(int p_int, float *p_max_peak,float *p_min_peak, int sample_cache_idx,int cur_width,int *p_final_sample) {
	float sample_idx;
	float size=sample_data->get_size();
	float factor=1;

	if (sample_cache_idx!=-1) { //have cache
		size=size/sample_cache[sample_cache_idx].factor;
		factor=sample_cache[sample_cache_idx].factor;
	}

	sample_idx=(double)p_int*((double)size*zoom)/(double)cur_width;
	sample_idx+=(float)offset/factor;

	*p_final_sample=int(sample_idx*factor);

	if (sample_cache_idx!=-1) { //have cache
/*
		if (p_int>=sample_cache[sample_cache_idx].min_peak_data.size()) {
			*p_min_peak=2;
			*p_max_peak=2;

		} else {

			*p_min_peak=sample_cache[sample_cache_idx].min_peak_data[p_int];
			*p_max_peak=sample_cache[sample_cache_idx].max_peak_data[p_int];

		}
			return;
*/
		if (sample_idx>=size)
			sample_idx=size-1;

		if (sample_idx>=(size-2)) {
			*p_min_peak=sample_cache[sample_cache_idx].min_peak_data[(int)sample_idx];
			*p_max_peak=sample_cache[sample_cache_idx].max_peak_data[(int)sample_idx];
		} else {
			float fract=sample_idx-(int)sample_idx;

#define INTERP(m_a,m_b,m_fract) ((m_a)+((m_b)-(m_a))*m_fract)

			*p_min_peak=INTERP(sample_cache[sample_cache_idx].min_peak_data[(int)sample_idx],sample_cache[sample_cache_idx].min_peak_data[(int)sample_idx+1],fract);
			*p_max_peak=INTERP(sample_cache[sample_cache_idx].max_peak_data[(int)sample_idx],sample_cache[sample_cache_idx].max_peak_data[(int)sample_idx+1],fract);
		}

	} else {
		float *sample_current;
		float *sample_next;
		ns_autoptr<float> ns_sample_current;
		ns_autoptr<float> ns_sample_next;

		sample_current = new float[sample_data->num_channels()];
		ns_sample_current.arr_new(sample_current);

		sample_next = new float[sample_data->num_channels()];
		ns_sample_next.arr_new(sample_next);

		Mutex_Lock_Container *lock = sample_data->lock(__FILE__, __LINE__);
		ns_autoptr<Mutex_Lock_Container> ns_lock;
		ns_lock.ptr_new(lock);

		sample_data->get_sample((size_t)sample_idx, sample_current);

		try {
			sample_data->get_sample((size_t)sample_idx+1, sample_next);
		} catch (Out_Of_Bounds OOB) {
			sample_next[0] = 0.0;
		} 
		float fract=sample_idx-(size_t)sample_idx;
		float sample_final=INTERP(sample_current[0],sample_next[0],fract);

		*p_max_peak=sample_final;
		*p_min_peak=sample_final;
	}
}

float Sample_Viewer::get_zoom() {

	return zoom;
}
void Sample_Viewer::set_zoom(float p_factor) {

	float zoom_min=0.0001;
        if (p_factor<zoom_min) //sucks
		p_factor=zoom_min;
	if (p_factor>1.0f)
		p_factor=1.0f;

	zoom=p_factor;


	update();
}

int Sample_Viewer::get_zoom_offset() {

	return offset;
}
void Sample_Viewer::set_zoom_offset(int p_offset) {

	offset=p_offset;
	if (offset<0) offset=0;
	if (offset>get_max_zoom_offset())
		offset=get_max_zoom_offset();

	update();
}
int Sample_Viewer::get_max_zoom_offset() {

	return sample_data?sample_data->get_size()- int ((double)sample_data->get_size()*(double)zoom):0;
}


void Sample_Viewer::draw_screen_pos(int p_x,QPainter &painter,int factor) {

	int cur_width=width();
	int cur_height=height();

	float prev_min_peak;
	float prev_max_peak;

	float min_peak;
	float max_peak;
	int current_sample_idx;
	int next_sample_idx;

	screen_to_sample(p_x,&min_peak,&max_peak,factor,cur_width,&current_sample_idx);

	min_peak+=1.0f;
	min_peak/=2.0f;
	min_peak*=cur_height;
	max_peak+=1.0f;
	max_peak/=2.0f ;
	max_peak*=cur_height;

	if (p_x>0) {

		screen_to_sample(p_x-1,&prev_min_peak,&prev_max_peak,factor,cur_width,&current_sample_idx);

		prev_min_peak+=1.0f;
		prev_min_peak/=2.0f;
		prev_min_peak*=cur_height;
		prev_max_peak+=1.0f;
		prev_max_peak/=2.0f ;
		prev_max_peak*=cur_height;

	} else {
		prev_max_peak=cur_height/2;
		prev_min_peak=cur_height/2;
	}

	int final_min_peak=(int)min_peak;
	int final_max_peak=(int)max_peak;

	if (final_min_peak>(prev_max_peak+1))
		final_min_peak=(int)prev_max_peak+1;

	if (final_max_peak<(prev_min_peak-1))
		final_max_peak=(int)prev_min_peak-1;


	painter.drawLine(p_x,final_min_peak,p_x,final_max_peak);

	/* Check for loops */

	next_sample_idx=int ( (double)(p_x+1)*((double)sample_data->get_size()*zoom)/(double)cur_width );
	next_sample_idx+=offset;


        QColor bg_col;
        QColor fg_col;

	bool selected=false;
	if (selection) selected=selection->is_in_range(current_sample_idx);


	if (
		(
			(current_sample_idx<=sample_data->get_loop_begin() && next_sample_idx>=sample_data->get_loop_begin()) ||
			(current_sample_idx<=sample_data->get_loop_end() && next_sample_idx>=sample_data->get_loop_end())
		)
		) {

                bg_col=selected?colors[Col_Loop_BackGround_Selected]:colors[Col_Loop_BackGround];
                fg_col=selected?colors[Col_Loop_ForeGround_Selected]:colors[Col_Loop_ForeGround];

	} else {

                bg_col=selected?colors[Col_BackGround_Selected]:colors[Col_BackGround];
		fg_col=selected?colors[Col_ForeGround_Selected]:colors[Col_ForeGround];

	}
        if (position_display_cache[p_x]) {

		painter.setPen(selected?colors[Col_Played_Position_Selected]:colors[Col_Played_Position]);
		painter.drawLine(p_x,0,p_x,cur_height);
	} else {

		painter.setPen(bg_col);
		painter.drawLine(p_x,0,p_x,final_min_peak-1);
		painter.setPen(fg_col);
		painter.drawLine(p_x,final_min_peak,p_x,final_max_peak);
		painter.setPen(bg_col);
		painter.drawLine(p_x,final_max_peak+1,p_x,cur_height);

	}

	painter.setPen(selected?colors[Col_Center_Selected]:colors[Col_Center]);
	painter.drawPoint(p_x,cur_height/2);

}

// paintEvent - An entry point to this module
//
// notes      - Called by Qt
//

void Sample_Viewer::paintEvent( QPaintEvent * p_event ) {
	
	if(fatal_error) return;

	try {

		if ((sample_data==NULL) || (sample_data->get_size()==0)) {

			QPainter painter(this);
			painter.fillRect(0,0,width(),height(),QColor(0,0,0));
			return;
		}

		int cur_width=width();
		int cur_height=height();

		if (recompute_pending) {

			initialize_sample_cache();
			update_sample_cache(0,sample_data->get_size()-1);
			recompute_pending=false;
		}


		if (cur_height!=backing_store_height || cur_width!=backing_store_width) {

			if (backing_store)
				delete backing_store;
			backing_store = new QPixmap (cur_width,cur_height);
		}
		if ((int)position_display_cache.size()!=width()) {

			position_display_cache.resize(width());
			old_position_display_cache.resize(width());
		}

	//	float prev;
		QPainter painter;
	//	painter.begin(backing_store, this );
		painter.begin(this );

		painter.setPen(QColor(0,0,200));


		int factor=get_factor();

		// (cur_width-1) is correct. Using cur_width instead
		// will result in an abort() in versions of the vector
		// template that do bounds checking. This is a workaround.
		// The actual bug is in the floating-point math that is
		// performed on "i" before dereferencing the vector.

		for (int i=0;i<(cur_width-1);i++) {

			draw_screen_pos(i,painter,factor);
		}
	} catch (Error E) {
		fatal_error=true;	// Prevent further paintEvents, as they will generate
					// fatal errors too. This will cause a bizarre effect
					// if the message box about to be displayed is moved.

		E.qt_fatal_error();	// Display a message box and abort.
	}

	//bitBlt(this,0,0,backing_store);


   //     p.drawText( 100,100, "hohohoho" );
}

//QSizePolicy Sample_Viewer::sizePolicy() const {


//}

void Sample_Viewer::set_sample_data(Sample_Data *p_sample_Data) {

	sample_data=p_sample_Data;
        recompute_pending=true;
	zoom=1; //1:1
	offset=0;

	update();
}


void Sample_Viewer::update_position_list_display() {

	if (recompute_pending) {
		initialize_sample_cache();
		update_sample_cache(0,sample_data->get_size()-1);
		recompute_pending=false;
	}

	if (!isVisible())
		return; //go back if hidden
	if (!sample_data->get_size())
		return; //no sample, no cookie

	QPainter painter;
	painter.begin(this);
	painter.setPen(QColor(0,0,200));

	int factor=get_factor();

	int drawn=0;

	for (int i=0;i<(int)position_display_cache.size();i++) {
		if (position_display_cache[i] || old_position_display_cache[i]) {
			draw_screen_pos(i,painter,factor);
			drawn++;
		}
	}

	if (drawn)
		old_position_display_cache=position_display_cache;

}

void Sample_Viewer::set_position_list(const PositionList& p_list) {
	if ((int)position_display_cache.size()!=width()) {
		position_display_cache.resize(width());
		old_position_display_cache.resize(width());
	}
	if (!isVisible())
		return; //go back if hidden
	if (!sample_data->get_size())
		return; //no sample, no cookie

        float screen_size=(float)sample_data->get_size()*zoom;

	for (int i=0;i<(int)position_display_cache.size();i++) {
		position_display_cache[i]=false;
	}
	for (int i=0;i<(int)p_list.size();i++) {
		if ( (p_list[i]<offset) || ((float)p_list[i]>((float)offset+screen_size)))
			continue; //cant plot this;
		float pos=p_list[i];
                pos-=(float)offset;
		pos=pos*(float)width()/screen_size;
		if (pos>=position_display_cache.size()) {
			//printf("Error? how can this happen?\n");
		} else {
			position_display_cache[(int)pos]=true;
		}
		//screen_pos+=offset;
	}
}



Sample_Viewer::Sample_Viewer( QWidget *p_parent) : QWidget( p_parent,"Sample Viewer" ) {
	fatal_error=false;
	sample_data=NULL;
	zoom=1; //1:1
	offset=0;
	sample_cache_target=40;
	max_sample_cache_size=200*1024;
	backing_store_width=backing_store_height=-1;
	backing_store=NULL;
	setBackgroundMode (Qt::NoBackground);
	recompute_pending=true;
	selection=NULL;
	grabbing_mouse=false;
	init_colormap();
	grab_type=SET_SELECTION;
}


Sample_Viewer::~Sample_Viewer()
{
	if (backing_store)
		delete backing_store;
}
