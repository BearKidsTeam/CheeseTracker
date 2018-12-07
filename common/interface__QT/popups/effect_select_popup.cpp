//
// C++ Implementation: effect_select_popup
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "effect_select_popup.h"
#include "components/audio/effect_source_manager.h"
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3Frame>

void Effect_Select_Popup::selected_effect(int p_which) {

	selected_effect_index=p_which;
}


Effect::Parameters * Effect_Select_Popup::get_selected_effect_params() {
	Effect_Source_Manager * esm = Effect_Source_Manager::get_singleton_instance();

	for (int i=0,c=0;i<esm->get_effect_source_count();i++) {

		Effect_Source*es=esm->get_effect_source(i);
		for (int j=0;j<es->get_effect_count();j++,c++) {

			if (c==selected_effect_index)
				return es->instantiate_effect_parameters(j);
		}
	}

	return NULL;
}


Effect_Select_Popup::Effect_Select_Popup() {

	setCaption("Select Effect");
	main_vbox = new Q3VBoxLayout(this);
	main_vbox->setAutoAdd(true);

	list_box = new Q3ListBox(this);
	QObject::connect(list_box,SIGNAL(highlighted(int)) ,this,SLOT(selected_effect(int)));
	list_box->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	Q3HBox *hv = new Q3HBox(this);
	hv->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	ok_button = new QPushButton("Ok",hv);
	QObject::connect(ok_button,SIGNAL(clicked()),this,SLOT(accept()));

	Q3Frame *fr = new Q3Frame(hv);
	fr->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	cancel_button = new QPushButton("Cancel",hv);
	QObject::connect(cancel_button,SIGNAL(clicked()),this,SLOT(reject()));

	setMinimumWidth(500);
	setMinimumHeight(400);

	Effect_Source_Manager * esm = Effect_Source_Manager::get_singleton_instance();

	for (int i=0,c=0;i<esm->get_effect_source_count();i++) {

		string efname=esm->get_effect_source_name(i);
		Effect_Source*es=esm->get_effect_source(i);
		for (int j=0;j<es->get_effect_count();j++,c++) {

			string name = efname+"- "+es->get_effect_display_name(j);
                        list_box->insertItem(name.c_str(),c);
		}
	}
	selected_effect_index=-1;

}

Effect_Select_Popup::~Effect_Select_Popup()
{
}


