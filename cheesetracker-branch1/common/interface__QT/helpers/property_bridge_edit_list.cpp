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

#include "property_bridge_edit_list.h"


void Property_Bridge_Edit_List::clear() {

	for (int i=0;i<(int)sections.size();i++) {

		tabs->removePage(sections[i].scroll);
		delete sections[i].scroll; //should take care of the rest, thanks Qt!
	}

	sections.clear();
}

void Property_Bridge_Edit_List::create_section(string p_name, bool p_in_tabwidget) {

	Section section;

	if (p_in_tabwidget)
		section.scroll = new Q3ScrollView(tabs);
	else
		section.scroll = new Q3ScrollView(this);

	section.scroll->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	section.scroll->setResizePolicy(Q3ScrollView::AutoOneFit);
	section.scroll->enableClipper(true);
	section.scroll->show();
	section.vbox = new Q3VBox( section.scroll );
	section.vbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	section.vbox->setMinimumWidth(200);
	section.scroll->addChild(section.vbox);
	section.vbox->show();
	if (p_in_tabwidget)
		tabs->addTab(section.scroll,p_name.c_str());
	sections.push_back(section);
}


bool Property_Bridge_Edit_List::get_bridge(Property_Bridge *p_bridge,QWidget *p_parent) {


	QWidget * parent = p_parent; //sections[ sections.size() -1 ].vbox;

	if ( p_bridge->get_type()=="int")  {

		Int_Property_Bridge *bridge_int = dynamic_cast<Int_Property_Bridge*>(p_bridge);
		if (bridge_int==NULL) {
			//printf("Warning: int bridge failed dynamic cast!\n");
			return true;
		}
		if (bridge_int->get_display_preference()=="slider") {

			Q_Property_Bridge_Int *bridge = new Q_Property_Bridge_Int(parent);
			bridge->set_bridge(bridge_int);
		} else {



			Q_Property_Bridge_Int_CSpinButon *bridge = new Q_Property_Bridge_Int_CSpinButon( parent );

			bridge->set_bridge(bridge_int);
			bridge->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
			bridge->show();
		}
		return false;
	} else if ( p_bridge->get_type()=="float")  {

		Float_Property_Bridge *bridge_float = dynamic_cast<Float_Property_Bridge*>(p_bridge);
		if (bridge_float==NULL) {
			ERROR("Bridge is not float?");
			return true;
		}

                Q_Property_Bridge_Float *bridge = new Q_Property_Bridge_Float(parent,true);
		bridge->set_bridge(bridge_float);
		bridge->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
		bridge->show();

		return false;

	} else if ( p_bridge->get_type()=="bool")  {

		Bool_Property_Bridge *bridge_bool = dynamic_cast<Bool_Property_Bridge*>(p_bridge);
		if (bridge_bool==NULL)
			return true;

                Q_Property_Bridge_Bool *bridge = new Q_Property_Bridge_Bool(parent);
		bridge->set_bridge(bridge_bool);
		bridge->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
		bridge->show();
		return false;

	} else if ( p_bridge->get_type()=="string")  {

		String_Property_Bridge *bridge_string = dynamic_cast<String_Property_Bridge*>(p_bridge);
		if (bridge_string==NULL)
			return true;

		if (bridge_string->get_display_preference()=="multi_line") {

			Q_Property_Bridge_String *bridge = new Q_Property_Bridge_String(parent);
			bridge->set_bridge(bridge_string);
			bridge->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
			bridge->show();

		} else {
			Q_Property_Bridge_String_Multiline *bridge = new Q_Property_Bridge_String_Multiline(parent);
			bridge->set_bridge(bridge_string);
			bridge->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
			bridge->show();
		}
		return false;
	} else if ( p_bridge->get_type()=="options")  {

		Options_Property_Bridge *bridge_options = dynamic_cast<Options_Property_Bridge*>(p_bridge);
		if (bridge_options==NULL)
			return true;


		Q_Property_Bridge_Options *bridge = new Q_Property_Bridge_Options(parent);
		bridge->set_bridge(bridge_options);
		bridge->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
		bridge->show();

		return false;
	}

	return true;
}


void Property_Bridge_Edit_List::set_property_bridge_list(list<Property_Bridge*> p_list) {

	clear();

	if (p_list.size()==0) {
		tabs->hide();
		return;
	}


	bool ignore_sections=false;

	list<Property_Bridge*>::iterator I=p_list.begin();

	if ((*I)->get_type()=="section") {

        	//create section here
		create_section( (*I)->get_name() );
		I++;
		tabs->show();

	} else {

		create_section("Default",false);
		//removeTabToolTip( sections[ sections.size() -1 ].scroll )
		ignore_sections=true;
		tabs->hide();
	}


	for (;I!=p_list.end();I++) {

		if ( ((*I)->get_type()=="section") && !ignore_sections) {

			create_section( (*I)->get_name() );
		} else {

			if (get_bridge( (*I) , sections[ sections.size() -1 ].vbox )) {
				//printf("Warning, cant find bridge for type %s\n",(*I)->get_type().c_str());
			}
		}
	}
}

Property_Bridge_Edit_List::Property_Bridge_Edit_List(QWidget *p_parent) : Q3VBox(p_parent) {

	tabs = new QTabWidget(this);
	tabs->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
}
