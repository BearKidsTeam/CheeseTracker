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

#ifndef SAMPLE_EDITOR_H
#define SAMPLE_EDITOR_H

#include <qvbox.h>
#include "interface__QT/audio/sample_viewer_zoom.h"
#include "interface__QT/audio/sample_editor_format.h"
#include "interface__QT/audio/sample_editor_clipboard.h"
#include "interface__QT/audio/sample_editor_effects.h"
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qtabwidget.h>


/**
 *
 * Juan Linietsky
 **/
class Sample_Editor : public QVBox {

	Q_OBJECT


	Sample_Editor();

	Sample_Viewer * sample_viewer;
	QHBox *format_box;
	Sample_Data *sample_data;
	Sample_Viewer_Zoom * sample_viewer_zoom;
	Sample_Editor_Format *sample_editor_format;
	QTabWidget *tabs;

	Sample_Editor_Clipboard *sample_editor_clipboard;
	Sample_Editor_Effects *sample_editor_effects;

	Selection selection;



protected slots:

	void destructive_operation_end_cbk();
	void data_modification_begin_notify_cbk();
	void data_modification_end_notify_cbk();

signals:
	void destructive_operation_begin();
	void destructive_operation_end();
	void sample_freq_changed();
public:

	void add_edit_effect(Edit_Effect *p_edit_effect);

	Sample_Viewer* get_sample_viewer() { return sample_viewer; }
	void set_sample_data(Sample_Data *p_sample_data);
	QTabWidget *get_tabs() { return tabs; }

	Sample_Editor(QWidget *p_parent);

    ~Sample_Editor();
};

#endif
