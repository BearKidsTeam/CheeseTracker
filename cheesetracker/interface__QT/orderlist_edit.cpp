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

#include "orderlist_edit.h"
//Added by qt3to4:
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>


void OrderList_Edit::draw_single_mode() {
	this->repaint();
}
void OrderList_Edit::paintEvent( QPaintEvent * p_event ) {
	int i;
       	char tmpnumb[4]={'0','0','0',0};

      	if ((editor==NULL)) return;

	QPainter painter(this);
        painter.setFont(font);
	const QFontMetrics &aux_metrics=painter.fontMetrics();
	int font_width=aux_metrics.width("X");
	int font_height=aux_metrics.height();

	cache_font_height=font_height;
	cache_font_width=font_width;

	setMinimumWidth(font_width*9); //why here? QT docs says it's more accurate to get font info here :(
//	char buf[20];


	int rows_to_draw;
	rows_to_draw=height()/font_height;

	if (editor->orderlist_get_cursor_y()<row_offset) row_offset=editor->orderlist_get_cursor_y();
	if (editor->orderlist_get_cursor_y()>=(row_offset+rows_to_draw)) row_offset=(editor->orderlist_get_cursor_y()-rows_to_draw)+1;

	for (i=0;i<rows_to_draw;i++) {

		painter.fillRect(0,font_height*i,width(),font_height,colors[Col_BackGround]);

		if ((i+row_offset)<MAX_ORDERS) {
		    	tmpnumb[0]='0'+(i+row_offset)/100;
		     	tmpnumb[1]='0'+((i+row_offset)/10) % 10;
		     	tmpnumb[2]='0'+(i+row_offset) % 10;

			if (current_playing_order==(i+row_offset)) {
				painter.setPen(colors[Col_BeingPlayed]);
			} else {
				painter.setPen(colors[Col_Separator]);
			}
			painter.drawText(font_width,font_height*i+font_height,tmpnumb);
			painter.setPen(colors[Col_ForeGround]);
			painter.drawText(font_width*5,font_height*i+font_height,editor->orderlist_get_cursor_string(i+row_offset).c_str());
		}

	}

	//clear last part
	painter.fillRect(0,font_height*rows_to_draw,width(),font_height,colors[Col_BackGround]);

 //       painter.fillRect(
//	window.draw_rectangle(sepcol,TRUE,13+font_width*3,0,3,height());
	if (hasFocus()) {

		painter.setPen(colors[Col_Cursor]);
		painter.drawRect(0,0,width()-1,height()-1);

		int initx,inity;

		initx=font_width*4+editor->orderlist_get_cursor_x()*font_width;
		inity=font_height*(editor->orderlist_get_cursor_y()-row_offset);

		painter.drawRect(font_width+initx-1,inity-1,font_width+2,font_height+2);

	}
}
bool OrderList_Edit::event(QEvent *e) {

	if ( (e->type()==QEvent::FocusIn) || (e->type()==QEvent::FocusOut)) {

		update();
		return QWidget::event(e);;
	}
	if ( (e->type()==QEvent::MouseButtonPress) ) {


		QMouseEvent *me=(QMouseEvent*)e;
		//printf("mouse event at %i,%i\n",me->x(),me->y());
		int new_row=me->y();
		new_row/=cache_font_height;
		new_row+=row_offset;
		if (new_row>=MAX_ORDERS) {

			new_row=MAX_ORDERS-1;
		}

		editor->orderlist_set_cursor_y(new_row);

		switch( me->x() / cache_font_width ) {

			case 5: {
				editor->orderlist_set_cursor_x(0);
			} break;
			case 6: {
				editor->orderlist_set_cursor_x(1);
			} break;
			case 7: {
				editor->orderlist_set_cursor_x(2);
			} break;
		};

		update();
		return QWidget::event(e);
	}
	if (e->type()!=QEvent::KeyPress) {


		return QWidget::event(e);
	}

	QKeyEvent *event=(QKeyEvent*)e;

	int key_char=event->ascii();
	if ((key_char>='a') && (key_char<='z')) key_char-=('a'-'A');



	switch (key_char) {


                case '=':
		case '+': {
			editor->orderlist_add_separator();
		} break;
		case '.': {
			editor->orderlist_clear_order();
		} break;
		default: {

			if ((key_char>='0') && (key_char<='9'))
				editor->orderlist_insert_value(key_char-'0');
		} break;
	};

	bool capture=false;
	switch (event->key()) {

		case Qt::Key_Insert: {

			editor->orderlist_insert();
		} break;
		case Qt::Key_Delete: {

			editor->orderlist_delete();
		} break;
		case Qt::Key_Up: {

			editor->orderlist_move_up();
			capture=true;
		} break;
		case Qt::Key_Down: {

			editor->orderlist_move_down();
			capture=true;

		} break;
		case Qt::Key_Left: {

			editor->orderlist_move_left();
			capture=true;

		} break;
		case Qt::Key_Right: {

			editor->orderlist_move_right();
			capture=true;

		} break;
		case Qt::Key_PageUp: {

			editor->orderlist_page_up();
			capture=true;

		} break;
		case Qt::Key_PageDown: {

			editor->orderlist_page_down();
			capture=true;
		} break;
	}

	update();
	if (capture) {

		event->accept();
		return false;
	} else {

		return QWidget::event(e);

	}
}

void OrderList_Edit::set_current_playing_order(int p_order) {

	if (current_playing_order!=p_order) {

		update();
		current_playing_order=p_order;
	}
}


OrderList_Edit::OrderList_Edit(QWidget *p_parent) : Table_Base(p_parent) {

        setBackgroundMode (Qt::NoBackground);
	setFocusPolicy(Qt::StrongFocus);

	row_offset=0;
	editor=NULL;
	allocate_colormap ();
	current_playing_order=-1;
}

OrderList_Edit::~OrderList_Edit(){
}
