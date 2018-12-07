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
#include "sample_instrument_table.h"
#include <math.h> //thanks mrs brisby!
#include <qtooltip.h>
//Added by qt3to4:
#include <QEvent>
#include <QKeyEvent>
#include <QPaintEvent>


void Sample_Instrument_Table::draw_single_mode() {
	this->repaint();
}
void Sample_Instrument_Table::paintEvent( QPaintEvent * p_event ) {
	int i;


	int rows_to_draw;

      	if ((editor==NULL) || (instrument==NULL)) return;

	QPainter painter(this);
        painter.setFont(font);
	const QFontMetrics &aux_metrics=painter.fontMetrics();
	int font_width=aux_metrics.width("X");
	int font_height=aux_metrics.height();
	setMinimumWidth(font_width*LINE_CHAR_WIDTH); //why here? QT docs says it's more accurate to get font info here :(
	char buf[20];
	buf[19]=0;

	rows_to_draw=height()/font_height;

	if (cursor_y<row_offset) row_offset=cursor_y;
	if (cursor_y>=(row_offset+rows_to_draw)) row_offset=(cursor_y-rows_to_draw)+1;

	for (i=0;i<rows_to_draw;i++) {

		if ((i+row_offset)<Note::NOTES) {

			string left_note;
			string right_note;
			char instr_number[3]={'.','.',0};

			if (instrument->sample_number[i+row_offset]!=EMPTY_FIELD) {

				instr_number[0]='0'+(instrument->sample_number[i+row_offset]+1)/10;
				instr_number[1]='0'+(instrument->sample_number[i+row_offset]+1) % 10;
			}

			left_note=editor->get_single_note_string(i+row_offset);
			right_note=editor->get_single_note_string(instrument->note_number[i+row_offset]);

//                        tmpord=song->variables->get_cstring_order(i+row_offset);

//			aux=song->get_order(i+row_offset);


			painter.fillRect(0,font_height*i,width(),font_height*i+font_height,colors[Col_BackGround]);

			painter.setPen(QColor(colors[Col_Separator]));
			painter.drawText(font_width,font_height*i+font_height,left_note.c_str());
			painter.setPen(QColor(colors[Col_ForeGround]));
			snprintf(buf,sizeof(buf)-1,"%s %s",right_note.c_str(),instr_number);
			painter.drawText(font_width*5,font_height*i+font_height,buf);

		}

	}

	if (hasFocus()) {


		painter.setPen(QColor(colors[Col_Cursor]));
		painter.drawRect(0,0,width()-1,height()-1);

		int initx,inity;
		const int cursor_spacing[4]={0,2,4,5};

		initx=font_width*5+cursor_spacing[cursor_x]*font_width;
		inity=font_height*(cursor_y-row_offset);

		painter.drawRect(initx-1,inity-1,font_width+2,font_height+2);
	}
}

void Sample_Instrument_Table::MoveUp() {

        if (cursor_y>0) cursor_y--;

}

void Sample_Instrument_Table::MoveDown() {

        if (cursor_y<Note::NOTES-1) cursor_y++;

}

void Sample_Instrument_Table::MoveLeft() {

        if (cursor_x>0) cursor_x--;

}

void Sample_Instrument_Table::MoveRight() {

        if (cursor_x<3) cursor_x++;

}

void Sample_Instrument_Table::PageUp() {

       cursor_y-=8;
       if (cursor_y<0) cursor_y=0;
}

bool Sample_Instrument_Table::event(QEvent *e) {

	if ( (e->type()==QEvent::FocusIn) || (e->type()==QEvent::FocusOut)) {

		QWidget::event(e);
		update();
		return true;
	}
	if (e->type()!=QEvent::KeyPress) {

//		QWidget::event(e);
		return QWidget::event(e);
	}

	QKeyEvent *event=(QKeyEvent*)e;



	int key_char=event->ascii();
	if ((key_char>='a') && (key_char<='z')) key_char-=('a'-'A');


	if (event->state()&Qt::AltButton) {

		switch(key_char) {
			case 'Q': {

				for (int i=0;i<Note::NOTES;i++) {
					if (instrument->note_number[i]>=Note::NOTES)
						break;
					instrument->note_number[i]++;
					if (instrument->note_number[i]>=Note::NOTES)
						instrument->note_number[i]=EMPTY_FIELD;
				}
			} break;
			case 'A': {

				for (int i=0;i<Note::NOTES;i++) {
					if (instrument->note_number[i]>=Note::NOTES)
						break;
					if (instrument->note_number[i]==0)
						break;
					instrument->note_number[i]--;
					if (instrument->note_number[i]>=Note::NOTES)
						instrument->note_number[i]=EMPTY_FIELD;
				}
			} break;
			case 'S': {

				for (int i=0;i<Note::NOTES;i++) {
					instrument->sample_number[i]=sample_mask;
				}
			} break;
		}

	} else {
		InsertValue(key_char);
	}

	bool capture=false;
	switch (event->key()) {

		case Qt::Key_Up: {

			MoveUp();
			capture=true;
		} break;
		case Qt::Key_Down: {

			MoveDown();
			capture=true;

		} break;
		case Qt::Key_Left: {

			MoveLeft();
			capture=true;

		} break;
		case Qt::Key_Right: {

			MoveRight();
			capture=true;

		} break;
		case Qt::Key_PageUp: {

			PageUp();
			capture=true;

		} break;
		case Qt::Key_PageDown: {

			PageDown();
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


void Sample_Instrument_Table::PageDown() {

       cursor_y+=8;
       if (cursor_y>=Note::NOTES) cursor_y=Note::NOTES-1;
}


void Sample_Instrument_Table::AddSeparator() {

//	song->variables->orderlist[cursor_y]=253;
//	song->set_order(cursor_y,253);
	MoveDown();
}

void Sample_Instrument_Table::ClearOrder() {

	if (cursor_x>1) instrument->sample_number[cursor_y]=EMPTY_FIELD;
	sample_mask=EMPTY_FIELD;
	MoveDown();
}

void Sample_Instrument_Table::InsertValue(int number) {

	switch (toupper(number)) {
		case 'R': {

			if (cursor_x==0)
				break;
			instrument->sample_number[cursor_y]=sample_mask;
			MoveDown();

		} break;
		case '.': {

			ClearOrder();
		} break;
	}

	switch (cursor_x) {
		case 0: {

			Uint8 tmpkey = editor->get_note_from_key(number);
			if (tmpkey!=EMPTY_FIELD) {

				instrument->note_number[cursor_y]=tmpkey;
				instrument->sample_number[cursor_y]=sample_mask;

				MoveDown();
			}
		} break;
		case 1: {

			if ((number>='0') && (number<='9')) {

				int aux_numb=instrument->note_number[cursor_y] % 12;

				instrument->note_number[cursor_y]=(number-'0')*12+aux_numb;
				MoveDown();
			}
		} break;
		case 2: {

			if ((number>='0') && (number<='9')) {

				int aux_numb=instrument->sample_number[cursor_y];
                                int num_right;
				if (aux_numb==EMPTY_FIELD) aux_numb=0;
				else aux_numb++;

				num_right=aux_numb % 10;

                                aux_numb=(number-'0')*10+num_right;

				if (aux_numb==0) aux_numb=EMPTY_FIELD;
				else aux_numb--;

				sample_mask=aux_numb;
				instrument->sample_number[cursor_y]=aux_numb;
				MoveRight();
			}
		} break;
		case 3: {

			if ((number>='0') && (number<='9')) {

				int aux_numb=instrument->sample_number[cursor_y];
                                int num_left;
				if (aux_numb==EMPTY_FIELD) aux_numb=0;
				else aux_numb++;

				num_left=aux_numb / 10;

                                aux_numb=num_left*10+(number-'0');

				if (aux_numb==0) aux_numb=EMPTY_FIELD;
				else aux_numb--;

				sample_mask=aux_numb;
				instrument->sample_number[cursor_y]=aux_numb;
				MoveLeft();
				MoveDown();
			}
		} break;

	}

}

void Sample_Instrument_Table::InsertBlank() {



	if (cursor_x>1) instrument->sample_number[cursor_y]=EMPTY_FIELD;
/*
	for (i=NOTE_NOTES-1;i>cursor_y;i--) {

		song->set_order(i,song->get_order(i-1));
	}
	song->set_order(cursor_y,255);
*/
}

void Sample_Instrument_Table::DeleteRow() {


  /*
	for (i=cursor_y;i<NOTE_NOTES-1;i++) {

		song->set_order(i,song->get_order(i+1));
	}
	song->set_order(NOTE_NOTES-1,255);
*/
}

void Sample_Instrument_Table::InsertRepeat() {

//	song->set_order(cursor_y,254);
	MoveDown();
}


void Sample_Instrument_Table::link_to_instrument_layer(Instrument::Layer *p_instrument) {

	instrument=p_instrument;

	update();

}

void Sample_Instrument_Table::link_to_editor(Editor *p_editor) {

	editor=p_editor;
}

Sample_Instrument_Table::Sample_Instrument_Table(QWidget *p_widget) : Table_Base(p_widget) {

	row_offset=0;
	cursor_x=0;
	cursor_y=0;
	editor=NULL;
	instrument=NULL;
	allocate_colormap();

	setBackgroundMode (Qt::NoBackground);
	setFocusPolicy(Qt::StrongFocus);
	QToolTip::add( this, "Shorcuts:\n\n'.' - Clear field\nR - Repeat last field.\nAlt+Q Raise all notes a semitone.\nAlt+A Lower all notes a semitone.\nAlt+S Set all samples to last entered one.");

}

Sample_Instrument_Table::~Sample_Instrument_Table(){
}
