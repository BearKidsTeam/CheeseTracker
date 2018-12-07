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

#ifndef MDI_MAIN_WINDOW_H
#define MDI_MAIN_WINDOW_H

#include "interface__QT/interface.h"
#include <list>
#include <qaction.h>
#include <qworkspace.h>
#include <q3mainwindow.h>
#include <q3toolbar.h>
#include <q3popupmenu.h>
#include <qmenubar.h>
#include <qfile.h>
#include <q3filedialog.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <q3accel.h>
#include <qobject.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <QCloseEvent>
#include "interface__QT/settings_window.h"
#include "components/audio/sound_driver_manager.h"

using std::list;

class MDI_Sub_Window: public Q3MainWindow
{
    Q_OBJECT

    Interface *interface;

    Sound_Driver_Manager * sound_driver_manager;
    QStatusBar * status_bar;
    Q3VBox *vbox;

   string message_cache;

protected:
    void closeEvent( QCloseEvent * );

public:

	void set_info_lock(Mutex_Lock *p_lock);

	void update_display_box();

	void set_sound_driver_manager(Sound_Driver_Manager * p_sound_driver_manager);

	Interface *get_interface() { return interface; }

	void open_song(string p_name);
  MDI_Sub_Window( QWidget* parent, const char* name, Qt::WindowFlags wflags );
	~MDI_Sub_Window();

};

/**
 *
 * Juan Linietsky
 **/
class MDI_Main_Window: public Q3MainWindow
{
	Q_OBJECT

	MDI_Sub_Window* last_window;
	Interface::PageList last_window_page;

	string caption_cache;
	string caption_default;
        static int widget_timer_interval;
	int callback_count;

	list<QAction*> hidden_actions;

	QWorkspace* ws;

	Q3PopupMenu* windowsMenu;
	Q3ToolBar *file_menu_toolbar;
	Q3ToolBar *play_commands;
	Q3ToolBar *editing_octave_toolbar;
	CSpinButton *editing_octave;


	Q3PopupMenu *pattern_edit_menu;
	Q3PopupMenu *sample_edit_menu;
	Q3PopupMenu *instrument_edit_menu;
	Q3PopupMenu *settings_edit_menu;
	Q3PopupMenu *help_menu;

        enum PatternEditItemIDs {
		PatternEdit__Toggle_Last_Action=100,
		PatternEdit__Adjust_Volume_Column=101,
		PatternEdit__Toggle_Song_Follow=102,
		PatternEdit__EraseAll=103,
		PatternEdit__EraseOrders=104
	};

        enum SampleEditItemIDs {
		SampleEdit__Copy,
		SampleEdit__Cut,
		SampleEdit__Paste,
		SampleEdit__Delete,
		SampleEdit__Load,
		SampleEdit__Save,

		SampleEdit__MakeInstrument,
		SampleEdit__ClearUnused,
		SampleEdit__ClearDefaultPanning

	};

        enum InstrumentEditItemIDs {
		InstrumentEdit__Copy,
		InstrumentEdit__Cut,
		InstrumentEdit__Paste,
		InstrumentEdit__Delete,
		InstrumentEdit__Delete_With_Samples,
		InstrumentEdit__Load,
		InstrumentEdit__Save,

		InstrumentEdit__RecreateAllInstruments,
		InstrumentEdit__ClearDefaultPannings,

	};

        enum SettingsItemIDs {
		Settings__Display_Play_ToolBox,
		Settings__Display_Octave_ToolBox,
		Settings__Display_File_ToolBox,
		Settings__Display_Settings
	};

        enum HelpItemIDs {

		Help__Pattern,
		Help__About
	};


        enum MenuBarItemID {

		FileItemID,
		PatternEditItemID,
		SampleEditItemID,
		InstrumentEditItemID,
		SongEditItemID,
		SettingsItemID,
		WindowItemID,
		HelpItemID
	};

        QTimer *timer;

	bool showing_play_toolbar;
	bool showing_octave_toolbar;
	bool showing_file_toolbar;

	Settings_Window *settings_window;

	Sound_Driver_Manager * sound_driver_manager;

	Mutex_Lock *info_lock;
	
private slots:

	void timer_callback();
	void default_octave_changed_cbk(int p_to_which);
	MDI_Sub_Window * newDoc();

	void ctrl_i_make_instrument();
	void pattern_edit_menu_item_activated(int p_id);
	void instrument_edit_menu_item_activated(int p_id);
	void sample_edit_menu_item_activated(int p_id);
	void tools_menu_item_activated(int p_id);
	void about_to_show_tools_menu();
	void help_menu_item_activated(int p_id);


	void load();
	void save();
	void save_as();
//	void print();
	void close_window();
//	void tile_horizontal();

	void about();

	void windows_menu_item_active(int p_which);
	void about_to_show_window_menu();

        void play_prev_action_cbk();
	void play_action_cbk();
	void stop_action_cbk();
	void play_next_action_cbk();
	void play_pattern_action_cbk();
	void play_pattern_cursor_action_cbk();
	void play_song_cursor_action_cbk();


//	void windowsMenuAboutToShow();
//	void windowsMenuActivated( int id );

protected:

	void closeEvent( QCloseEvent * );

public:
	Settings_Window * get_settings_window() { return settings_window; }

	void set_sound_driver_manager(Sound_Driver_Manager * p_sound_driver_manager);
	void set_info_lock(Mutex_Lock *p_lock) { info_lock=p_lock; }
	void open_song(string p_song);
	MDI_Main_Window();
	~MDI_Main_Window();


};

#endif
