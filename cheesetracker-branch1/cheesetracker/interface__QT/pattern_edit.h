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

#ifndef PATTERN_EDIT_H
#define PATTERN_EDIT_H

#include "interface_binds/editor.h"
#include "trackercore/song.h"
#include "trackercore/player_data.h"
#include "common/components/data/property_bridges.h"

#include <qwidget.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QEvent>
#include <QPaintEvent>

#include "os/mutex_lock.h"



/**
 *
 * Juan Linietsky
 **/
class Pattern_Edit : public QWidget {
	Q_OBJECT
public:
	enum Colormap {

		Col_BackGround,
		Col_ForeGround,
		Col_Cursor,
		Col_Row_Hilite_Minor,
		Col_Row_Hilite_Major,
		Col_BackGround_Selected,
		Col_Row_Hilite_Selected,
		Col_ForeGround_Selected,
		Col_SepChannels,
		Col_SepNewChannels,
		Col_Mask,
		Col_ForeGround_Inactive,
		Col_Play_Position,
		Max_Colors
	};

private:

	static int volume_scale_memory;
	static int color_values[Max_Colors*3];
	static QColor colors[Max_Colors];
	static bool colors_initialized;

        static QFont font;
	static bool font_initialized;
	QFontMetrics font_metrics;

	int old_row_to_draw;

	// To accommodate Qt4's requirement that
	// all update_info_areas's code be moved to
	// paintEvent:

	int p_row_being_played;
	int p_pattern_being_played;

	Song *song;
	Editor *editor;

        void paintEvent( QPaintEvent * p_event );

        /* misc methods */
	void adjust_editor(QPainter &painter);
	void print_single_row(QPainter& painter,int rowofs);
	void print_rows(QPainter& painter);
        void print_number(QPainter& painter,int rowofs,bool hilite_number,bool mark_number);
	void print_cursor(QPainter& painter);
        void print_top(QPainter& painter);
//        void print_osciloscopes();
	bool event(QEvent *e);
	void repaint_after_command();

	void screen_to_pattern_coords(int &x,int &y,int &field, bool &outside);

	bool previous_shift;


	struct Mouse_Select {
		int begin_x,begin_y;
		bool begun;
		bool active;

	} mouse_select;

	Mutex_Lock *variables_lock;
	Player_Data *player;

	QPixmap *backing_store;
	QPainter *backing_store_painter;

	static bool use_backing_store;

signals:

	void instrument_changed_signal(int p_new_one);
	void operation_performed_signal();

public:

	static Bool_Property_Bridge backing_store_bridge;

	void set_variables_lock(Mutex_Lock *p_variables_lock);

	static void allocate_colormap();
	static void allocate_font();
	static QColor& get_color(Colormap p_which);
	static QFont *get_font();

	void update_info_areas(int p_row_being_played,int p_pattern_being_played);
	void configure(Song *p_song,Editor *p_editor,Player_Data *p_player);
	Pattern_Edit(QWidget *p_parent);
	~Pattern_Edit();
};

#endif
