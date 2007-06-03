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

#ifndef _SAMPLE_EDIT_H_
#define _SAMPLE_EDIT_H_


#include "gui-gtk/helpers/property_bridge_view.h"
#include "gui-gtk/helpers/widget_garbage_collector.h"
#include "gui-gtk/widgets/columned_list.h"
#include "trackercore/sample.h"
#include "interface_modes.h"


/**
 * 
 * Juan Linietsky
 **/
class Sample_Edit : public Gtk::HBox {


	/* Sample List */

	/* GTK treeview is a nightmare, see why */
	class Model_Columns : public Columned_List::Base_Model_Column {
	
		public:

		Model_Columns()
		{ add(column_index); add(column_active); add(column_volume); add(column_name); }

		Gtk::TreeModelColumn<unsigned int> column_index;
		Gtk::TreeModelColumn<Glib::ustring> column_active;
		Gtk::TreeModelColumn<Glib::ustring> column_volume;
		Gtk::TreeModelColumn<Glib::ustring> column_name;
	};


        void row_selected(int p_row);

	Model_Columns tree_model;
	Columned_List sample_list;
	/* Bridges */
	Widget_Garbage_Collector collector;

	Bridge_View_String prop_name;
	Bridge_View_String prop_filename;


	Bridge_View_Int_Slider prop_def_volume;
	Bridge_View_Int_Slider prop_glb_volume;

	Bridge_View_Bool prop_def_panning_on;
	Bridge_View_Int_Slider prop_def_panning;

	Bridge_View_Int_Slider prop_vibrato_speed;
	Bridge_View_Int_Slider prop_vibrato_depth;
	Bridge_View_Int_Slider prop_vibrato_rate;
	
	void init_list();
public:

	void build_interface(InterfaceMode p_mode);

	Sample_Edit();
	~Sample_Edit();
};

#endif
