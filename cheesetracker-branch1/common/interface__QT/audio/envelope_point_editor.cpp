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
#include "envelope_point_editor.h"
#include <qpainter.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEvent>
#include <math.h>


static const float MIN_ZOOM=0.2;
static const float ZOOM_ADJUST_MARGIN_MIN=30.0; //percent of screen
static const float ZOOM_ADJUST_MARGIN_MAX=5.0; //percent of screen

bool Envelope_Point_Editor::adjust_zoom_to_window() {

	float cur_width=width();
	float env_len=envelope->get_node_offset(envelope->get_node_count()-1);

	float adjust_margin_min=ZOOM_ADJUST_MARGIN_MIN*cur_width/100.0;
	float adjust_margin_max=ZOOM_ADJUST_MARGIN_MAX*cur_width/100.0;

	bool changed=false;

	float old_zoom=zoom;

        if ( (env_len/zoom) < (cur_width-adjust_margin_min) ) {

        	zoom=env_len/(cur_width-adjust_margin_max);
	}

        if ( (env_len/zoom) > (cur_width-adjust_margin_max) ) {

        	zoom=env_len/(cur_width-adjust_margin_min);
	}


	if (zoom<MIN_ZOOM) {
		zoom=MIN_ZOOM;
	}

	if (zoom!=old_zoom)
		changed=true;

	return changed;

}

void Envelope_Point_Editor::move_mouse_to_selected_node() {

//	int cur_width=width();
	int cur_height=height()-font_height_spacing;
//	int real_height=height();

	if (cur_height<=0)
		return;

        float env_normalize=-envelope->get_min_value();
	float env_total=envelope->get_max_value()-envelope->get_min_value();
	if (env_total==0)
		return; //canut be draw, broken envelope
	int center=int(env_normalize*(float)cur_height/env_total);
	center=cur_height-center;


	float node_x=(float)envelope->get_node_offset(grab.node);
	float node_y=(float)envelope->get_node_height(grab.node);

	node_x/=zoom;
	node_y+=env_normalize;
	node_y=node_y*(float)cur_height/env_total;
	node_y=cur_height-node_y;

	QCursor::setPos(mapToGlobal(QPoint((int)node_x,(int)node_y)));
}


bool Envelope_Point_Editor::event ( QEvent * e ) {

	if (envelope==NULL) return true;

//	int cur_width=width();
	int cur_height=height()-font_height_spacing;
//	int real_height=height();

	if (cur_height<=0)
		return true;

        float env_normalize=-envelope->get_min_value();
	float env_total=envelope->get_max_value()-envelope->get_min_value();
	if (env_total==0)
		return false; //canut be draw, broken envelope
	int center=int(env_normalize*(float)cur_height/env_total);
	center=cur_height-center;


	if (QEvent::MouseMove == e->type()) {
		QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);

		if (grab.moving_point) {

			float gx=mouseEvent->x(),gy=mouseEvent->y();
			gx*=zoom;

			gy=float(cur_height-gy)*env_total/(float)cur_height;
			gy-=env_normalize;

			if ( ((int)gx!=envelope->get_node_offset(grab.node)) || ((int)gy!=envelope->get_node_height(grab.node)) ) {

				//avoid pointless overdraw
				if (envelope_lock) envelope_lock->grab(__FILE__, __LINE__);
				envelope->set_node_offset(grab.node,(int)gx,(int)gy);
				if (envelope_lock) envelope_lock->release();
				if (adjust_zoom_to_window())
					move_mouse_to_selected_node();
				update();
			}
			//must setup new coordinates
		}
	}

	if (QEvent::MouseButtonRelease == e->type()) {
		QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);

		if (grab.moving_point && (mouseEvent->button()==Qt::LeftButton)) {

			grab.moving_point=false;
			releaseMouse();
			update();
		}
	}

	if (QEvent::MouseButtonPress == e->type()) {

		QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);

		float gx=mouseEvent->x(),gy=mouseEvent->y();

		// check if we grabbed anything.. distance in pixels
		int closest_node=-1;
		float closest_node_dist=1000;
		for (int i=0;i<envelope->get_node_count();i++) {


			float node_x,node_y;

			node_x=(float)envelope->get_node_offset(i);
			node_y=(float)envelope->get_node_height(i);

			node_x/=zoom;
			node_y+=env_normalize;
			node_y=node_y*(float)cur_height/env_total;
			node_y=cur_height-node_y;

			float distance=sqrt(pow(node_x-gx,2)+pow(node_y-gy,2));

			if ( (distance<closest_node_dist) && (distance<(float)MIN_DRAG_DISTANCE)) {

				closest_node_dist=distance;
				closest_node=i;
			}

		}

		switch (mouseEvent->button()) {

			case Qt::LeftButton: {

				// check the fish
				if (closest_node!=-1) {

					//convert to local coords

					grab.moving_point=true;
					grab.node=closest_node;
					grab.x=envelope->get_node_offset(grab.node);
					grab.y=envelope->get_node_height(grab.node);
					grabMouse();
					move_mouse_to_selected_node();
				} else { //couldnt find close node, adding the point!


					gx*=zoom; // to env coordinates
					gy=(cur_height-gy)*env_total/(float)cur_height;
					gy-=env_normalize;


					if (envelope_lock) envelope_lock->grab(__FILE__, __LINE__);
                                	int which=envelope->add_node_at_offset((int)gx,(int)gy);
					if (envelope_lock) envelope_lock->release();

					if (which>=0) {
						grab.moving_point=true;

						grab.node=which;
						grab.x=(int)gx;
						grab.y=(int)gy;
						grabMouse();
						move_mouse_to_selected_node();
						update();
						 envelopeChanged();

					}
				}

			} break;
			case Qt::RightButton: {

        			if (closest_node!=-1) {

					if (envelope_lock) envelope_lock->grab(__FILE__, __LINE__);
        				envelope->delete_node(closest_node);
					if (envelope_lock) envelope_lock->release();
					update();
					 envelopeChanged();
					adjust_zoom_to_window();
				}
			} break;
			default: {}
		}


	}

	QWidget::event(e);
	return true;
}


void Envelope_Point_Editor::set_envelope(Envelope *p_envelope) {

	envelope=p_envelope;
	adjust_zoom_to_window();
	recalc_pending=true;
	update();
}


void Envelope_Point_Editor::paint_envelope_points(QPainter &painter) {

	int cur_height=height()-font_height_spacing;
	int cur_width=width();
	int real_height=height();


	if (cur_height<=0)
		return;

        float env_normalize=-envelope->get_min_value();
	float env_total=envelope->get_max_value()-envelope->get_min_value();

	if (envelope->is_loop_enabled()) {

		float node_begin,node_end;

		node_begin=envelope->get_node_offset( envelope->get_loop_begin() )/zoom;
		node_end=envelope->get_node_offset( envelope->get_loop_end() )/zoom;

		painter.setPen(QColor(170,170,170));

        	painter.drawLine((int)node_begin,0,(int)node_begin,cur_height);
        	painter.drawLine((int)node_end,0,(int)node_end,cur_height);
	}

	if (envelope->is_sustain_loop_enabled()) {

		float node_begin,node_end;

		node_begin=envelope->get_node_offset( envelope->get_sustain_loop_begin() )/zoom;
		node_end=envelope->get_node_offset( envelope->get_sustain_loop_end() )/zoom;

		painter.setPen(QColor(220,220,220));

        	painter.drawLine((int)node_begin,0,(int)node_begin,cur_height);
        	painter.drawLine((int)node_end,0,(int)node_end,cur_height);
	}

	painter.fillRect(0,cur_height,cur_width,real_height-cur_height,QColor(0,0,0));

	painter.setPen(QColor(200,200,200));

	for (int i=0;i<envelope->get_node_count();i++) {


		float node_x,node_y;

		node_x=(float)envelope->get_node_offset(i)/zoom;

		node_y=(float)envelope->get_node_height(i);
		node_y+=env_normalize;
		node_y=node_y*(float)cur_height/env_total;
		node_y=cur_height-node_y;

		painter.drawRect((int)node_x-2,(int)node_y-2,6,6);
	}

	painter.setFont(display_font);
	const QFontMetrics &display_font_metrics=painter.fontMetrics();

	char buf[200];
	buf[199]=0;

//	painter.setPen(QColor(100, 200, 100));

	/* This code calculates the increment for displaying the numbers, so they dont overlap */
	float avg_string_width=(int)display_font_metrics.width("xxxxx");

	float increment=1.0;
	int step=0;

	while ((increment/zoom)<avg_string_width) {


		if (step % 2)
			increment*=2;
		else
			increment*=5;
		step++;

	}

        for (float i=0.0f;i<(cur_width*zoom);i+=increment) {

		int real_screen_offset=int((float)i/zoom);

		snprintf(buf, sizeof(buf)-1,"%i",(int)i);
		if (i>0.0f)
			painter.drawText(real_screen_offset-display_font_metrics.width(buf)/2,cur_height+display_font_metrics.height(),buf);
	}



	if (grab.moving_point) {

		snprintf(buf, sizeof(buf)-1,"Zoom %i%%, %i nodes. Moving node %i, pos: %i,%i",int(zoom*100.0),envelope->get_node_count(),grab.node,envelope->get_node_offset(grab.node),envelope->get_node_height(grab.node));
	} else {

		snprintf(buf, sizeof(buf)-1,"Zoom %i%%, %i nodes. ",int(zoom*100.0),envelope->get_node_count());
	}


	painter.drawText(2,cur_height+3+display_font_metrics.height()*2,buf);

}

void Envelope_Point_Editor::draw_envelope_pos(int p_x,QPainter &painter) {

//	int cur_width=width();
	int cur_height=height()-font_height_spacing-1;
//	int real_height=height();

	if (cur_height<=0)
		return;

        float env_normalize=-envelope->get_min_value();
	float env_total=envelope->get_max_value()-envelope->get_min_value();
	if (env_total==0)
		return; //canut be draw, broken envelope
	int center=int(env_normalize*(float)cur_height/env_total);
	center=cur_height-center;

	if (position_cache[p_x]) {

		painter.setPen(QColor(255,255,255));

	} else {

		painter.setPen(QColor(0,0,0));
	}

	painter.drawLine(p_x,0,p_x,cur_height);


	int real_x_offset=int(p_x*zoom);
	int prev_real_x_offset=int((p_x-1)*zoom);

	int line_size=1;


	if (real_x_offset!=prev_real_x_offset) {

		if ((real_x_offset % 5)==0) {

			line_size=3;
		}

		if ((real_x_offset % 10)==0) {

			line_size=5;
		}

		if ((real_x_offset % 50)==0) {

			line_size=7;
		}

		if ((real_x_offset % 100)==0) {

			line_size=9;
		}

		if ((real_x_offset % 500)==0) {

			line_size=11;
		}
	}

	painter.setPen(QColor(200,200,255));
	int end=center+(line_size/2);

	if (end>=(cur_height))
		end=cur_height;

	painter.drawLine(p_x,center-(line_size/2),p_x,end);


	float val=envelope->get_height_at_pos((float)p_x*zoom);

	if (p_x*zoom>envelope->get_node_offset(envelope->get_node_count()-1))
		return; //dont draw what it's pointless to draw

	val+=env_normalize;
	val=val*(float)cur_height/env_total;
	val=cur_height-val;

	float prev_val=envelope->get_height_at_pos((float)(p_x-1)*zoom);
	prev_val+=env_normalize;
	prev_val=prev_val*(float)cur_height/env_total;
	prev_val=cur_height-prev_val;


	painter.setPen(QColor(140,80,80));
	#define MIN(m_a,m_b) ( ((m_a)<(m_b))?(m_a):(m_b))
	#define MAX(m_a,m_b) ( ((m_a)>(m_b))?(m_a):(m_b))
	painter.drawLine(p_x,(int)MIN(prev_val,val)-1,p_x,(int)MAX(prev_val,val)+1); //must make it better


	painter.setPen(QColor(255,150,150));
	painter.drawLine(p_x,(int)prev_val,p_x,(int)val); //must make it better

}

void Envelope_Point_Editor::paintEvent( QPaintEvent * p_event ) {


	if (envelope==NULL) return;

	if (recalc_pending) {
		adjust_zoom_to_window();
		recalc_pending=false;
	}

	if ( (int)position_cache.size()!=width() ){

		position_cache.resize(width());
		old_position_cache.resize(width());
	}

	QPainter painter;
	painter.begin(this );


	painter.setPen(QColor(0,0,200));



	for (int i=0;i<width();i++) {

		draw_envelope_pos(i,painter);
	}

	paint_envelope_points(painter);


}

// FIXME: Put this into a QPaintEvent somehow!!!

void Envelope_Point_Editor::update_position_list(const vector<int>& p_positions)
{
	if (!this->isVisible())
		return; //do nothing if not visible

	if ( (int)position_cache.size()!=width() ){

		position_cache.resize(width());
		old_position_cache.resize(width());
	}


	for (int i=0;i<(int)position_cache.size();i++) {

		position_cache[i]=false;
	}

	for (int i=0;i<(int)p_positions.size();i++) {

		float pos=p_positions[i];
		pos/=zoom;
                if ((int)pos>=width())
			continue;

		position_cache[(int)pos]=true;
	}

	this->repaint();

	old_position_cache=position_cache;

}



Envelope_Point_Editor::Envelope_Point_Editor(QWidget *p_parent) : QWidget(p_parent) {

	setBackgroundMode (Qt::NoBackground);
	envelope=NULL;
	zoom=0.25f;

	display_font.setFamily("fixed");
	display_font.setPointSize(10);
	display_font.setBold(true);
	display_font.setStyleHint(QFont::Courier,QFont::NoAntialias);


	QFontMetrics display_font_metrics(display_font);
	font_height_spacing=display_font_metrics.height()*2+6;

	grab.moving_point=false;
	grab.node=-1;
	grab.x=0;
	grab.y=0;
	recalc_pending=true;
	envelope_lock=NULL;
}

Envelope_Point_Editor::~Envelope_Point_Editor()
{
}
