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
#include "sample_editor_format.h"
#include "interface__QT/icons/transp_12_down.xpm"
#include "interface__QT/icons/transp_12_up.xpm"
#include "interface__QT/icons/transp_14_down.xpm"
#include "interface__QT/icons/transp_14_up.xpm"
#include "interface__QT/icons/transp_octave_down.xpm"
#include "interface__QT/icons/transp_octave_up.xpm"
#include "interface__QT/icons/transp_semitone_down.xpm"
#include "interface__QT/icons/transp_semitone_up.xpm"
#include <qpushbutton.h>
#include <q3hbox.h>
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
#include <Q3Frame>

void Sample_Editor_Format::transpose_up_1_4_cbk() {

	if (!sample_data->get_size())
		return;

	float freq=sample_data->get_c5_freq();
	freq*=pow(2.0,1.0/96.0);
	sample_data->set_c5_freq(roundf(freq));
	update_format();
	sample_base_freq_changed();
}


void Sample_Editor_Format::transpose_up_1_2_cbk() {

	if (!sample_data->get_size())
		return;

	float freq=sample_data->get_c5_freq();
	freq*=pow(2.0,1.0/48.0);
	sample_data->set_c5_freq(roundf(freq));
	update_format();
	sample_base_freq_changed();

}

void Sample_Editor_Format::transpose_up_1_cbk() {

	if (!sample_data->get_size())
		return;

	float freq=sample_data->get_c5_freq();
	freq*=pow(2.0,1.0/12.0);
	sample_data->set_c5_freq(roundf(freq));
	update_format();
	sample_base_freq_changed();


}
void Sample_Editor_Format::transpose_up_12_cbk() {

	if (!sample_data->get_size())
		return;

	int freq=sample_data->get_c5_freq();
	freq*=2;
	sample_data->set_c5_freq(freq);
	update_format();
	sample_base_freq_changed();

}


void Sample_Editor_Format::transpose_down_1_4_cbk() {

	if (!sample_data->get_size())
		return;

	float freq=sample_data->get_c5_freq();
	freq/=pow(2.0,1.0/96.0);
	sample_data->set_c5_freq(roundf(freq));
	update_format();
	sample_base_freq_changed();
}
void Sample_Editor_Format::transpose_down_1_2_cbk() {

	if (!sample_data->get_size())
		return;

	float freq=sample_data->get_c5_freq();
	freq/=pow(2.0,1.0/48.0);
	sample_data->set_c5_freq(roundf(freq));
	update_format();
	sample_base_freq_changed();

}
void Sample_Editor_Format::transpose_down_1_cbk() {

	if (!sample_data->get_size())
		return;

	float freq=sample_data->get_c5_freq();
	freq/=pow(2.0,1.0/12.0);
	sample_data->set_c5_freq(roundf(freq));
	update_format();
	sample_base_freq_changed();

}
void Sample_Editor_Format::transpose_down_12_cbk() {

	if (!sample_data->get_size())
		return;

	int freq=sample_data->get_c5_freq();
	freq/=2;
	sample_data->set_c5_freq(freq);
	update_format();
	sample_base_freq_changed();

}

void Sample_Editor_Format::base_freq_changed(int p_frq) {
	if (updating) return;

	sample_data->set_c5_freq(p_frq);

}

void Sample_Editor_Format::loop_begin_changed_cbk(int p_idx) {

	if (updating) return;
	sample_data->set_loop_begin(spin_loop_begin->value());
	update_loop();

}
void  Sample_Editor_Format::loop_end_changed_cbk(int p_idx) {

	if (updating) return;
	sample_data->set_loop_end(spin_loop_end->value());
	update_loop();
}

void  Sample_Editor_Format::loop_type_changed_cbk(int p_idx) {
	if (updating) return;
	switch (p_idx) {

		case 0:{
			sample_data->set_loop_enabled(false);
			sample_data->set_loop_ping_pong(false);
		} break;
		case 1:{
			sample_data->set_loop_enabled(true);
			sample_data->set_loop_ping_pong(false);
		} break;
		case 2:{
			sample_data->set_loop_enabled(true);
			sample_data->set_loop_ping_pong(true);
		} break;
	}
}

void Sample_Editor_Format::update_format() {
	updating=true;

	char auxbuf[40];
	auxbuf[39]=0;

	entry_depth->setText( (sample_data->is_16bit()?"16 bits":"8 bits"));
	snprintf(auxbuf,39,"%i",sample_data->get_size());
	entry_size->setText(auxbuf);
	spin_base_freq->setValue(sample_data->get_c5_freq());

	if (sample_data->is_loop_enabled()) {

			if (sample_data->is_loop_ping_pong())
				combo_loop_type->setCurrentItem(2);
			else
				combo_loop_type->setCurrentItem(1);
	} else {

		combo_loop_type->setCurrentItem(0);
	}

	updating=false;
}
void Sample_Editor_Format::update_loop() {

	updating=true;

	spin_loop_begin->setMinValue(0);
	spin_loop_begin->setMaxValue(sample_data->get_loop_end()-1);
	spin_loop_begin->setValue(sample_data->get_loop_begin());




	spin_loop_end->setMinValue(sample_data->get_loop_begin()+1);
	spin_loop_end->setMaxValue(sample_data->get_size());
	spin_loop_end->setValue(sample_data->get_loop_end());


	if (sample_viewer)
		sample_viewer->update();

	updating=false;
}

void Sample_Editor_Format::set_sample_data(Sample_Data *p_sample_data) {

	sample_data=p_sample_data;
	update_format();
	update_loop();

}
void Sample_Editor_Format::set_sample_viewer(Sample_Viewer *p_sample_viewer) {

	sample_viewer=p_sample_viewer;
}

Sample_Editor_Format::Sample_Editor_Format(QWidget *p_parent) : Q3VBox(p_parent) {


	Q3Grid * grid = new Q3Grid(4,this);
	//first row
	label_base_freq = new QLabel ("C5 Freq:",grid);
	spin_base_freq = new CSpinButton(grid);
	spin_base_freq->setMinValue(500);
	spin_base_freq->setMaxValue(0xFFFFFFF);

	label_loop_type = new QLabel("Loop:",grid);
  	combo_loop_type = new QComboBox(grid);
	combo_loop_type->insertItem("Disabled");
	combo_loop_type->insertItem("Forward ->");
	combo_loop_type->insertItem("<- Ping Pong ->");

	//second row
	label_depth = new QLabel ("Format:",grid);
	entry_depth = new QLineEdit(grid);
	entry_depth->setReadOnly(true);
	label_loop_begin = new QLabel("Loop Begin:",grid);
	spin_loop_begin = new CSpinButton(grid);

	//third row
	label_size = new QLabel("Length:",grid);
	entry_size = new QLineEdit(grid);
	entry_size->setReadOnly(true);

	label_loop_end = new QLabel("Loop End:",grid);
	spin_loop_end = new CSpinButton(grid);

	QObject::connect( combo_loop_type , SIGNAL(activated(int)),this, SLOT(loop_type_changed_cbk(int)) );
	QObject::connect( spin_loop_begin, SIGNAL(valueChanged(int)),this, SLOT(loop_begin_changed_cbk(int)) );
	QObject::connect( spin_loop_end, SIGNAL(valueChanged(int)),this, SLOT(loop_end_changed_cbk(int)) );
	QObject::connect( spin_base_freq, SIGNAL(valueChanged(int)),this, SLOT(base_freq_changed(int)) );
	updating=false;
	sample_viewer=NULL;


	grid->setSpacing(5);

	Q3HBox * grid2 = new Q3HBox(this);
	grid2->setSpacing(2);

	QPushButton *auxbut;
	auxbut = new QPushButton(QPixmap((const char**)transp_14_down_xpm),"",grid2);
	auxbut->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	QObject::connect( auxbut, SIGNAL(clicked()), this, SLOT(transpose_down_1_4_cbk()));
	auxbut = new QPushButton(QPixmap((const char**)transp_14_up_xpm),"",grid2);
	auxbut->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	QObject::connect( auxbut, SIGNAL(clicked()), this, SLOT(transpose_up_1_4_cbk()));

	(new Q3Frame(grid2))->setFrameStyle(VLine+Sunken);

	auxbut = new QPushButton(QPixmap((const char**)transp_12_down_xpm),"",grid2);
	auxbut->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	QObject::connect( auxbut, SIGNAL(clicked()), this, SLOT(transpose_down_1_2_cbk()));
	auxbut = new QPushButton(QPixmap((const char**)transp_12_up_xpm),"",grid2);
	auxbut->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	QObject::connect( auxbut, SIGNAL(clicked()), this, SLOT(transpose_up_1_2_cbk()));


	(new Q3Frame(grid2))->setFrameStyle(VLine+Sunken);

	auxbut = new QPushButton(QPixmap((const char**)transp_semitone_down_xpm),"",grid2);
	auxbut->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	QObject::connect( auxbut, SIGNAL(clicked()), this, SLOT(transpose_down_1_cbk()));
	auxbut = new QPushButton(QPixmap((const char**)transp_semitone_up_xpm),"",grid2);
	auxbut->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	QObject::connect( auxbut, SIGNAL(clicked()), this, SLOT(transpose_up_1_cbk()));


	(new Q3Frame(grid2))->setFrameStyle(VLine+Sunken);

	auxbut = new QPushButton(QPixmap((const char**)transp_octave_down_xpm),"",grid2);
	auxbut->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	QObject::connect( auxbut, SIGNAL(clicked()), this, SLOT(transpose_down_12_cbk()));
	auxbut = new QPushButton(QPixmap((const char**)transp_octave_up_xpm),"",grid2);
	auxbut->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	QObject::connect( auxbut, SIGNAL(clicked()), this, SLOT(transpose_up_12_cbk()));

	setSpacing(4);
}

Sample_Editor_Format::~Sample_Editor_Format()
{
}
