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

#include "sample_edit.h"



 void Sample_Edit::row_selected(int p_row) {


}

void Sample_Edit::init_list() {


	Gtk::TreeModel::Row auxrow = *(sample_list.tree_model_ref->append());
	auxrow[tree_model.column_internal_index] = 1;
	auxrow[tree_model.column_index] = 1;
	auxrow[tree_model.column_name] = "hooraye";

	auxrow = *(sample_list.tree_model_ref->append());
	auxrow[tree_model.column_index] = 2;
	auxrow[tree_model.column_internal_index] = 0;
	auxrow[tree_model.column_name] = "mamma mia";

	//Add the TreeView's view columns:
	sample_list.tree_view.append_column("#", tree_model.column_index);
	sample_list.tree_view.append_column("Active", tree_model.column_active);
	sample_list.tree_view.append_column("L", tree_model.column_volume);
	sample_list.tree_view.append_column("Name                   ", tree_model.column_name);

	sample_list.row_selected.connect(SigC::slot(*this,&Sample_Edit::row_selected));
}


void Sample_Edit::build_interface(InterfaceMode p_mode) {

	switch (p_mode) {

		case IMPULSE_TRACKER: {
		
			Gtk::VBox * vbox_one = new Gtk::VBox;
			collector.add(vbox_one);
			pack_start(sample_list,0,0);
			pack_start(*vbox_one);
			
			vbox_one->pack_start(prop_def_volume,0,0);			
			vbox_one->pack_start(prop_glb_volume,0,0);			
			vbox_one->pack_start(prop_def_panning_on,0,0);			
			vbox_one->pack_start(prop_def_panning,0,0);			
			vbox_one->pack_start(prop_vibrato_speed,0,0);			
			vbox_one->pack_start(prop_vibrato_depth,0,0);			
			vbox_one->pack_start(prop_vibrato_rate,0,0);			
			
 					
		} break;
		default: {
		
			//bleh		
		};
		
	}
}


Sample_Edit::Sample_Edit() : sample_list(&tree_model) {

	init_list();
	build_interface(IMPULSE_TRACKER);
	sample_list.set_label("Sample List:");


}


Sample_Edit::~Sample_Edit()
{
}
