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
// C++ Interface: sample_editor_effects
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SAMPLE_EDITOR_EFFECTS_H_
#define SAMPLE_EDITOR_EFFECTS_H_

#include "components/audio/edit_effect.h"
#include "components/data/selection.h"
#include "interface__QT/helpers/property_bridge_edit_list.h"
#include <q3hbox.h>
#include <q3vbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <q3groupbox.h>
#include <q3listbox.h>
#include <vector>

	using std::vector;

class Sample_Editor_Effects : public Q3VBox {

	Q_OBJECT

        Q3VBox * main_vbox;

	QPushButton *apply;
	Q3ListBox *effect_listbox;

	vector<Edit_Effect*> effect_list;
	int selected_effect;

	void update_effect_list();
	Selection *selection;
	Sample_Data *sample;
	Property_Bridge_Edit_List *bridge_list_edit;

protected slots:

	void effect_selected_cbk(int p_which);
	void effect_apply_cbk();

signals:

	//this is emited if data is modifierd
	void data_modification_begin_notify();
	void data_modification_end_notify();

	//common stuff for locking/updating
	void destructive_operation_begin();
	void destructive_operation_end();



public:

	void set_selection(Selection *p_selection) { selection=p_selection; }
	void set_sample_data(Sample_Data *p_sample);

	void add_edit_effect(Edit_Effect* p_effect);

	Sample_Editor_Effects(QWidget *p_parent);
};


#endif
