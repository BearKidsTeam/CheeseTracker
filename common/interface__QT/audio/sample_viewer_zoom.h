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

#ifndef SAMPLE_VIEWER_ZOOM_H
#define SAMPLE_VIEWER_ZOOM_H


#include <q3vbox.h>
#include "interface__QT/audio/sample_viewer.h"
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qscrollbar.h>
#include <qslider.h>
/**
 *
 * Juan Linietsky
 **/
class Sample_Viewer_Zoom : public Q3VBox {

	Q_OBJECT
	Sample_Viewer * sample_viewer;

        bool updating_zoom;

	Q3HBox * zoom_box;

	QPushButton *zoom_more;
	QPushButton *zoom_less;
	QPushButton *zoom_all;
	QPushButton *zoom_select;

	QScrollBar *zoom_view_offset;

	QPixmap pix_zoom_more;
	QPixmap pix_zoom_less;
	QPixmap pix_zoom_all;
	QPixmap pix_zoom_select;

	Sample_Data *sample_data;
	QSlider * zoom_slider;
	QLabel * zoom_slider_label;


protected Q_SLOTS:

	void zoom_more_cbk();
	void zoom_less_cbk();
	void zoom_all_cbk();
	void zoom_selection_cbk();
	void zoom_slider_cbk(int p_val);
	void zoom_offset_changed_cbk(int p_ofs);

public:

	void update_zoom_bar();

	void set_sample_data(Sample_Data *p_sample_data);
	void set_sample_viewer(Sample_Viewer *p_sample_viewer);
    Sample_Viewer_Zoom(QWidget *p_parent);

    ~Sample_Viewer_Zoom();
};

#endif
