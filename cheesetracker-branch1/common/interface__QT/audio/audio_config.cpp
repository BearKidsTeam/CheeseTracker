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
#include "audio_config.h"
#include <q3filedialog.h>
//Added by qt3to4:
#include <QLabel>

const int Audio_Config::frequency_options[Audio_Config::OPTIONS_FREQUENCY] = {

	4096,
	8192,
	11025,
	22050,
	32768,
	44100,
	48000,
};
const int Audio_Config::buffersize_options[Audio_Config::OPTIONS_BUFFERSIZE] = {
	256,
	512,
	1024,
	2048,
	4096,
	8192,
	16384,
	32768,
	65536
};



void Audio_Config::load_settings(ConfigHandler *p_config_handler) {

	driver_manager->stop_active_driver();
	driver_manager->request_mix_stereo(p_config_handler->get_int_property("Sound Driver","mix_stereo"));
	driver_manager->request_mix_16bits(p_config_handler->get_int_property("Sound Driver","mix_16bits"));
	driver_manager->request_mix_frequency(p_config_handler->get_int_property("Sound Driver","mix_freq"));
	driver_manager->request_buffer_size(p_config_handler->get_int_property("Sound Driver","mix_buffersize"));
	driver_manager->set_active_driver(p_config_handler->get_int_property("Sound Driver","audio_driver_index")-1);
	driver_manager->stop_active_driver();

	if (p_config_handler->get_int_property("Sound Driver","audio_driver_enabled"))
		driver_manager->reset_active_driver();
//	p_config_handler->add_property("Sound Driver","audio_driver_file",driver_manager->get_active_driver_current_file(),"File this audio driver needs (if applies)");

	update_driver_variables();

}

void Audio_Config::save_settings(ConfigHandler *p_config_handler) {

	p_config_handler->add_section("Sound Driver");
	p_config_handler->add_property("Sound Driver","mix_stereo",driver_manager->get_mix_stereo(),"1 stereo, 0 mono");
	p_config_handler->add_property("Sound Driver","mix_16bits",driver_manager->get_mix_16bits(),"1 16bits, 0 8bits");
	p_config_handler->add_property("Sound Driver","mix_freq",driver_manager->get_mix_frequency(),"Sampling Rate (in HZ)");
	p_config_handler->add_property("Sound Driver","mix_buffersize",driver_manager->get_mix_buffer_size(),"Mixing Buffer Size (bytes)");

	string audio_list;
	static char buf[200];
	for (int i=0;i<driver_manager->get_amount_of_drivers();i++) {
        	buf[199]=0; 
		snprintf(buf,199,"%i- %s ",i+1,driver_manager->get_driver_name(i).c_str());
		audio_list+=buf;
	}

	p_config_handler->add_property("Sound Driver","audio_driver_index",driver_manager->get_active_driver()+1,audio_list);
	p_config_handler->add_property("Sound Driver","audio_driver_enabled",driver_manager->is_active_driver_active(),"if 1, start enabled, 0 start disabled");
	p_config_handler->add_property("Sound Driver","audio_driver_file",driver_manager->get_active_driver_current_file(),"File this audio driver needs (if applies)");
}


void Audio_Config::update_driver_variables() {

        updating_variables=true;

	driver_list_box->clear();

	for (int i=0;i<driver_manager->get_amount_of_drivers();i++) {

		driver_list_box->insertItem(driver_manager->get_driver_name(i).c_str(),i);
	}

	combo_stereo->setCurrentItem(driver_manager->get_mix_stereo()?1:0);
	combo_16bits->setCurrentItem(driver_manager->get_mix_16bits()?1:0);
	char buf[40];
	buf[39]=0;
	snprintf(buf,39,"%i",driver_manager->get_mix_buffer_size());
	combo_buffersize->setCurrentText(buf);

	snprintf(buf,39,"%i",driver_manager->get_mix_frequency());
	combo_freq->setCurrentText(buf);


	button_enabled->setChecked(driver_manager->is_active_driver_active());

	driver_list_box->setSelected(driver_manager->get_active_driver(),true);

	save_name->setText(driver_manager->get_active_driver_current_file().c_str());

	if (driver_manager->does_active_driver_perform_disk_write())
		hbox_2->show();
	else
		hbox_2->hide();

        updating_variables=false;

}
void Audio_Config::button_reset_callback() {

        driver_manager->reset_active_driver();
	update_driver_variables();
}

void Audio_Config::listbox_driver_changed(int p_idx) {
	if (updating_variables)
		return;

	driver_manager->set_active_driver(p_idx);
 	update_driver_variables();

}


void Audio_Config::combo_stereo_changed_cbk(int p_idx) {
	if (updating_variables)
		return;

	driver_manager->request_mix_stereo(p_idx);
	update_driver_variables();

}

void Audio_Config::combo_bits_changed_cbk(int p_idx) {
	if (updating_variables)
		return;

	driver_manager->request_mix_16bits(p_idx);

	update_driver_variables();

}

void Audio_Config::combo_buffersize_changed_cbk(int p_idx) {
	if (updating_variables)
		return;

	driver_manager->request_buffer_size(buffersize_options[p_idx]);

	update_driver_variables();

}

void Audio_Config::combo_freq_changed_cbk(int p_idx) {
	if (updating_variables)
		return;

	driver_manager->request_mix_frequency(frequency_options[p_idx]);
	update_driver_variables();

}

void Audio_Config::button_enabled_cbk(bool p_on) {
	if (updating_variables)
		return;

	if (p_on)
	        driver_manager->reset_active_driver();
	else
	        driver_manager->stop_active_driver();

	update_driver_variables();

}


void Audio_Config::set_sound_driver_manager(Sound_Driver_Manager *p_driver_manager) {

	driver_manager=p_driver_manager;
	update_driver_variables();

}

void Audio_Config::button_choose_file_callback() {

	QString filter;

	filter="Output File/Device";

	if (driver_manager->get_active_driver_disk_write_extension()!="") {

		filter = filter+(QString)"( "+driver_manager->get_active_driver_disk_write_extension().c_str()+ (QString)")";
	} else {

		filter = filter = "( *.* )";
	}



	QString fn = Q3FileDialog::getOpenFileName( save_name->text(), filter, this );
	if ( !fn.isEmpty() ) {
		entry_file_changed_callback(fn);
	}

}

void Audio_Config::entry_file_changed_callback(const QString &p_string) {
	if (updating_variables)
		return;

         driver_manager->set_active_driver_disk_writer_file(save_name->text().ascii());
	 update_driver_variables();
}

Audio_Config::Audio_Config(QWidget *p_parent) : Q3GroupBox(1, Qt::Vertical,"Audio Device",p_parent)
{
	vbox = new Q3VBox(this);
	hbox_1 = new Q3HBox(vbox);
	hbox_1->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

        driver_list_box = new Q3ListBox(hbox_1);
	driver_list_box->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	driver_list_box->setMinimumWidth(200);

	options_group = new Q3GroupBox(1,Qt::Vertical,"Properties:",hbox_1);
	options_group->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	hbox_1->setStretchFactor(driver_list_box,3);
	hbox_1->setStretchFactor(options_group,2);

	driver_options = new Q3VBox(options_group);
	driver_options->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	new QLabel("Sampling Depth:",driver_options);
	combo_16bits = new QComboBox(driver_options);
	combo_16bits->insertItem("8 Bits",0);
	combo_16bits->insertItem("16 Bits",1);

	new QLabel("Channels:",driver_options);
	combo_stereo = new QComboBox(driver_options);
	combo_stereo->insertItem("Mono",0);
	combo_stereo->insertItem("Stereo",1);

	new QLabel("Buffer Size:",driver_options);
	combo_buffersize = new QComboBox(driver_options);

	char buf[40];
	buf[39]=0;

	for (int i=0;i<OPTIONS_BUFFERSIZE;i++) {

		snprintf(buf,39,"%i",buffersize_options[i]);
		combo_buffersize->insertItem(buf,i);

	}


	new QLabel("Sampling Rate:",driver_options);
	combo_freq = new QComboBox(driver_options);

	for (int i=0;i<OPTIONS_FREQUENCY;i++) {

		snprintf(buf,39,"%i",frequency_options[i]);
		combo_freq->insertItem(buf,i);

	}

	new QLabel("Status:",driver_options);
	status_hbox = new Q3HBox(driver_options);

	button_enabled = new QCheckBox("Enabled",status_hbox);
	button_reset = new QPushButton("Reset!",status_hbox);

	hbox_2 = new Q3HBox(vbox);
	hbox_2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	new QLabel("File Name:",hbox_2);
	save_name = new QLineEdit(hbox_2);
	save_name->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	save_choose = new QPushButton("Browse",hbox_2);


	QObject::connect(driver_list_box,SIGNAL(highlighted(int)),this,SLOT(listbox_driver_changed(int)));

	QObject::connect(combo_stereo,SIGNAL(activated(int)),this,SLOT(combo_stereo_changed_cbk(int)));
	QObject::connect(combo_16bits,SIGNAL(activated(int)),this,SLOT(combo_bits_changed_cbk(int)));
	QObject::connect(combo_buffersize,SIGNAL(activated(int)),this,SLOT(combo_buffersize_changed_cbk(int)));
	QObject::connect(combo_freq,SIGNAL(activated(int)),this,SLOT(combo_freq_changed_cbk(int)));
	QObject::connect(button_reset,SIGNAL(clicked()),this,SLOT(button_reset_callback()));

	QObject::connect(button_enabled,SIGNAL(toggled(bool)),this,SLOT(button_enabled_cbk(bool)));
	QObject::connect(save_choose,SIGNAL(clicked()),this,SLOT(button_choose_file_callback()));

	QObject::connect(save_name,SIGNAL(textChanged(const QString&)),this,SLOT(entry_file_changed_callback(const QString&)));

}

Audio_Config::~Audio_Config()
{
}
