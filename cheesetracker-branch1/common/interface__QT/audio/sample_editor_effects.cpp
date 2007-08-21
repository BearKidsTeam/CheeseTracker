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
// C++ Implementation: sample_editor_effects
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "Error.h"
#include "message_box.h"
#include "sample_editor_effects.h"
//Added by qt3to4:
#include <Q3Frame>


void Sample_Editor_Effects::set_sample_data(Sample_Data *p_sample) {

	sample=p_sample;
	bridge_list_edit->clear();
	effect_selected_cbk(selected_effect);

}


void Sample_Editor_Effects::effect_selected_cbk(int p_which) {

	// This function is called by the moc-generated file
	// moc__sample_editor_effects.cpp.

	GENERIC_TRY_CATCH(

		selected_effect=p_which;

		if (effect_list.size()==0)
			return; //nothing to apply
		if (sample==NULL)
			return;
		if (sample->get_size()==0)
			return;

		int begin=selection->is_active()?selection->get_begin():0;
		int end=selection->is_active()?selection->get_end():(sample->get_size()-1);

		effect_list[selected_effect]->selected_notify(sample,begin,end);
		bridge_list_edit->set_property_bridge_list( effect_list[selected_effect]->get_property_list() );
	)
}

void Sample_Editor_Effects::effect_apply_cbk() {

	GENERIC_TRY_CATCH(
		if (sample==NULL)
			return;
		if (sample->get_size()==0)
			return;
		if (effect_list.size()==0)
			return; //nothing to apply

		int begin=selection->is_active()?selection->get_begin():0;
		int end=selection->is_active()?(selection->get_end()-1):(sample->get_size()-1);

		if (effect_list[selected_effect]->is_destructive())
			destructive_operation_begin();

		//remind this!
		data_modification_begin_notify();

		effect_list[selected_effect]->process(sample,begin,end);

		data_modification_end_notify();

		if (effect_list[selected_effect]->is_destructive())
			destructive_operation_end();

	)
}

void Sample_Editor_Effects::update_effect_list() {

	effect_listbox->clear();

	for (int i=0;i<(int)effect_list.size();i++) {


		effect_listbox->insertItem(effect_list[i]->get_name().c_str(),i);
	}
	effect_listbox->setSelected(selected_effect,true);
}


void Sample_Editor_Effects::add_edit_effect(Edit_Effect* p_effect) {

	effect_list.push_back(p_effect);

	update_effect_list();
}


Sample_Editor_Effects::Sample_Editor_Effects(QWidget *p_parent) : Q3VBox(p_parent) {

	Q3GroupBox *useless_gb= new Q3GroupBox( 1, Qt::Horizontal,"Effect List",this);
	useless_gb->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        Q3VBox * split_vbox = new Q3VBox(useless_gb);
	split_vbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        Q3HBox * split_hbox = new Q3HBox(split_vbox);
	split_hbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	/* Effect listbox */
        effect_listbox = new Q3ListBox(split_hbox);
	effect_listbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	effect_listbox->setScrollBar(true);

	/* effet stuff */
	//QTabWidget *sections = new QTabWidget(split_hbox);
	//sections->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	bridge_list_edit = new Property_Bridge_Edit_List(split_hbox);
	bridge_list_edit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	//sections->addTab(bridge_list_edit,"Params");

	/* apply button */
	Q3HBox * auxhv = new Q3HBox(split_vbox);
	auxhv->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	Q3Frame *useless = new Q3Frame(auxhv);
	useless->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	apply = new QPushButton("Apply",auxhv);
//	apply->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding));

	QObject::connect(apply,SIGNAL(clicked()),this,SLOT(effect_apply_cbk()));
	QObject::connect(effect_listbox,SIGNAL(highlighted(int)),this,SLOT(effect_selected_cbk(int)));
	sample=NULL;
	selection=NULL;
	selected_effect=0;

}
