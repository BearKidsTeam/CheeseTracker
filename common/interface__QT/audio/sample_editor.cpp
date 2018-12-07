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
#include "sample_editor.h"
#include <q3groupbox.h>

void Sample_Editor::add_edit_effect(Edit_Effect *p_edit_effect) {


	sample_editor_effects->add_edit_effect(p_edit_effect);
}

void Sample_Editor::data_modification_begin_notify_cbk() {


}

void Sample_Editor::data_modification_end_notify_cbk() {

	sample_viewer->set_zoom_offset( sample_viewer->get_zoom_offset() );
	sample_viewer->sample_changed_notification();
	sample_viewer_zoom->update_zoom_bar();
	sample_editor_format->set_sample_data(sample_data);

}


void Sample_Editor::set_sample_data(Sample_Data *p_sample_data) {
	sample_viewer->set_sample_data(p_sample_data);
	sample_viewer_zoom->set_sample_data(p_sample_data);
	sample_editor_format->set_sample_data(p_sample_data);
	sample_editor_clipboard->set_sample_data(p_sample_data);
	sample_editor_effects->set_sample_data(p_sample_data);

	sample_data=p_sample_data;
	selection.clear();

}

void Sample_Editor::destructive_operation_end_cbk() {

	if (sample_data && sample_data->get_size()) {
		if (selection.get_point_1()>=sample_data->get_size()) {

			selection.set_point_1(sample_data->get_size()-1);
		}
		if (selection.get_point_2()>=sample_data->get_size()) {

			selection.set_point_2(sample_data->get_size()-1);
		}
		if (selection.get_point_2()==selection.get_point_1()) {
			selection.clear();
		}
	} else {
		selection.clear();
	}

	//sample_editor_clipboard->set_sample_data(sample_data);
	sample_viewer->set_zoom_offset( sample_viewer->get_zoom_offset() );
	sample_viewer->sample_changed_notification();
	sample_editor_effects->set_sample_data(sample_data); //if destructive, sorry!

	sample_viewer_zoom->update_zoom_bar();

	sample_editor_format->set_sample_data(sample_data);


	destructive_operation_end();
}

Sample_Editor::Sample_Editor(QWidget *p_parent) : Q3VBox(p_parent)   {

	sample_data=NULL;

	sample_viewer = new Sample_Viewer(this);
	setStretchFactor(sample_viewer,3);
	sample_viewer->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	format_box = new Q3HBox(this);
	format_box->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	sample_viewer_zoom = new Sample_Viewer_Zoom(this);
	sample_viewer_zoom->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	sample_viewer_zoom->set_sample_viewer(sample_viewer);

	tabs = new QTabWidget(this);
	setStretchFactor(tabs,2);


	Q3VBox *auxvb2 = new Q3VBox(tabs);
	auxvb2->setMargin(5);
	Q3GroupBox *auxvb = new Q3GroupBox(1, Qt::Vertical,"Sample Format && Loop",auxvb2);
	auxvb->setMargin(5);
	sample_editor_format=new Sample_Editor_Format(auxvb);

	sample_editor_format->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	sample_editor_format->set_sample_viewer(sample_viewer);
	sample_viewer->set_selection(&selection);
	tabs->addTab(auxvb2,"Format");
	QObject::connect(sample_editor_format,SIGNAL(sample_base_freq_changed()),this,SIGNAL(sample_freq_changed()));


	Q3VBox *auxvb2_2 = new Q3VBox(tabs);
	auxvb2_2->setMargin(5);
	Q3GroupBox *auxvb_2 = new Q3GroupBox(1, Qt::Vertical,"Clipboard Options",auxvb2_2);
	auxvb_2->setMargin(5);

	sample_editor_clipboard = new Sample_Editor_Clipboard(auxvb_2);
	sample_editor_clipboard->set_selection(&selection);
	tabs->addTab(auxvb2_2,"Edit");
	sample_editor_clipboard->setMargin(5);

	sample_editor_clipboard->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	QObject::connect(sample_editor_clipboard,SIGNAL(destructive_operation_begin()),this,SIGNAL(destructive_operation_begin()));
	QObject::connect(sample_editor_clipboard,SIGNAL(destructive_operation_end()),this,SLOT(destructive_operation_end_cbk()));

	sample_editor_effects = new Sample_Editor_Effects(tabs);
	sample_editor_effects->setMargin(5);
	tabs->addTab(sample_editor_effects,"Effects");
	sample_editor_effects->set_selection(&selection);

	QObject::connect(sample_editor_effects,SIGNAL(data_modification_begin_notify()),this,SLOT(data_modification_begin_notify_cbk()));
	QObject::connect(sample_editor_effects,SIGNAL(data_modification_end_notify()),this,SLOT(data_modification_end_notify_cbk()));

	QObject::connect(sample_editor_effects,SIGNAL(destructive_operation_begin()),this,SIGNAL(destructive_operation_begin()));
	QObject::connect(sample_editor_effects,SIGNAL(destructive_operation_end()),this,SLOT(destructive_operation_end_cbk()));



}

Sample_Editor::~Sample_Editor()
{
}
