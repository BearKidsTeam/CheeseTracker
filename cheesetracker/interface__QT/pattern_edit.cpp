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
#include "pattern_edit.h"
//Added by qt3to4:
#include <QPaintEvent>
#include <QPixmap>




//#define SEP_OFFSET (-5)
#define SEP_OFFSET 0
#define VOLUME_OVERDRIVE_LIMIT 270

int Pattern_Edit::volume_scale_memory=100; //default val
bool Pattern_Edit::colors_initialized=false;
bool Pattern_Edit::font_initialized=false;
bool Pattern_Edit::use_backing_store=false;
Bool_Property_Bridge Pattern_Edit::backing_store_bridge("Pattern Editor Backing Store:",&use_backing_store);
QFont Pattern_Edit::font;

void Pattern_Edit::set_variables_lock(Mutex_Lock *p_variables_lock) {

	variables_lock=p_variables_lock;
}


QColor Pattern_Edit::colors[Pattern_Edit::Max_Colors];
int Pattern_Edit::color_values[Pattern_Edit::Max_Colors*3] = {
    10, 20, 30,
    100, 200, 100,
    250, 250, 250,
    50, 60, 70,
    70, 80, 90,
    10, 50, 10,
    15, 80, 15,
    250, 250, 250,
    150, 150, 150,
    210, 210, 210,
    250, 100, 100,
    70, 110, 70,
    30,50,250

};

QColor& Pattern_Edit::get_color(Colormap p_which) {

	if (p_which==Max_Colors)
		return colors[Max_Colors-1];
	else
		return colors[p_which];
}


QFont *Pattern_Edit::get_font() {

	return &font;
}

void Pattern_Edit::allocate_colormap () {

	if (colors_initialized)
		return;

	int i;

	for (i=0;i<Max_Colors*3;i+=3) {

		colors[i/3].setRgb(color_values[i],color_values[i+1],color_values[i+2]);
	}
	colors_initialized=true;
}

void Pattern_Edit::allocate_font () {

	if (font_initialized)
		return;

#if defined(MACOSX_ENABLED)
	font.setFamily("Monaco CY");
	font.setPointSize(11);
#elseif defined(CYGWIN_ENABLED)

	font.setFamily("Terminal");
	font.setPointSize(10);
#else

	font.setFamily("fixed");
	font.setPointSize(11);
	font.setBold(true);
#endif
	font.setPointSize(11);
	font.setStyleHint(QFont::Courier,QFont::NoAntialias);

	font_initialized=true;
}

void Pattern_Edit::adjust_editor(QPainter &painter) {

	const QFontMetrics &aux_metrics=painter.fontMetrics();
	editor->set_font_width(aux_metrics.width("X"));
	editor->set_font_height(aux_metrics.height());
	editor->set_window_width(width());
	editor->set_window_height(height());
	editor->adjust_view_to_cursor();


}
/*
void Pattern_Edit::print_osciloscopes() {

	int i;

	return;
        if (player==NULL) return;

	window.draw_rectangle(GC[Col_BackGround],TRUE ,0,editor->get_row_height(),editor->left_numbers_width()+SEP_OFFSET,editor->get_row_height());

	for (i=0;i<editor->get_visible_columns();i++) {

		int tmpvoice,pos,j,tmppan,tmpvol,panpos,tmpvolval,tmpvolcol;
		window.draw_rectangle(GC[Col_BackGround],TRUE ,editor->left_numbers_width()+i*editor->get_column_width()+SEP_OFFSET,editor->get_row_height(),editor->get_window_width(),editor->get_row_height());
		tmpvoice=player->get_channel_voice(editor->get_column_offset()+i);

		window.draw_rectangle(GC[Col_SepChannels],TRUE,editor->left_numbers_width()+i*editor->get_column_width()+SEP_OFFSET,editor->get_row_height(),1,editor->get_row_height());
		window.draw_rectangle(GC[Col_SepChannels],TRUE,editor->left_numbers_width()+i*editor->get_column_width()+SEP_OFFSET+5,editor->get_row_height(),1,editor->get_row_height());

                if ((tmpvoice==-1) || (player->get_mixer_ptr()->is_voice_stopped(tmpvoice))) continue;

		pos=player->get_mixer_ptr()->get_voice_sample_pos_index(tmpvoice);
		tmppan=player->get_mixer_ptr()->get_voice_panning_pos(tmpvoice);
		tmpvol=player->get_mixer_ptr()->get_voice_volume(tmpvoice);



		if (tmppan==PAN_SURROUND) {

			panpos=editor->get_font_width();
			window.draw_string(*font,GC[Col_ForeGround_Selected],editor->left_numbers_width()+i*editor->get_column_width()+5,editor->get_row_height()+editor->get_font_height(),"s");
		} else {
               		panpos=5+(tmppan*(editor->get_column_width()-5)/PAN_RIGHT);
			window.draw_rectangle(GC[Col_ForeGround_Selected],TRUE,editor->left_numbers_width()+i*editor->get_column_width()+panpos+SEP_OFFSET,editor->get_row_height(),1,editor->get_row_height());
		}

		if (panpos>(editor->get_column_width()-5)) panpos=editor->get_column_width()-5;

		tmpvolval=(tmpvol*(editor->get_row_height()-1)/VOLUME_OVERDRIVE_LIMIT);

		switch ((tmpvolval*3)/(editor->get_row_height()-1)) {

			case 0: {

				tmpvolcol=Col_SepChannels;
			} break;
			case 1: {

				tmpvolcol=Col_ForeGround;
			} break;
			case 2: {

				tmpvolcol=Col_ForeGround_Selected;
			} break;
        		default: {

				tmpvolcol=Col_Mask;
			} break;
		}



		if (tmpvolval>(editor->get_row_height()-1)) tmpvolval=(editor->get_row_height()-1);

		window.draw_rectangle(GC[tmpvolcol],TRUE,editor->left_numbers_width()+i*editor->get_column_width()+SEP_OFFSET,(editor->get_top_height()-tmpvolval),5,editor->get_top_height()-(editor->get_top_height()-tmpvolval));

		for(j=5;j<editor->get_column_width();j++) {

                 	Sint8* sample_8bits;
                 	Sint16* sample_16bits;
			int size,val;

			if (player->get_mixer_ptr()->get_voice_sample_data_ptr(tmpvoice)==NULL) break;

			sample_8bits=(Sint8*)player->get_mixer_ptr()->get_voice_sample_data_ptr(tmpvoice)->data_ptr;
			sample_16bits=player->get_mixer_ptr()->get_voice_sample_data_ptr(tmpvoice)->data_ptr;
			size=player->get_mixer_ptr()->get_voice_sample_data_ptr(tmpvoice)->size;

			if ((size==0) || (sample_8bits==NULL)) break;

			if (player->get_mixer_ptr()->get_voice_sample_data_ptr(tmpvoice)->is_16bits) {

                                val=(32768+sample_16bits[(pos+j)%size])*(editor->get_row_height()-1)/65535;

			} else {

                                val=(128+sample_8bits[(pos+j)%size])*(editor->get_row_height()-1)/255;
			}

			window.draw_rectangle(GC[(editor->is_channel_muted(editor->get_column_offset()+i))?Col_SepChannels:Col_ForeGround],TRUE,editor->left_numbers_width()+i*editor->get_column_width()+j+SEP_OFFSET,editor->get_row_height()+val,1,1);
		}


	}

}
*/

void Pattern_Edit::print_top(QPainter &painter) {

	char mask_string[5]={'I','V','C','P',0};
	int i;

/*  Print TOP info */

        /*  Clear top */
	painter.fillRect(0,0,editor->get_window_width(),editor->get_row_height()*2,colors[Col_BackGround]);

        /* Edit Mask */
	if (!editor->get_instrument_mask()) mask_string[0]=' ';
	if (!editor->get_volume_mask()) mask_string[1]=' ';
	if (!editor->get_command_mask()) mask_string[2]=' ';
	if (!editor->get_volume_pan_mode()) mask_string[3]=' ';

	painter.setPen(colors[Col_Mask]);
	painter.drawText(editor->get_font_width(),editor->get_row_height(),mask_string);

/* Track Numbers */

	for (i=0;i<editor->get_visible_columns();i++) {

		char track_string[15]={'C','h','a','n','n','e','l',' ','0','0','(','M',')',0};

		int col_offset = editor->get_column_offset()+i;

                track_string[8]='0'+(col_offset+1) / 10;
                track_string[9]='0'+(col_offset+1) % 10;

		if (!editor->is_channel_muted(col_offset)) {

                 	track_string[10]=' ';
                 	track_string[11]=' ';
                 	track_string[12]=' ';
			painter.setPen(colors[Col_SepNewChannels]);
			painter.drawText(editor->left_numbers_width()+i*editor->get_column_width(),editor->get_row_height(),track_string);

		} else {

			painter.setPen(colors[Col_ForeGround_Inactive]);
			painter.drawText(editor->left_numbers_width()+i*editor->get_column_width(),editor->get_row_height(),track_string);

		}




	}
}

void Pattern_Edit::print_number(QPainter &painter,int rowofs,bool hilite_number,bool mark_number) {

	char number_string[5];
	number_string[4]=0;

	if ((rowofs<0) || (rowofs>editor->get_visible_rows())) return;
	number_string[0]='0'+(rowofs+editor->get_row_offset()) / 100;
	number_string[1]='0'+((rowofs+editor->get_row_offset())/10) % 10;
	number_string[2]='0'+(rowofs+editor->get_row_offset()) % 10;
	number_string[3]=mark_number?'<':' ';

/* Left Numbers */

	painter.fillRect(0,editor->get_top_height()+rowofs*editor->get_row_height(),editor->left_numbers_width(),editor->get_row_height(),colors[Col_BackGround]);


	if (hilite_number) {

		painter.setPen(colors[Col_Play_Position]);
		painter.drawText(editor->get_font_width(),editor->get_top_height()+rowofs*editor->get_row_height()+editor->get_font_height(),number_string);
	} else {

		painter.setPen(colors[Col_ForeGround]);
		painter.drawText(editor->get_font_width(),editor->get_top_height()+rowofs*editor->get_row_height()+editor->get_font_height(),number_string);
        }
}

void Pattern_Edit::print_single_row(QPainter &painter,int rowofs) {

	int i;
        char note_string[19];
	int current_row,y_offset;

	note_string[18]=0;

	if ((rowofs<0) || (rowofs>=editor->get_visible_rows()) ) return;

	current_row=rowofs+editor->get_row_offset();

	/* ROW Background - may depend on row hilite */

	y_offset=editor->get_top_height()+rowofs*editor->get_row_height();



	/* Draw notes and separator lines. */

	QPainter *cpainter=&painter;

	for (i=0;i<editor->get_visible_columns();i++) {

		editor->get_note_string(editor->get_column_offset()+i,current_row,(char*)&note_string);

                int x,y;

		int row_x=editor->left_numbers_width()+i*editor->get_column_width();
		int row_y=y_offset;
		x=row_x;
		y=row_y;

		if ((current_row % editor->get_hl_major())==0) {

			cpainter->fillRect(x,y,editor->get_column_width(),editor->get_row_height(),colors[Col_Row_Hilite_Major]);

		} else if ((current_row % editor->get_hl_minor())==0) {

			cpainter->fillRect(x,y,editor->get_column_width(),editor->get_row_height(),colors[Col_Row_Hilite_Minor]);

		} else {

			cpainter->fillRect(x,y,editor->get_column_width(),editor->get_row_height(),colors[Col_BackGround]);
		}


		if ( (editor->is_selection_active())
                     && (editor->get_column_offset()+i>=editor->get_selection_begin_x())
                     && (editor->get_column_offset()+i<=editor->get_selection_end_x())
                     && (editor->get_row_offset()+rowofs>=editor->get_selection_begin_y())
                     && (editor->get_row_offset()+rowofs<=editor->get_selection_end_y())
		    ) {


			if ((current_row % editor->get_hl_major())==0) {

				cpainter->fillRect(x,y,editor->get_column_width()/*-editor->get_font_width()*/,editor->get_row_height(),colors[Col_Row_Hilite_Selected]);
                        } else {

				cpainter->fillRect(x,y,editor->get_column_width()/*-editor->get_font_width() */,editor->get_row_height(),colors[Col_BackGround_Selected]);
			}

			cpainter->setPen(colors[Col_ForeGround_Selected]);
	                cpainter->drawText(x,y+editor->get_font_height(),note_string);

		} else 	{

			cpainter->setPen(colors[Col_ForeGround]);
	                cpainter->drawText(x,y+editor->get_font_height(),note_string);

		}

		if (i!=(editor->get_visible_columns()-1))
			cpainter->fillRect((x+editor->get_column_width()-5),y, 1 ,editor->get_row_height(),colors[Col_SepChannels]);


	}


	/* Draw Numbers */

//	if (editor->is_player_active() && current_row==editor->.get_playing_row()) {

//		print_number(rowofs,true);

//	} else {

	print_number(painter,rowofs,false,editor->is_mark_active() && (current_row==editor->get_marked_row()) && (editor->get_current_pattern()==editor->get_marked_pattern()));
//	}

}



void Pattern_Edit::print_rows(QPainter &painter) {

	int i;

	for (i=0;i<editor->get_visible_rows();i++) print_single_row(painter,i);
	//feel remaining thing
	int begin=editor->left_numbers_width()+editor->get_column_width()*editor->get_visible_columns();

	painter.fillRect(begin,0,width()-begin,height(),colors[Col_BackGround]);
	painter.fillRect(0,(editor->get_visible_rows()+2)*editor->get_row_height(),editor->get_window_width(),height()-((editor->get_visible_rows()+2)*editor->get_row_height()),colors[Col_BackGround]);
}

void Pattern_Edit::print_cursor(QPainter &painter) {

	int xinit,yinit;

        if (!hasFocus()) return; //no focus, no cursor
	const char cursor_spacing[] = {

		0 , //note
		2 , //octave
		4 , //instrument 1
		5 , //instrument 2
		7 , //volume 1
		8 , //volume 2
		10, //command
		11, //parameter 1
		12, //parameter 2
	};

	xinit=editor->left_numbers_width()+(editor->get_cursor_x()-editor->get_column_offset())*editor->get_column_width()+cursor_spacing[editor->get_cursor_field()]*editor->get_font_width();
 	yinit=editor->get_top_height()+(editor->get_cursor_y()-editor->get_row_offset())*editor->get_row_height();
        painter.setPen(colors[Col_Cursor]);
	painter.drawRect(xinit-2, yinit,editor->get_font_width()+4,editor->get_font_height()+3);
}



void Pattern_Edit::paintEvent( QPaintEvent * p_event ) {
	if(editor->flag_update_info_areas()) {
		QPainter painter(this);
		painter.setFont(font);
		bool current_row_marked =editor->is_mark_active() && (p_row_being_played==editor->get_marked_row()) && (editor->get_current_pattern()==editor->get_marked_pattern());
		bool old_row_marked =editor->is_mark_active() && (old_row_to_draw==editor->get_marked_row()) && (editor->get_current_pattern()==editor->get_marked_pattern());

		if (p_pattern_being_played==editor->get_current_pattern()) {

			if (old_row_to_draw!=p_row_being_played) {

				print_number(painter,old_row_to_draw-editor->get_row_offset(),false,old_row_marked);
				print_number(painter,p_row_being_played-editor->get_row_offset(),true,current_row_marked);
			}

			old_row_to_draw=p_row_being_played;

		} else {

			print_number(painter,old_row_to_draw-editor->get_row_offset(),false,old_row_marked);
			old_row_to_draw=-1;
		}
		return;
	}

	if ((editor==NULL) || (song==NULL))
		return;
	int cursor_y_ofs_to_clear;

	QPainter painter(this);
	painter.setFont(font);
	adjust_editor(painter);

	// editor->flag_repaint() is only true during an explicit
	// repaint().

	if ((!editor->flag_repaint()) || editor->flag_redraw_all()) {

		print_top(painter);
		print_rows(painter);
		print_cursor(painter);

	} else {

		if (editor->flag_redraw_row()) {

			cursor_y_ofs_to_clear=editor->get_previous_cursor_y()-editor->get_row_offset();

	                print_single_row(painter,cursor_y_ofs_to_clear-1);
	                print_single_row(painter,cursor_y_ofs_to_clear);
	                print_single_row(painter,cursor_y_ofs_to_clear+1);

	                print_single_row(painter,editor->get_cursor_y()-editor->get_row_offset());
			print_cursor(painter);
		}

		if (editor->flag_redraw_top()) {

                        print_top(painter);
		}

		if (editor->flag_redraw_playing_row()) {

			print_single_row(painter,editor->get_playing_row()-editor->get_row_offset());
			print_single_row(painter,editor->get_old_playing_row()-editor->get_row_offset());

			print_cursor(painter);
		}


	}

}


void Pattern_Edit::repaint_after_command() {
	editor->set_flag_repaint();
	this->repaint();
	editor->set_previous_cursor_y(editor->get_cursor_y());
	editor->set_previous_cursor_x(editor->get_cursor_x());
	editor->clear_redraw_flags();
}

void Pattern_Edit::update_info_areas(int p_row_being_played,int p_pattern_being_played) {

	if (!isVisible()) return;


	if ((editor->is_song_follow() && (p_row_being_played>=0) && (p_pattern_being_played>=0))) {

		editor->set_current_pattern(p_pattern_being_played);

		editor->set_cursor_y(p_row_being_played);

		if (old_row_to_draw!=p_row_being_played)
			this->repaint();
	}
	editor->set_flag_update_info_areas();
	this->p_row_being_played = p_row_being_played;
	this->p_pattern_being_played = p_pattern_being_played;
	this->repaint();
	editor->clear_flag_update_info_areas();
}

void Pattern_Edit::configure(Song *p_song,Editor *p_editor,Player_Data *p_player) {

	song=p_song;
	editor=p_editor;
	player=p_player;
	update();


}

Pattern_Edit::Pattern_Edit(QWidget *p_parent) : QWidget(p_parent),font_metrics(font)
{

	song=NULL;
	editor=NULL;
	old_row_to_draw=-1;

	allocate_colormap();
	allocate_font();


//	font.setFamily("Console8x16.pcf");
	setBackgroundMode (Qt::NoBackground);
	previous_shift=false;
	setFocusPolicy(Qt::StrongFocus);
	variables_lock=NULL;

	mouse_select.begin_y=0;
	mouse_select.begin_x=0;
	mouse_select.begun=false;
	mouse_select.active=false;
	backing_store=NULL;
}

Pattern_Edit::~Pattern_Edit()
{
}
