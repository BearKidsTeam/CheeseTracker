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

#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H


#include <q3mainwindow.h>
#include <qtabwidget.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QCloseEvent>
#include "interface__QT/helpers/ccolor_list.h"
#include "interface__QT/audio/audio_config.h"
#include "interface__QT/audio/resampler_config.h"
#include "components/data/config_handler.h"
#include "interface__QT/helpers/property_bridge_edit.h"
#include "interface__QT/helpers/font_bridge.h"
#include "interface__QT/helpers/keyboard_input_config.h"


/**
 *
 * Juan Linietsky
 **/

class Interface_Settings : public Q3GroupBox {

	Q_Property_Bridge_Int *bridge_repaint;
	Q_Property_Bridge_Bool *bridge_bstore;
	Font_Bridge *pattern_edit_font;
	Font_Bridge *order_and_table_font;

public:

        void load_settings(ConfigHandler *p_config_handler);
        void save_settings(ConfigHandler *p_config_handler);

	Interface_Settings(QWidget *p_parent);

};


class Settings_Window : public Q3MainWindow {

	Q3VBox *vbox;
	QTabWidget *sections;
	Q3HBox *action_area;
	QPushButton *button_close;

	Audio_Config *audio_config;
	CColor_List *color_list;
	CColor_List::Preset ct_preset;
	CColor_List::Preset it_preset;
	Keyboard_Input_Config *keyboard_config;
        Interface_Settings *interface_settings;
	Resampler_Config *resampler_config;
	void closeEvent( QCloseEvent *  e);
public:

        void load_settings(ConfigHandler *p_config_handler);
        void save_settings(ConfigHandler *p_config_handler);

	void set_sound_driver_manager(Sound_Driver_Manager *p_driver_manager);
	Settings_Window();
	~Settings_Window();
};

#endif
