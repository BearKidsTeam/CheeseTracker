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

#ifndef SAMPLE_EDITOR_FORMAT_H
#define SAMPLE_EDITOR_FORMAT_H


#include <qlabel.h>
#include <qgrid.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qvbox.h>
#include "common/components/audio/sample_data.h"
#include "common/interface__QT/helpers/cspinbutton.h"
#include "interface__QT/audio/sample_viewer.h"
/**
 *
 * Juan Linietsky
 **/
class Sample_Editor_Format : public QVBox {

	Q_OBJECT

	bool updating;
	QLabel * label_depth;
	QLabel * label_size;
	QLabel * label_base_freq;

	QLabel * label_loop_type;
	QLabel * label_loop_begin;
	QLabel * label_loop_end;

	QLineEdit *entry_depth;
	QLineEdit *entry_size;

  	QComboBox* combo_loop_type;
	CSpinButton*spin_loop_begin;
	CSpinButton *spin_loop_end;
	CSpinButton *spin_base_freq;

	Sample_Data *sample_data;
	Sample_Viewer *sample_viewer;

	void update_format();
	void update_loop();
protected slots:

	void loop_begin_changed_cbk(int p_idx);
	void loop_end_changed_cbk(int p_idx);
	void loop_type_changed_cbk(int p_idx);

	void base_freq_changed(int p_frq);

	void transpose_up_1_4_cbk();
	void transpose_up_1_2_cbk();
	void transpose_up_1_cbk();
	void transpose_up_12_cbk();

	void transpose_down_1_4_cbk();
	void transpose_down_1_2_cbk();
	void transpose_down_1_cbk();
	void transpose_down_12_cbk();

signals:

	void sample_base_freq_changed();


public:




	void set_sample_data(Sample_Data *p_sample_data);
	void set_sample_viewer(Sample_Viewer *p_sample_viewer);
	Sample_Editor_Format(QWidget *p_grid);

	~Sample_Editor_Format();
};

#endif
