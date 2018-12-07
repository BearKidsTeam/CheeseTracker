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
#include "resampler_config.h"


void Resampler_Config::resampler_selected(int p_which) {

	Resampler_Manager * resmanager=Resampler_Manager::get_resampler_manager();
	resmanager->set_default_resampler(p_which);
}


void Resampler_Config::update_resampler_list() {

	Resampler_Manager * resmanager=Resampler_Manager::get_resampler_manager();

	resampler_list_box->clear();

	for (int i=0;i<resmanager->get_resampler_count();i++) {

		resampler_list_box->insertItem(resmanager->get_resampler(i)->get_name().c_str(),i);
	}
	resampler_list_box->setSelected(resmanager->get_default_resampler(),true);

}


Resampler_Config::Resampler_Config(QWidget *p_parent) : Q3GroupBox(1, Qt::Vertical,"Wave Resampler:",p_parent) {

	resampler_list_box = new Q3ListBox(this);
	resampler_list_box->setSizePolicy(QSizePolicy( QSizePolicy::Expanding , QSizePolicy::Expanding ));

	QObject::connect(resampler_list_box,SIGNAL(highlighted(int)),this,SLOT(resampler_selected(int)));

	update_resampler_list();
}

void Resampler_Config::load_settings(ConfigHandler *p_config_handler) {

	Resampler_Manager * resmanager=Resampler_Manager::get_resampler_manager();
	resmanager->set_default_resampler(p_config_handler->get_int_property("Resampler","resampler_index"));
	update_resampler_list();
}

void Resampler_Config::save_settings(ConfigHandler *p_config_handler) {

	Resampler_Manager * resmanager=Resampler_Manager::get_resampler_manager();
	p_config_handler->add_section("Resampler");

	p_config_handler->add_property("Resampler","resampler_index",resmanager->get_default_resampler(),"0 Raw, 1 Linear");


}

Resampler_Config::~Resampler_Config()
{
}
