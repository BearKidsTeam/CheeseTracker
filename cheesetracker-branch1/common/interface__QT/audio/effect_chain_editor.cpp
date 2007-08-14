//
// C++ Implementation: effect_chain_editor
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "components/audio/sound_driver_manager.h"
#include "effect_chain_editor.h"


void Effect_Chain_Editor::add_effect_slot() {

	if (!vars.chain) return;

	esp->show();
	if (esp->exec() == QDialog::Accepted) {

		Effect::Parameters *new_effect = esp->get_selected_effect_params();
		if (new_effect) {

			Mutex_Lock * vl=Sound_Driver_Manager::get_singleton_instance()->get_variables_lock();
			if (vl) vl->grab(__FILE__, __LINE__);
			vars.chain->add_effect(new_effect);
			if (vl) vl->release();

			//update_chain(); signal handles it now
			effect_selected_slot( vars.chain->get_effect_count()-1 );
			effect_list->setSelected( vars.selected_chain , true);
		}
	}
	esp->hide();
}

void Effect_Chain_Editor::move_up_slot() {

	if (!vars.chain) return;

	Mutex_Lock * vl=Sound_Driver_Manager::get_singleton_instance()->get_variables_lock();
	if (vl) vl->grab(__FILE__, __LINE__);
	bool res=vars.chain->move_effect_up(vars.selected_chain);
	if (vl) vl->release();
	if (res) {
	    int new_idx = --vars.selected_chain;
	    if (new_idx<0)
                 new_idx=0;
		//update_chain(); signal handles it now
		vars.selected_chain=new_idx;
		effect_selected_slot(vars.selected_chain);
		effect_list->setSelected( vars.selected_chain , true);

	}
}

void Effect_Chain_Editor::move_down_slot() {

	if (!vars.chain) return;

	Mutex_Lock * vl=Sound_Driver_Manager::get_singleton_instance()->get_variables_lock();
	if (vl) vl->grab(__FILE__, __LINE__);
	bool res=vars.chain->move_effect_down(vars.selected_chain);
	if (vl) vl->release();

	if (res) {
	    int new_idx = ++vars.selected_chain;
	    if (new_idx>=vars.chain->get_effect_count())
		new_idx=vars.chain->get_effect_count()-1;
		if (new_idx<0) {
                    ERROR("what?");
		    return;
		}
			//update_chain(); signal handles it now
		vars.selected_chain=new_idx;
		effect_selected_slot(vars.selected_chain);
		effect_list->setSelected( vars.selected_chain , true);
	}
}

void Effect_Chain_Editor::clear() {

	prop_edit_list->clear();
	effect_list->clear();

}

void Effect_Chain_Editor::erase_slot() {

	if (!vars.chain) return;

	Mutex_Lock * vl=Sound_Driver_Manager::get_singleton_instance()->get_variables_lock();
	if (vl) vl->grab(__FILE__, __LINE__);
	bool res=vars.chain->remove_effect(vars.selected_chain);
	if (vl) vl->release();

	if (res) {

		//update_chain();    signal handles it now
		vars.selected_chain--;
		if (vars.selected_chain>=0) {
			effect_selected_slot(vars.selected_chain);
			effect_list->setSelected( vars.selected_chain , true);
		}
	}
}

void Effect_Chain_Editor::effect_selected_slot(int p_slot) {

	vars.selected_chain=p_slot;
	list<Property_Bridge*> bridge_list=vars.chain->get_effect(p_slot)->get_property_list();
	prop_edit_list->set_property_bridge_list( bridge_list );
}


void Effect_Chain_Editor::update_chain() {

	clear();
	if (vars.chain==NULL)
		return;

	for (int i=0;i<vars.chain->get_effect_count();i++) {

		effect_list->insertItem(vars.chain->get_effect(i)->get_display_name().c_str(),i);
	}

}


void Effect_Chain_Editor::set_effect_chain(Effect_Parameter_Chain *p_chain) {

	vars.chain=p_chain;
	vars.selected_chain=-1;
	update_chain();
	vars.chain_connection.disconnect();
	vars.chain_connection=p_chain->chain_changed_signal.connect(SigC::slot( *this, &Effect_Chain_Editor::update_chain ) );
}


Effect_Chain_Editor::Effect_Chain_Editor(QWidget *p_parent) : Q3HBox(p_parent) {

	Q3VBox *vb = new Q3VBox(this);
	effect_list = new Q3ListBox(vb);
	effect_list->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	list_manage = new Clist_Manager(vb);
	list_manage->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	prop_edit_list = new Property_Bridge_Edit_List(this);
	prop_edit_list->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	setStretchFactor(vb,1);
	setStretchFactor(prop_edit_list,1);
	vars.chain=NULL;
	vars.selected_chain=0;
	QObject::connect(effect_list,SIGNAL(highlighted(int)) ,this,SLOT(effect_selected_slot(int)));
	esp= new Effect_Select_Popup;

	QObject::connect(list_manage->add,SIGNAL(clicked()) ,this,SLOT(add_effect_slot()));
	QObject::connect(list_manage->moveup,SIGNAL(clicked()) ,this,SLOT(move_up_slot()));
	QObject::connect(list_manage->movedown,SIGNAL(clicked()) ,this,SLOT(move_down_slot()));
	QObject::connect(list_manage->remove,SIGNAL(clicked()) ,this,SLOT(erase_slot()));


}


Effect_Chain_Editor::~Effect_Chain_Editor()
{
}


