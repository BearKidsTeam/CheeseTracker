//
// C++ Interface: effect_chain_editor
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EFFECT_CHAIN_EDITOR_H
#define EFFECT_CHAIN_EDITOR_H

#include "components/audio/effect_chain.h"

#include <q3listbox.h>
#include <q3hbox.h>
#include "interface__QT/helpers/clist_manager.h"
#include "interface__QT/popups/effect_select_popup.h"
#include "interface__QT/helpers/property_bridge_edit_list.h"


/**
@author Juan Linietsky
*/
class Effect_Chain_Editor : public QHBoxLayout, public sigc::trackable {

	Q_OBJECT

	QListBox *effect_list;
	Clist_Manager *list_manage;
	Property_Bridge_Edit_List *prop_edit_list;
	struct Vars {
		Effect_Parameter_Chain *chain;
		int selected_chain;
		sigc::connection chain_connection;
	} vars;
	Effect_Select_Popup * esp;


	void update_chain();
protected slots:

	void effect_selected_slot(int p_which);
	void add_effect_slot();
	void move_up_slot();
	void move_down_slot();
	void erase_slot();

public:
	void clear();
	void set_effect_chain(Effect_Parameter_Chain *p_chain);

	Effect_Chain_Editor(QWidget *p_parent);
	~Effect_Chain_Editor();

};

#endif
