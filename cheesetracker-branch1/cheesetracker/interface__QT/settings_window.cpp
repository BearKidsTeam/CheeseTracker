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
#include <QDesktopWidget>
#include "interface__QT/pattern_edit.h"
#include "interface__QT/table_base.h"
#include "settings_window.h"
#include "interface__QT/icons/icon_colors.xpm"
#include "interface__QT/icons/icon_audio_config.xpm"
#include "interface__QT/icons/icon_interface.xpm"
#include "interface__QT/icons/icon_keys.xpm"
#include <qapplication.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>

#include "interface__QT/interface.h"
#include "interface_binds/tracker_instance.h"
#include "components/data/keyboard_input.h"



Interface_Settings::Interface_Settings(QWidget *p_parent) : Q3GroupBox(1, Qt::Horizontal,"Interface Settings",p_parent) {

	bridge_repaint = new Q_Property_Bridge_Int(this);
	bridge_repaint->set_bridge(&Interface::timer_interval_bridge);
	bridge_repaint->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	bridge_bstore = new Q_Property_Bridge_Bool(this);
	bridge_bstore->set_bridge(&Pattern_Edit::backing_store_bridge);
	QLabel *auxlabel = new QLabel("Pattern Editor Font:",this);
	auxlabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	Pattern_Edit::allocate_font();
	pattern_edit_font = new Font_Bridge(Pattern_Edit::get_font(),this);
	pattern_edit_font->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	auxlabel = new QLabel("Orderlist & Sample/Note Table Font:",this);
	auxlabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	Table_Base::allocate_font();
	order_and_table_font = new Font_Bridge(Table_Base::get_font(),this);
	order_and_table_font->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));


}

void Interface_Settings::load_settings(ConfigHandler *p_config_handler) {


	Interface::timer_interval_bridge.set( p_config_handler->get_int_property("Misc Interface","repaint_interval" ));
	Pattern_Edit::backing_store_bridge.set( p_config_handler->get_int_property("Misc Interface","use_backing_store"));

	Pattern_Edit::get_font()->setFamily( p_config_handler->get_str_property("Misc Interface","pattern_edit_font_family").c_str());
	Pattern_Edit::get_font()->setPointSize(p_config_handler->get_int_property("Misc Interface","pattern_edit_font_point_size"));
	Table_Base::get_font()->setFamily(p_config_handler->get_str_property("Misc Interface","orderlist_font_family").c_str());
	Table_Base::get_font()->setPointSize(p_config_handler->get_int_property("Misc Interface","orderlist_font_point_size"));

	bridge_repaint->set_bridge(&Interface::timer_interval_bridge);
	bridge_bstore->set_bridge(&Pattern_Edit::backing_store_bridge);
	pattern_edit_font->update();
	order_and_table_font->update();


}

void Interface_Settings::save_settings(ConfigHandler *p_config_handler) {

	p_config_handler->add_section("Misc Interface");
	p_config_handler->add_property("Misc Interface","repaint_interval",Interface::timer_interval_bridge.get());
	p_config_handler->add_property("Misc Interface","use_backing_store",Pattern_Edit::backing_store_bridge.get());
	p_config_handler->add_property("Misc Interface","pattern_edit_font_family",Pattern_Edit::get_font()->family().ascii());
	p_config_handler->add_property("Misc Interface","pattern_edit_font_point_size",Pattern_Edit::get_font()->pointSize());
	p_config_handler->add_property("Misc Interface","orderlist_font_family",Table_Base::get_font()->family().ascii());
	p_config_handler->add_property("Misc Interface","orderlist_font_point_size",Table_Base::get_font()->pointSize());
}



void Settings_Window::closeEvent( QCloseEvent *  e) {

	e->ignore();
	hide();
}


void Settings_Window::set_sound_driver_manager(Sound_Driver_Manager *p_driver_manager) {

	audio_config->set_sound_driver_manager(p_driver_manager);
}

void Settings_Window::load_settings(ConfigHandler *p_config_handler) {

	audio_config->load_settings(p_config_handler);
	color_list->load_settings(p_config_handler);
	Keyboard_Input::get_singleton_instance()->load_settings(p_config_handler);
	keyboard_config->update(); //update the keys displayed
	interface_settings->load_settings(p_config_handler);
	resampler_config->load_settings(p_config_handler);
}
void Settings_Window::save_settings(ConfigHandler *p_config_handler) {

	audio_config->save_settings(p_config_handler);
	color_list->save_settings(p_config_handler);
	Keyboard_Input::get_singleton_instance()->save_settings(p_config_handler);
	interface_settings->save_settings(p_config_handler);
	resampler_config->save_settings(p_config_handler);
}


Settings_Window::Settings_Window() : Q3MainWindow(0,"Preferences")
{

	setCaption("CheeseTracker - Preferences");
	vbox = new Q3VBox(this);
	vbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	sections=new QTabWidget(vbox);
	sections->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	action_area=new Q3HBox(vbox);
	action_area->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	Q3Frame *auxframe = new Q3Frame(action_area);
	auxframe->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	button_close = new QPushButton("Close",action_area);
	setFocusProxy(vbox);
	setCentralWidget(vbox);


	Q3HBox *auxhbox = new Q3HBox(sections);

	sections->addTab(auxhbox,QPixmap((const char**)icon_audio_config_xpm),"Audio");

	audio_config = new Audio_Config(auxhbox);
	audio_config->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	auxhbox->setStretchFactor(audio_config,2);
	resampler_config = new Resampler_Config(auxhbox);
	resampler_config->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	auxhbox->setStretchFactor(resampler_config,1);


	color_list = new CColor_List(sections);
	sections->addTab(color_list,QPixmap((const char**)icon_colors_xpm),"Colors");
	color_list->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	Tracker_Instance::init_keyboard();
        keyboard_config = new Keyboard_Input_Config(sections);
	sections->addTab(keyboard_config,QPixmap((const char**)icon_keys_xpm),"Keyboard");
	keyboard_config->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));


	color_list->set_section("pattern_edit","Pattern Editor:");

	Pattern_Edit::allocate_colormap();
	Table_Base::allocate_colormap();

	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_BackGround),"background","Background:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_ForeGround),"foreground","Text:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_Cursor),"cursor","Cursor:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_Row_Hilite_Minor),"hl_maj","Row HL Minor:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_Row_Hilite_Major),"hl_min","Row HL Major:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_BackGround_Selected),"background_selected","Background Selected:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_ForeGround_Selected),"foreground_selected","Text Selected:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_Row_Hilite_Selected),"hl_selected","Row HL Selected:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_SepChannels),"sep","Channel Separator:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_SepNewChannels),"sep_main","Main Separator:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_Mask),"mask","Mask Display:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_ForeGround_Inactive),"foreground_inactive","Foreground Inactive:");
	color_list->register_color(&Pattern_Edit::get_color(Pattern_Edit::Col_Play_Position),"played_position","Play Position:");

	color_list->set_section("lists","Order & Sample Lists:");

	color_list->register_color(Table_Base::get_color(Table_Base::Col_BackGround),"background","Background:");
	color_list->register_color(Table_Base::get_color(Table_Base::Col_ForeGround),"foreground","Text:");
	color_list->register_color(Table_Base::get_color(Table_Base::Col_Cursor),"cursor","Cursor::");
	color_list->register_color(Table_Base::get_color(Table_Base::Col_BeingPlayed),"being_played","Pos. Being Played:");
	color_list->register_color(Table_Base::get_color(Table_Base::Col_Separator),"separator","Hilite:");

	color_list->set_section("sample_edit","Sample Editor:");

	Sample_Viewer::init_colormap();
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_BackGround),"background","Background:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_ForeGround),"foreground","Foreground:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_Center),"center","Center:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_Loop_BackGround),"loop_background","Loop Background:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_Loop_ForeGround),"loop_foreground","Loop Foreground:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_Played_Position),"played_position","Play Position:");

	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_BackGround_Selected),"background_selected","Background Selected:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_ForeGround_Selected),"foreground_selected","Foreground Selected:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_Center_Selected),"center_selected","Center Selected:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_Loop_BackGround_Selected),"loop_background_selected","Loop Background Selected:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_Loop_ForeGround_Selected),"loop_foreground_selected","Loop Foreground Selected:");
	color_list->register_color(Sample_Viewer::get_color(Sample_Viewer::Col_Played_Position_Selected),"played_position_selected","Play Position Selected:");

	QWidget *desktop = QApplication::desktop();

	if( desktop )
	{
		move( (desktop->width() - width()) / 2, (desktop->height() - height()) / 2 );
	}

	ct_preset.add_item("pattern_edit","background",QColor(245,245,245));
	ct_preset.add_item("pattern_edit","foreground",QColor(61,61,61));
	ct_preset.add_item("pattern_edit","cursor",QColor(255,0,0));
	ct_preset.add_item("pattern_edit","hl_maj",QColor(216,216,216));
	ct_preset.add_item("pattern_edit","hl_min",QColor(176,176,176));
	ct_preset.add_item("pattern_edit","background_selected",QColor(149,180,227));
	ct_preset.add_item("pattern_edit","foreground_selected",QColor(0,0,0));
	ct_preset.add_item("pattern_edit","hl_selected",QColor(0,85,255));
	ct_preset.add_item("pattern_edit","sep",QColor(0,0,57));
	ct_preset.add_item("pattern_edit","sep_main",QColor(0,1,65));
	ct_preset.add_item("pattern_edit","mask",QColor(0,170,255));
	ct_preset.add_item("pattern_edit","foreground_inactive",QColor(171,171,171));
	ct_preset.add_item("pattern_edit","played_position",QColor(35,200,222));

	ct_preset.add_item("lists","background",QColor(245,245,245));
	ct_preset.add_item("lists","foreground",QColor(0,83,150));
	ct_preset.add_item("lists","cursor",QColor(255,0,0));
	ct_preset.add_item("lists","being_played",QColor(35,200,222));
	ct_preset.add_item("lists","separator",QColor(0,0,57));

	ct_preset.add_item("sample_edit","background",QColor(255,255,255));
	ct_preset.add_item("sample_edit","foreground",QColor(0,83,150));
	ct_preset.add_item("sample_edit","center",QColor(0,0,0));
	ct_preset.add_item("sample_edit","loop_background",QColor(255,0,0));
	ct_preset.add_item("sample_edit","loop_foreground",QColor(83,0,150));
	ct_preset.add_item("sample_edit","played_position",QColor(0,0,0));

	ct_preset.add_item("sample_edit","background_selected",QColor(0,83,150));
	ct_preset.add_item("sample_edit","foreground_selected",QColor(245,245,245));
	ct_preset.add_item("sample_edit","center_selected",QColor(150,150,150));
	ct_preset.add_item("sample_edit","loop_background_selected",QColor(83,0,150));
	ct_preset.add_item("sample_edit","loop_foreground_selected",QColor(255,0,0));
	ct_preset.add_item("sample_edit","played_position_selected",QColor(0,0,0));

	ct_preset.name="Discrete";

	color_list->add_preset(&ct_preset);


	it_preset.add_item("pattern_edit","background",QColor(10,20,30));
	it_preset.add_item("pattern_edit","foreground",QColor(100,200,100));
	it_preset.add_item("pattern_edit","cursor",QColor(250,250,250));
	it_preset.add_item("pattern_edit","hl_maj",QColor(50,60,70));
	it_preset.add_item("pattern_edit","hl_min",QColor(70,80,90));
	it_preset.add_item("pattern_edit","background_selected",QColor(10,50,10));
	it_preset.add_item("pattern_edit","foreground_selected",QColor(250,250,250));
	it_preset.add_item("pattern_edit","hl_selected",QColor(15,80,15));
	it_preset.add_item("pattern_edit","sep",QColor(150,150,150));
	it_preset.add_item("pattern_edit","sep_main",QColor(210,210,210));
	it_preset.add_item("pattern_edit","mask",QColor(250,100,100));
	it_preset.add_item("pattern_edit","foreground_inactive",QColor(70,110,70));
	it_preset.add_item("pattern_edit","played_position",QColor(255,255,255));

	it_preset.add_item("lists","background",QColor(10,20,30));
	it_preset.add_item("lists","foreground",QColor(100,200,100));
	it_preset.add_item("lists","cursor",QColor(250,250,250));
	it_preset.add_item("lists","being_played",QColor(255,255,255));
	it_preset.add_item("lists","separator",QColor(143,143,143));

	it_preset.add_item("sample_edit","background",QColor(0,0,0));
	it_preset.add_item("sample_edit","foreground",QColor(131,255,238));
	it_preset.add_item("sample_edit","center",QColor(0,0,170));
	it_preset.add_item("sample_edit","loop_background",QColor(250,250,255));
	it_preset.add_item("sample_edit","loop_foreground",QColor(0,0,255));
	it_preset.add_item("sample_edit","played_position",QColor(255,0,0));

	it_preset.add_item("sample_edit","background_selected",QColor(245,245,245));
	it_preset.add_item("sample_edit","foreground_selected",QColor(0,83,150));
	it_preset.add_item("sample_edit","center_selected",QColor(150,150,150));
	it_preset.add_item("sample_edit","loop_background_selected",QColor(255,0,0));
	it_preset.add_item("sample_edit","loop_foreground_selected",QColor(83,0,150));
	it_preset.add_item("sample_edit","played_position_selected",QColor(0,0,0));

	it_preset.name="Impulsive";

	color_list->add_preset(&it_preset);
        color_list->preset_selected_cbk(0);

	interface_settings = new Interface_Settings(sections);
	interface_settings->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	sections->addTab(interface_settings,QPixmap((const char**)icon_interface_xpm),"Interface");
        QObject::connect(button_close,SIGNAL(clicked()),this,SLOT(hide()));


}


Settings_Window::~Settings_Window()
{
}
