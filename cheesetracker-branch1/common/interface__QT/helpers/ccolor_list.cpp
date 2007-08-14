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
#include "ccolor_list.h"
//Added by qt3to4:
#include <QLabel>
#include <Q3Frame>
void CColor_List::load_settings(ConfigHandler *p_config_handler) {

//	static char buf[40];
	for (int i=0;i<(int)color_list.size();i++) {


		string prop_name=(string)color_list[i].section.ascii() + ":-_-:" + (string)color_list[i].name.ascii();

		if (!p_config_handler->property_exists("Color Theme",prop_name))
			continue;
		string prop=p_config_handler->get_str_property("Color Theme",prop_name);
		/* No commas? goodbye!*/
		if (prop.find(",")>=prop.length())
			continue;
		/* RED */
		string r=prop.substr(0,prop.find(","));

		prop=prop.substr(prop.find(",")+1,prop.length()-(prop.find(",")+1));


		if (prop.find(",")>=prop.length())
			continue;

		/* GREEN */
		string g=prop.substr(0,prop.find(","));

		prop=prop.substr(prop.find(",")+1,prop.length()-(prop.find(",")+1));

		/* BLUE */
		string b=prop;


		color_list[i].bridge->get_color()->setRgb(atoi(r.c_str()),atoi(g.c_str()),atoi(b.c_str()));
	}
}
void CColor_List::save_settings(ConfigHandler *p_config_handler) {

	p_config_handler->add_section("Color Theme");
	static char buf[40];
	buf[39]=0;
	for (int i=0;i<(int)color_list.size();i++) {


		string prop_name=(string)color_list[i].section.ascii() + ":-_-:" + (string)color_list[i].name.ascii();
		snprintf(buf,39,"%i,%i,%i",color_list[i].bridge->get_color()->red(),color_list[i].bridge->get_color()->green(),color_list[i].bridge->get_color()->blue());

		p_config_handler->add_property("Color Theme",prop_name,buf);
	}

}


void CColor_List::Preset::add_item(string p_section,string p_name,const QColor &p_color) {

	string item_str=p_section+":-_-:"+p_name;
	Data data;
	data.color=p_color;
	data_list[item_str]=data;
}
const QColor * CColor_List::Preset::get_item(string p_section,string p_name) {

	string item_str=p_section+":-_-:"+p_name;
	map<string,Data>::iterator I;
	I=data_list.find(item_str);
	if (I==data_list.end())
		return NULL;

	return &I->second.color;
}

CColor_List::CColor_List(QWidget *p_parent) : Q3VBox(p_parent)
{
	Q3HBox *aux_hbox = new Q3HBox(this);
	aux_hbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	new QLabel("Preset:",aux_hbox);
	preset_combo = new QComboBox(aux_hbox);


	scroll = new Q3ScrollView(this);

	scroll->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	scroll->setResizePolicy(Q3ScrollView::AutoOneFit);
	scroll->enableClipper(true);

	vbox = new Q3VBox(scroll);
	scroll->addChild(vbox);

	QObject::connect(preset_combo,SIGNAL(activated(int)),this,SLOT(preset_selected_cbk(int)));

}

void CColor_List::preset_selected_cbk(int p_which) {

	for (int i=0;i<(int)color_list.size();i++) {

		const QColor * color;
		color=preset_list[p_which]->get_item(color_list[i].section.ascii(),color_list[i].name.ascii());
		if (color==NULL) {

			//printf("Warning no color for section %s, color %s\n",color_list[i].section.ascii(),color_list[i].name.ascii());
			continue;
		}
		color_list[i].bridge->set_color(*color);
	}
}

void CColor_List::add_preset(Preset* p_preset) {

	preset_combo->insertItem(p_preset->name.c_str(),preset_list.size());
	preset_list.push_back(p_preset);

}

void CColor_List::set_section(QString p_section_name,QString p_section_label) {

	current_section=p_section_name;
	new QLabel(p_section_label,vbox);
	Q3Frame * frame = new Q3Frame(vbox);
	frame->setFrameStyle(HLine+Sunken);
}

void CColor_List::register_color(QColor *p_color,const QString &p_name,const QString &p_label) {

	if (current_section=="")
		set_section("default","Default:");

	Color_Data new_color_data;
	new_color_data.bridge = new CColor_Bridge(vbox,p_color,p_label);
	new_color_data.section = current_section;
	new_color_data.name = p_name;

	color_list.push_back(new_color_data);
}

CColor_List::~CColor_List()
{
}

