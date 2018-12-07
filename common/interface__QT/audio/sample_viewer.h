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

#ifndef SAMPLE_VIEWER_H
#define SAMPLE_VIEWER_H

#include <qwidget.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QEvent>
#include <QPaintEvent>
#include "common/components/audio/sample_data.h"
#include "common/components/data/selection.h"
#include <vector>
	using std::vector;

/**
 *
 * Juan Linietsky
 **/
class Sample_Viewer : public QWidget
{
	Q_OBJECT
public:

	enum Colors {

		Col_BackGround,
		Col_ForeGround,
		Col_Center,
		Col_Loop_BackGround,
		Col_Loop_ForeGround,
		Col_Played_Position,

		Col_BackGround_Selected,
		Col_ForeGround_Selected,
		Col_Center_Selected,
		Col_Loop_BackGround_Selected,
		Col_Loop_ForeGround_Selected,
		Col_Played_Position_Selected,

		Col_Max
	};

	typedef vector<int> PositionList;


protected:

	static int color_list[Col_Max*3];
	static QColor colors[Col_Max];
	static bool colors_initialized;
	bool fatal_error; 	 // To prevent further paintEvents while an
	                         // error message is being displayed.

	QPixmap *backing_store;
	int backing_store_width,backing_store_height;

	void initialize_sample_cache();
	void update_sample_cache(size_t p_range_begin,size_t p_range_end);

	int screen_pos_to_sample_pos(int p_pos);

	int get_factor();


	void screen_to_sample(int p_int, float *p_max_peak,float *p_min_peak, int sample_cache_idx,int cur_width,int *p_final_sample);


	struct SampleCache {

		int factor;
		vector<float> min_peak_data;
		vector<float> max_peak_data;
	};

	int sample_cache_target;
	int max_sample_cache_size;
	vector<SampleCache> sample_cache;

	void paintEvent( QPaintEvent * p_event );

	float zoom;
	int offset;


	bool recompute_pending;
	Sample_Viewer();
	Sample_Data *sample_data;

        vector<bool> position_display_cache;
        vector<bool>  old_position_display_cache;

	void draw_screen_pos(int p_x,QPainter &painter,int factor);
	bool event ( QEvent * e );
	bool grabbing_mouse;
	int closer_point; //1 or 2

	enum GrabType {

		SET_SELECTION,
		SET_SELECTION_BEGIN,
		SET_SELECTION_END
	};

	GrabType grab_type;


	Selection *selection;
public:

	static QColor * get_color(Colors p_color);
	static void init_colormap();

	//QSizePolicy sizePolicy() const;
	void sample_changed_notification() { recompute_pending=true; update(); }
	float get_zoom();
	void set_zoom(float p_factor);
	void set_selection(Selection *p_selection) { selection=p_selection; }
	Selection *get_selection() { return selection; }

	int get_zoom_offset();
	void set_zoom_offset(int p_offset);
	int get_max_zoom_offset();

	void set_position_list(const PositionList& p_list);
	void update_position_list_display();


	Sample_Viewer( QWidget *parent=0);
	void set_sample_data(Sample_Data *p_sample_Data);

	~Sample_Viewer();
};

#endif
