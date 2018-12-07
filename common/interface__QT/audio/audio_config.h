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

#ifndef AUDIO_CONFIG_H
#define AUDIO_CONFIG_H

#include "components/audio/sound_driver_manager.h"

#include <q3groupbox.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include "components/data/config_handler.h"



/**
 *
 * Juan Linietsky
 **/
class Audio_Config : public Q3GroupBox {

	Q_OBJECT


	enum {
		OPTIONS_BUFFERSIZE=9,
		OPTIONS_FREQUENCY=7
	};

        static const int frequency_options[OPTIONS_FREQUENCY];
        static const int buffersize_options[OPTIONS_BUFFERSIZE];


	Q3VBox *vbox;
		Q3GroupBox *options_group;
		Q3HBox *hbox_1;
			Q3ListBox *driver_list_box;
			Q3VBox *driver_options;
				QComboBox *combo_stereo;
				QComboBox *combo_16bits;
				QComboBox *combo_buffersize;
				QComboBox *combo_freq;
				Q3HBox *status_hbox;
				QCheckBox *button_enabled;
				QPushButton *button_reset;


	Q3HBox *hbox_2;
		QLineEdit *save_name;
		QPushButton *save_choose;


	Sound_Driver_Manager *driver_manager;

	bool updating_variables;

        void update_driver_variables();

protected slots:

	void listbox_driver_changed(int p_idx);

	void combo_stereo_changed_cbk(int p_idx);
	void combo_bits_changed_cbk(int p_idx);
	void combo_buffersize_changed_cbk(int p_idx);
	void combo_freq_changed_cbk(int p_idx);
	void button_enabled_cbk(bool p_on);
	void button_reset_callback();

	void button_choose_file_callback();

	void entry_file_changed_callback(const QString &p_string);
//	void entry_filename_changed_cbk(QLineEdit& p_line);


public:
        void load_settings(ConfigHandler *p_config_handler);
        void save_settings(ConfigHandler *p_config_handler);

	void set_sound_driver_manager(Sound_Driver_Manager *p_driver_manager);


	Audio_Config(QWidget *p_parent);
	~Audio_Config();
};

#endif
