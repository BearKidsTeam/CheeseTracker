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
// Maintainer: J. Phelps <phelpsj@nuvox.net>
// AKA Godless Infidel <godless@users.sf.net> 2007 (Copyright still held
// by Linietsky)
//
// Copyright: See COPYING file that comes with this distribution
//
//

#define APP_NAME_ "CheeseTracker "
#define APP_VERSION_ "0.9.15.1"
#define APP_AUTHOR_ "(c) 1999,2004 Juan Linietsky"
#define APP_RELEASE_DATE_ "Tue Aug  7 15:42:17 EDT 2007"
#define APP_MAINTAINER_ "J Phelps <godless@users.sf.net>"
#define APP_WEBSITE_ "http://cheesetracker.sf.net/"

#include <QDesktopWidget>
#include "mdi_main_window.h"
#include "interface__QT/icons/control_play_prev.xpm"
#include "interface__QT/icons/control_play.xpm"
#include "interface__QT/icons/control_stop.xpm"
//#include "interface__QT/icons/control_pause.xpm"
#include "interface__QT/icons/control_play_next.xpm"
#include "interface__QT/icons/control_play_pattern.xpm"
#include "interface__QT/icons/control_play_pattern_all.xpm"

#include "interface__QT/icons/file_disk_new_song.xpm"
#include "interface__QT/icons/file_disk_open.xpm"
#include "interface__QT/icons/file_disk_saveas.xpm"
#include "interface__QT/icons/file_disk.xpm"
#include "interface__QT/icons/file_disk_close.xpm"

#include "interface_binds/interface_help_text.h"
#include "trackercore/Error.h"

#include "icons/cheese_48x48.xpm"
//#include "icons/cheese_16x16.xpm"

#include <interface__QT/popups/text_area_popup.h>


#include "interface__QT/popups/cspindialog.h"
#include <qaction.h>
//Added by qt3to4:
#include <QLabel>
#include <QPixmap>
#include <Q3Frame>
#include <Q3PopupMenu>
#include <QCloseEvent>

void MDI_Sub_Window::closeEvent( QCloseEvent *  e) {

	//just ugly :)
	switch( QMessageBox::warning( this, "Closing..",tr("Really close \"%1\"?").arg( caption() ),tr("Yes"), tr("No") ) ) {
		case 0:{
			e->accept();
		} break;
		case 1: {
			e->ignore();
		} break;
		default: {
			e->ignore();
		} break;
	}
}

void MDI_Sub_Window::set_sound_driver_manager(Sound_Driver_Manager * p_sound_driver_manager) {

	sound_driver_manager=p_sound_driver_manager;
	sound_driver_manager->add_mixer(&interface->get_tracker()->mixer);
	interface->get_pattern_editor()->set_variables_lock(sound_driver_manager->get_variables_lock());
	interface->get_tracker()->player.link_variables_lock(sound_driver_manager->get_variables_lock());
	interface->get_instrument_editor()->set_variables_lock(sound_driver_manager->get_variables_lock());

}

void MDI_Sub_Window::set_info_lock(Mutex_Lock *p_lock) {

	interface->get_tracker()->player.link_info_lock(p_lock);
}

void MDI_Sub_Window::update_display_box() {

 	static char aux_msg_buffer[200];
	int aux_playing_channels;

	aux_msg_buffer[199]=0;

	aux_playing_channels=interface->get_tracker()->mixer.get_active_voices_count();
	int aux_playing_buffers=interface->get_tracker()->mixer.get_active_buffers_count();


	Player_Data *player=&interface->get_tracker()->player;
	Song * song=&interface->get_tracker()->song;

	 if (player->is_playing()) {

		if (player->get_current_pattern() != EMPTY_FIELD &&
		    player->get_play_mode()==Player_Data::PLAY_SONG) {

			snprintf(aux_msg_buffer, sizeof(aux_msg_buffer)-1,"Status: Playing, Order: %i/%i, Pattern %i, Row %i/%i, %i Voices, %i Buffers.",player->get_current_order(),song->get_used_orders_amount(),player->get_current_pattern(),player->get_current_row(),song->get_pattern(player->get_current_pattern())->get_length(),aux_playing_channels,aux_playing_buffers);
		} else {

			if(player->get_current_pattern() != EMPTY_FIELD)
				snprintf(aux_msg_buffer, sizeof(aux_msg_buffer)-1, "Status: Playing, Pattern %i, Row %i/%i, %i Voices, %i Buffers.",player->get_current_pattern(),player->get_current_row(),song->get_pattern(player->get_current_pattern())->get_length(),aux_playing_channels,aux_playing_buffers);
			else
				snprintf(aux_msg_buffer, sizeof(aux_msg_buffer)-1,
				"Status: Playing an empty pattern\n");
		}

	} else if (aux_playing_channels>0) {

		snprintf(aux_msg_buffer, sizeof(aux_msg_buffer)-1, "Status: Active, %i Voices, %i Buffers.",aux_playing_channels,aux_playing_buffers);

	} else {

		if (aux_playing_buffers)
			snprintf(aux_msg_buffer, sizeof(aux_msg_buffer)-1, "Status: Idle, %i Buffers.",aux_playing_buffers);
		else
			snprintf(aux_msg_buffer, sizeof(aux_msg_buffer)-1, "Status: Idle.");
	}

	if (message_cache!=aux_msg_buffer) {

		message_cache=aux_msg_buffer;
                status_bar->message(aux_msg_buffer);
	}
}

MDI_Sub_Window::MDI_Sub_Window( QWidget* parent, const char* name, Qt::WindowFlags wflags ) : Q3MainWindow( parent, name, wflags )
{
	vbox = new Q3VBox(this);
	status_bar = new QStatusBar(vbox);
	interface = new Interface(vbox);
	interface->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	setFocusProxy(interface);
	setCentralWidget(vbox);
	status_bar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	status_bar->setSizeGripEnabled(false);
	setIcon(QPixmap((const char**)file_disk_new_song_xpm));

}

void MDI_Sub_Window::open_song(string p_name) {

	try {
		Loader::Error  res=interface->open_song(p_name);
		if (res) {

			QMessageBox::critical( this, "Error Loading Song",Loader::error_string[res],"Ok :(");
			close();
		}
	}
	catch (Error E) {
		QMessageBox::critical(this, "Error Loading Song", E.what(), "Ok :(");
		close();
	}

}

MDI_Sub_Window::~MDI_Sub_Window() {

	sound_driver_manager->remove_mixer(&interface->get_tracker()->mixer);

}



/** MAIN MDI WINDOW */

int MDI_Main_Window::widget_timer_interval=50;

void MDI_Main_Window::default_octave_changed_cbk(int p_to_which) {

	Editor::set_default_octave(p_to_which);
}

void MDI_Main_Window::timer_callback() {

	callback_count++;

	if (callback_count%3) {
		QWidgetList windows = ws->windowList();
		for ( int i = 0; i < int( windows.count() ); ++i ) {
			MDI_Sub_Window *window = dynamic_cast<MDI_Sub_Window*>(windows.at( i ));
			if (!window)
				continue;

			window->update_display_box();
		}
	}

	//widgets from now on dont need to be updated as fast
	if (callback_count%5)
		return;
	/* Check the octave SpinBox */
	if (editing_octave->value()!=Editor::get_default_octave())
		editing_octave->setValue(Editor::get_default_octave());

	/* Check if the song name is OK */
	QWidgetList windows = ws->windowList();
	for ( int i = 0; i < int( windows.count() ); ++i ) {
		MDI_Sub_Window *window = dynamic_cast<MDI_Sub_Window*>(windows.at( i ));
		if (!window)
			continue;

		if (window->caption()!=window->get_interface()->get_song_name().c_str()) {

			window->setCaption(window->get_interface()->get_song_name().c_str());
		}
	}

	MDI_Sub_Window* window = dynamic_cast<MDI_Sub_Window*>(ws->activeWindow());
	if ( window ) {
		string new_caption=((string)APP_NAME_+(string)APP_VERSION_);

		if (caption_cache!=new_caption.c_str()) {
			caption_cache=new_caption;
			setCaption(new_caption.c_str());
		}
	} else {
        	if (caption_cache!=caption_default) {

			setCaption(caption_default.c_str());
			caption_cache=caption_default;
		}
	}

	if ( (last_window!=window) || (window && (last_window_page!=window->get_interface()->get_current_page()))) {

		if (window==NULL) {

			menuBar()->setItemEnabled(PatternEditItemID,false);
			menuBar()->setItemEnabled(SampleEditItemID,false);
			menuBar()->setItemEnabled(InstrumentEditItemID,false);

		} else {

			menuBar()->setItemEnabled(PatternEditItemID,window->get_interface()->get_current_page()==Interface::Page__Pattern_Editor);
			menuBar()->setItemEnabled(SampleEditItemID,window->get_interface()->get_current_page()==Interface::Page__Sample_Editor);
			menuBar()->setItemEnabled(InstrumentEditItemID,window->get_interface()->get_current_page()==Interface::Page__Instrument_Editor);
		}
	}

	last_window=window;
	if (window)
		last_window_page=window->get_interface()->get_current_page();

}

void MDI_Main_Window::ctrl_i_make_instrument()
{
	sample_edit_menu_item_activated(SampleEdit__MakeInstrument);
}

void MDI_Main_Window::pattern_edit_menu_item_activated(int p_id) {

	if (last_window==NULL)
		return;
	if (last_window_page!=Interface::Page__Pattern_Editor)
		return;


	if (p_id==PatternEdit__Toggle_Last_Action) {

        	last_window->get_interface()->get_tracker()->editor.undo_index(0);
	} else if (p_id==PatternEdit__Adjust_Volume_Column) {

		//should put this somewhere else, but i'm lazy
		CSpinDialog * ask_value = new CSpinDialog(this);
		ask_value->setCaption("Volume Amplifier");
		ask_value->set_description("Amplify Volume by:");
		ask_value->get_spin()->setMaxValue(200);
		ask_value->get_spin()->setSuffix("%");
		ask_value->get_spin()->setValue(100);

		if (ask_value->exec() == QDialog::Accepted) {

			int ramp_result=ask_value->get_spin()->value();
			last_window->get_interface()->get_tracker()->editor.selection_volume_scale(ramp_result);
		}

		delete ask_value;
	} else if (p_id==PatternEdit__Toggle_Song_Follow) {

		last_window->get_interface()->get_tracker()->editor.set_song_follow(
			!last_window->get_interface()->get_tracker()->editor.is_song_follow()
		);
	} else if (p_id==PatternEdit__EraseAll) {

		if (QMessageBox::warning( this, "Really?",tr("This will erase all the patterns!.\nAre you sure?"),tr("Yes"), tr("No") ) )
			return;
		last_window->get_interface()->get_tracker()->player.play_stop();
		last_window->get_interface()->get_tracker()->song.clear_all_patterns();
		last_window->get_interface()->get_pattern_editor()->reupdate_components();

	} else if (p_id==PatternEdit__EraseOrders) {

		if (QMessageBox::warning( this, "Really?",tr("This will erase the Order List!.\nAre you sure?"),tr("Yes"), tr("No") ) )
			return;

		last_window->get_interface()->get_tracker()->player.play_stop();
		last_window->get_interface()->get_tracker()->song.clear_all_orders();
	} else {
		last_window->get_interface()->get_tracker()->editor.perform_action((Editor::Pattern_Actions)p_id);
	}

	last_window->get_interface()->update_pattern_editor();
}

void MDI_Main_Window::sample_edit_menu_item_activated(int p_id) {

	if (last_window==NULL)
		return;
	if (last_window_page!=Interface::Page__Sample_Editor)
		return;

	switch (p_id) {

        	case SampleEdit__Copy: {

			last_window->get_interface()->get_sample_editor()->copy_sample();
		} break;
		case SampleEdit__Cut:  {
			last_window->get_interface()->get_sample_editor()->cut_sample();

		} break;
		case SampleEdit__Paste: {
			last_window->get_interface()->get_sample_editor()->paste_sample();

		} break;
		case SampleEdit__Delete: {

			last_window->get_interface()->get_sample_editor()->delete_sample();
		} break;
		case SampleEdit__Load: {

			last_window->get_interface()->get_sample_editor()->load_sample();

		} break;
		case SampleEdit__Save: {

			last_window->get_interface()->get_sample_editor()->save_sample();

		} break;
		case SampleEdit__MakeInstrument: {

			if (QMessageBox::warning( this, "Really?",tr("This will create a new instrument ouf ot this sample.\nAre you sure?"),tr("Yes"), tr("No") ) )
				break;

			last_window->get_interface()->get_tracker()->player.play_stop();

			last_window->get_interface()->get_tracker()->song.make_instrument_out_of_sample(
				last_window->get_interface()->get_sample_editor()->get_selected_sample_index()
			);
			last_window->get_interface()->get_instrument_editor()->update();



		} break;
		case SampleEdit__ClearUnused: {

			if (QMessageBox::warning( this, "Really?",tr("This will erase all the samples unused within the song.\nAre you sure?"),tr("Yes"), tr("No") ) )
        			break;

			last_window->get_interface()->get_tracker()->player.play_stop();
			last_window->get_interface()->get_tracker()->song.clear_unused_samples();
			last_window->get_interface()->get_sample_editor()->update_samples();
			last_window->get_interface()->get_sample_editor()->update_selected_sample();


		} break;
		case SampleEdit__ClearDefaultPanning: {

			if (QMessageBox::warning( this, "Really?",tr("This will erase ALL default pannings.\nAre you sure?"),tr("Yes"), tr("No") ) )
        			break;
			last_window->get_interface()->get_tracker()->song.clear_default_sample_pannings();
			last_window->get_interface()->get_sample_editor()->update_samples();
			last_window->get_interface()->get_sample_editor()->update_selected_sample();

		} break;
	};
}

void MDI_Main_Window::instrument_edit_menu_item_activated(int p_id) {

	if (last_window==NULL)
		return;
	if (last_window_page!=Interface::Page__Instrument_Editor)
		return;

	switch (p_id) {

        	case InstrumentEdit__Copy: {

			last_window->get_interface()->get_instrument_editor()->copy_instrument();
		} break;
		case InstrumentEdit__Cut:  {
			last_window->get_interface()->get_instrument_editor()->cut_instrument();

		} break;
		case InstrumentEdit__Paste: {
			last_window->get_interface()->get_instrument_editor()->paste_instrument();

		} break;
		case InstrumentEdit__Delete: {

			if (QMessageBox::warning( this, "Really?",tr("This will erase the instrument.\nAre you sure?"),tr("Yes"), tr("No") ) )
				break;

			last_window->get_interface()->get_instrument_editor()->delete_instrument();
		} break;
		case InstrumentEdit__Delete_With_Samples: {

			last_window->get_interface()->get_instrument_editor()->delete_instrument_with_samples();
			last_window->get_interface()->get_sample_editor()->update_samples();
			last_window->get_interface()->get_sample_editor()->update_selected_sample();
		} break;
		case InstrumentEdit__Load: {

			last_window->get_interface()->get_instrument_editor()->load_instrument();

		} break;
		case InstrumentEdit__Save: {

			last_window->get_interface()->get_instrument_editor()->save_instrument();

		} break;
		case InstrumentEdit__RecreateAllInstruments: {

			if (QMessageBox::warning( this, "Really?",tr("This will erase ALL the instruments and\ncreate them from their respective samples.\nAre you sure?"),tr("Yes"), tr("No") ) )
				break;
			last_window->get_interface()->get_tracker()->player.play_stop();
			last_window->get_interface()->get_tracker()->song.recreate_all_instruments();
			last_window->get_interface()->get_instrument_editor()->update();

		} break;
		case InstrumentEdit__ClearDefaultPannings: {
			if (QMessageBox::warning( this, "Really?",tr("This will clear the default pannings on all instruments and layers\nAre you sure?"),tr("Yes"), tr("No") ) )
				break;

			last_window->get_interface()->get_tracker()->song.clear_default_instrument_pannings();
			last_window->get_interface()->get_instrument_editor()->update();
		} break;
	};


}


void MDI_Main_Window::windows_menu_item_active(int p_which) {

   QWidget* w = ws->windowList().at( p_which );
    if ( w ) {
        w->showNormal();
        w->setFocus();
    }
}

void MDI_Main_Window::play_prev_action_cbk() {

	MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
	if ( !m )
		return;

	m->get_interface()->play_previous_order();

	//m->get_n

}
void MDI_Main_Window::play_action_cbk()
{
	MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
	if ( !m ) {
		return;
	}

	m->get_interface()->play_song();

}
void MDI_Main_Window::stop_action_cbk() {

	MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
	if ( !m )
		return;

	m->get_interface()->stop_song();

}
void MDI_Main_Window::play_next_action_cbk() {

	MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
	if ( !m )
		return;

	m->get_interface()->play_next_order();


}
void MDI_Main_Window::play_pattern_action_cbk() {

	MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
	if ( !m )
		return;

	m->get_interface()->play_pattern();

}


void MDI_Main_Window::play_pattern_cursor_action_cbk() {


	MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
	if ( !m )
		return;

	m->get_interface()->play_pattern_from_cursor();

}

void MDI_Main_Window::play_song_cursor_action_cbk() {


	MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
	if ( !m )
		return;

	m->get_interface()->play_from_cursor();

}


void MDI_Main_Window::about_to_show_window_menu() {

	windowsMenu->clear();

	QWidgetList windows = ws->windowList();
	for ( int i = 0; i < int(windows.count()); ++i ) {
		int id = windowsMenu->insertItem(windows.at(i)->caption(),
						this, SLOT( windows_menu_item_active( int ) ) );
		windowsMenu->setItemParameter( id, i );
		windowsMenu->setItemChecked( id, ws->activeWindow() == windows.at(i) );
	}
}

void MDI_Main_Window::tools_menu_item_activated(int p_id)
{

	switch (p_id) {

		case Settings__Display_Play_ToolBox: {

			if (play_commands->isVisible())
				play_commands->hide();
			else
				play_commands->show();

		} break;
		case Settings__Display_Octave_ToolBox: {
			if (editing_octave_toolbar->isVisible())
				editing_octave_toolbar->hide();
			else
				editing_octave_toolbar->show();

		} break;
		case Settings__Display_File_ToolBox: {

			if (file_menu_toolbar->isVisible())
				file_menu_toolbar->hide();
			else
				file_menu_toolbar->show();


		} break;
		case Settings__Display_Settings: {

			settings_window->raise();
			settings_window->showNormal();
			settings_window->setFocus();
		} break;
	}
}
void MDI_Main_Window::about_to_show_tools_menu()
{

	settings_edit_menu->clear();
	settings_edit_menu->setCheckable(true);
	settings_edit_menu->insertItem("Show Play Toolbar",Settings__Display_Play_ToolBox);
	settings_edit_menu->insertItem("Show Editing Octave Toolbar",Settings__Display_Octave_ToolBox);
	settings_edit_menu->insertItem("Show File Operations Toolbar",Settings__Display_File_ToolBox);
	settings_edit_menu->setItemChecked( Settings__Display_Play_ToolBox, play_commands->isVisible() );
	settings_edit_menu->setItemChecked( Settings__Display_Octave_ToolBox, editing_octave_toolbar->isVisible() );
	settings_edit_menu->setItemChecked( Settings__Display_File_ToolBox, file_menu_toolbar->isVisible() );

	settings_edit_menu->insertSeparator();
	settings_edit_menu->insertItem("Configure CheeseTracker",Settings__Display_Settings);



}


MDI_Main_Window::MDI_Main_Window() : Q3MainWindow( 0, "Main Window"/*, WDestructiveClose*/ )
{

	caption_default=(string)APP_NAME_+(string)APP_VERSION_+" "+(string)APP_AUTHOR_;

        
	//QObject::connect( windowsMenu, SIGNAL( aboutToShow() ), this, SLOT( windowsMenuAboutToShow() ) );

	file_menu_toolbar = new Q3ToolBar(this,"file menu");
	addToolBar( file_menu_toolbar, tr( "File Options" ), Qt::DockTop, false );
	play_commands = new Q3ToolBar( this, "player control" );
	addToolBar( play_commands, tr( "Play Control" ), Qt::DockTop, false );
	Q3PopupMenu * file = new Q3PopupMenu( this );
	menuBar()->insertItem( "File", file,FileItemID );


	QAction * file_new_action = new QAction(QPixmap((const char**)file_disk_new_song_xpm),"&New Song",Qt::CTRL+Qt::Key_N,this, "New Song");
	QObject::connect( file_new_action, SIGNAL( activated() ) , this, SLOT(newDoc()) );
	file_menu_toolbar->addAction(file_new_action);
	file->addAction(file_new_action);

//	file->insertItem( "&New", this, SLOT(newDoc()), CTRL+Key_N );

//	id = file->insertItem( openIcon, "&Open...",this, SLOT(load()), CTRL+Key_O );
//	id = file->insertItem(  "&Open...",this, SLOT(load()), CTRL+Key_O );
	QAction * file_open_action = new QAction(QPixmap((const char**)file_disk_open_xpm),"&Load...",Qt::CTRL+Qt::Key_L,this,"Open Song");
	QObject::connect( file_open_action, SIGNAL( activated() ) , this, SLOT(load()) );
	file_menu_toolbar->addAction(file_open_action);
	file->addAction(file_open_action);

	// F9_file_open_action is a hidden action: It has a
	// key binding, but no corresponding menu entry.

	QAction * F9_file_open_action =
		new QAction(QPixmap((const char**)file_disk_open_xpm), "",
		            Qt::Key_F9, this, "Open Song");

	QObject::connect(F9_file_open_action, SIGNAL( activated() ), this,
	                 SLOT(load()) );

	this->addAction(F9_file_open_action);

	// QT3: Hidden actions must go into the hidden action list to prevent
	// a memory leak.

	// hidden_actions.push_back(F9_file_open_action);


	// Make Ctrl+I load an instrument. This requires a QAction with
	// no associated menu item.

	QAction * file_save_action = new QAction(QPixmap((const char**)file_disk_xpm),"&Save",Qt::CTRL+Qt::Key_S,this,"Save");
	QObject::connect( file_save_action, SIGNAL( activated() ) , this, SLOT(save()) );
	file_menu_toolbar->addAction(file_save_action);
	file->addAction(file_save_action);

	QAction * file_saveas_action = new QAction(QPixmap((const char**)file_disk_saveas_xpm),"Save &As..",Qt::CTRL+Qt::ALT+Qt::Key_S,this,"Save As");
	QObject::connect( file_saveas_action, SIGNAL( activated() ) , this, SLOT(save_as()) );
	file_menu_toolbar->addAction(file_saveas_action);
	file->addAction(file_saveas_action);

	file->insertSeparator();

//	file->insertItem( "&Close", this, SLOT(close_window()), CTRL+Key_W );
	QAction * file_close_action = new QAction(QPixmap((const char**)file_disk_close_xpm),"&Close",Qt::CTRL+Qt::Key_W,this,"Close");
	QObject::connect( file_close_action, SIGNAL( activated() ) , this, SLOT(close_window()) );
	file_menu_toolbar->addAction(file_close_action);
	file->addAction(file_close_action);

	file->insertItem( "&Quit", qApp, SLOT( closeAllWindows() ), Qt::CTRL+Qt::Key_Q );

        /* EDIT Section */

	pattern_edit_menu = new Q3PopupMenu( this );
	menuBar()->insertItem( "Pattern", pattern_edit_menu,PatternEditItemID );

	pattern_edit_menu->insertItem("Undo/Redo Last Action",PatternEdit__Toggle_Last_Action);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_BackSpace ,PatternEdit__Toggle_Last_Action);
	pattern_edit_menu->insertSeparator();
	pattern_edit_menu->insertItem("Mark- Block Begin",Editor::Pattern_MARK_BLOCK_BEGIN);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_B ,Editor::Pattern_MARK_BLOCK_BEGIN);
	pattern_edit_menu->insertItem("Mark- Block End",Editor::Pattern_MARK_BLOCK_END);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_E ,Editor::Pattern_MARK_BLOCK_END);
	pattern_edit_menu->insertItem("Mark- quarter/half pattern length notes", Editor::Pattern_MARK_COLUMN_QUARTER_HALF);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_D, Editor::Pattern_MARK_COLUMN_QUARTER_HALF);
	pattern_edit_menu->insertItem("Mark- Toggle Column/All",Editor::Pattern_MARK_COLUMN_ALL);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_L ,Editor::Pattern_MARK_COLUMN_ALL);
	pattern_edit_menu->insertSeparator();
	pattern_edit_menu->insertItem("Block- Copy",Editor::Pattern_BLOCK_COPY);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_C ,Editor::Pattern_BLOCK_COPY);
	pattern_edit_menu->insertItem("Block- Paste Insert",Editor::Pattern_BLOCK_PASTE_INSERT);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_P ,Editor::Pattern_BLOCK_PASTE_INSERT);
	pattern_edit_menu->insertItem("Block- Paste Overwrite",Editor::Pattern_BLOCK_PASTE_OVERWRITE);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_O ,Editor::Pattern_BLOCK_PASTE_OVERWRITE);
	pattern_edit_menu->insertItem("Block- UnMark",Editor::Pattern_BLOCK_UNMARK);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_U ,Editor::Pattern_BLOCK_UNMARK);
	pattern_edit_menu->insertItem("Block- Cut",Editor::Pattern_BLOCK_CUT);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_Z ,Editor::Pattern_BLOCK_CUT);
	pattern_edit_menu->insertSeparator();
	pattern_edit_menu->insertItem("Block- Raise Notes",Editor::Pattern_BLOCK_RAISE_NOTES);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_Q ,Editor::Pattern_BLOCK_RAISE_NOTES);
	pattern_edit_menu->insertItem("Block- Lower Notes",Editor::Pattern_BLOCK_LOWER_NOTES);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_A ,Editor::Pattern_BLOCK_LOWER_NOTES);
	pattern_edit_menu->insertItem("Block- Set Current Instrument",Editor::Pattern_BLOCK_SET_CURRENT_INSTRUMENT);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_S ,Editor::Pattern_BLOCK_SET_CURRENT_INSTRUMENT);
	pattern_edit_menu->insertItem("Block- Set Current Volume/Pan",Editor::Pattern_BLOCK_SET_CURRENT_VOLUME);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_V ,Editor::Pattern_BLOCK_SET_CURRENT_VOLUME);
	pattern_edit_menu->insertItem("Block- Remove Stray Volumes",Editor::Pattern_BLOCK_WIPE_STRAY_VOLUMES);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_W ,Editor::Pattern_BLOCK_WIPE_STRAY_VOLUMES);
	pattern_edit_menu->insertItem("Block- Volume Amplifier",PatternEdit__Adjust_Volume_Column);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_J ,PatternEdit__Adjust_Volume_Column);
	pattern_edit_menu->insertItem("Block- Ramp or Wipe(2xPress) Volumes/Panings",Editor::Pattern_BLOCK_RAMP_WIPE_VOLUMES);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_K ,Editor::Pattern_BLOCK_RAMP_WIPE_VOLUMES);
	pattern_edit_menu->insertItem("Block- Ramp or Wipe(2xPress) Effects",Editor::Pattern_BLOCK_RAMP_WIPE_EFFECTS);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_X ,Editor::Pattern_BLOCK_RAMP_WIPE_EFFECTS);
	pattern_edit_menu->insertItem("Block- Double Block Length",Editor::Pattern_BLOCK_DOUBLE_BLOCK_LENGTH);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_G ,Editor::Pattern_BLOCK_DOUBLE_BLOCK_LENGTH);
	pattern_edit_menu->insertItem("Block- Halve Block Length",Editor::Pattern_BLOCK_HALVE_BLOCK_LENGTH);
	pattern_edit_menu->setAccel(Qt::ALT+Qt::Key_F ,Editor::Pattern_BLOCK_HALVE_BLOCK_LENGTH);
	pattern_edit_menu->insertSeparator();
	pattern_edit_menu->insertItem("Cursor Field- Toggle Mask",Editor::Pattern_FIELD_TOGGLE_MASK);
	pattern_edit_menu->setAccel(Qt::Key_Comma ,Editor::Pattern_FIELD_TOGGLE_MASK);
	pattern_edit_menu->insertItem("Cursor Field- Clear",Editor::Pattern_FIELD_CLEAR);
	pattern_edit_menu->setAccel(Qt::Key_Period ,Editor::Pattern_FIELD_CLEAR);
	pattern_edit_menu->insertItem("Cursor Field- Copy Current",Editor::Pattern_FIELD_COPY_CURRENT);
	pattern_edit_menu->setAccel(Qt::Key_Enter ,Editor::Pattern_FIELD_COPY_CURRENT);
	pattern_edit_menu->insertItem("Cursor Field- Paste Current",Editor::Pattern_FIELD_PASTE_CURRENT);
	pattern_edit_menu->setAccel(Qt::Key_Space ,Editor::Pattern_FIELD_PASTE_CURRENT);
	pattern_edit_menu->insertSeparator();
	pattern_edit_menu->insertItem("Toggle Cursor Follows Song",PatternEdit__Toggle_Song_Follow);
	pattern_edit_menu->insertSeparator();
	pattern_edit_menu->insertItem("Clear ALL Patterns",PatternEdit__EraseAll);
	pattern_edit_menu->insertItem("Clear Orderlist",PatternEdit__EraseOrders);
	pattern_edit_menu->setAccel(Qt::Key_ScrollLock ,PatternEdit__Toggle_Song_Follow);


	QObject::connect(pattern_edit_menu,SIGNAL(activated(int)),this,SLOT(pattern_edit_menu_item_activated(int)));

        /* Sample Edit */

	sample_edit_menu = new Q3PopupMenu( this );
	menuBar()->insertItem( "Sample", sample_edit_menu,SampleEditItemID );

	sample_edit_menu->insertItem("Sample- Copy",SampleEdit__Copy);
	sample_edit_menu->insertItem("Sample- Cut",SampleEdit__Cut);
	sample_edit_menu->insertItem("Sample- Paste",SampleEdit__Paste);
	sample_edit_menu->insertItem("Sample- Delete",SampleEdit__Delete);
	sample_edit_menu->insertSeparator();
	sample_edit_menu->insertItem("Sample- Load",SampleEdit__Load);
	sample_edit_menu->insertItem("Sample- Save",SampleEdit__Save);
	sample_edit_menu->insertSeparator();
	sample_edit_menu->insertItem("Sample- Make Instrument",SampleEdit__MakeInstrument);
	sample_edit_menu->setAccel(Qt::CTRL+Qt::Key_I, SampleEdit__MakeInstrument);
	sample_edit_menu->insertSeparator();
	sample_edit_menu->insertItem("Sample- Clear Unused Samples",SampleEdit__ClearUnused);
	sample_edit_menu->insertItem("Sample- Clear Default Pannings",SampleEdit__ClearDefaultPanning);
	QObject::connect(sample_edit_menu,SIGNAL(activated(int)),this,SLOT(sample_edit_menu_item_activated(int)));


        /* Instrument Edit */

	instrument_edit_menu = new Q3PopupMenu( this );
	menuBar()->insertItem( "Instrument", instrument_edit_menu,InstrumentEditItemID );

	instrument_edit_menu->insertItem("Instrument- Copy",InstrumentEdit__Copy);
	instrument_edit_menu->insertItem("Instrument- Cut",InstrumentEdit__Cut);
	instrument_edit_menu->insertItem("Instrument- Paste",InstrumentEdit__Paste);
	instrument_edit_menu->insertItem("Instrument- Delete",InstrumentEdit__Delete);
	instrument_edit_menu->insertItem("Instrument- Delete (samples too)",InstrumentEdit__Delete_With_Samples);
	instrument_edit_menu->insertSeparator();
	instrument_edit_menu->insertItem("Instrument- Load",InstrumentEdit__Load);
	instrument_edit_menu->insertItem("Instrument- Save",InstrumentEdit__Save);
	instrument_edit_menu->insertSeparator();
	instrument_edit_menu->insertItem("Instrument- Recreate ALL from Samples",InstrumentEdit__RecreateAllInstruments);
	instrument_edit_menu->insertItem("Instrument- Clear Default Pannings",InstrumentEdit__ClearDefaultPannings);


	QObject::connect(instrument_edit_menu,SIGNAL(activated(int)),this,SLOT(instrument_edit_menu_item_activated(int)));

        /* Toolbar play */

	Q3PopupMenu* play_control_menu = new Q3PopupMenu( this );
	menuBar()->insertItem( "Song", play_control_menu,SongEditItemID );


	QAction * play_prev_action = new QAction(QPixmap((const char**)control_play_prev_xpm),"Skip To P&revious Order",Qt::ALT+Qt::Key_Minus,this,"Skip To Previous Order");
	QObject::connect( play_prev_action, SIGNAL( activated() ) , this, SLOT( play_prev_action_cbk() ) );
	play_commands->addAction(play_prev_action);
	play_control_menu->addAction(play_prev_action);

	QAction * play_action = new QAction(QPixmap((const char**)control_play_xpm),"&Play Song",Qt::Key_F5,this,"Play Song");
	QObject::connect( play_action, SIGNAL( activated() ) , this, SLOT( play_action_cbk() ) );
	play_commands->addAction(play_action);
	play_control_menu->addAction(play_action);

	QAction * stop_action = new QAction(QPixmap((const char**)control_stop_xpm),"&Stop",Qt::Key_F8,this,"Stop");
	QObject::connect( stop_action, SIGNAL( activated() ) , this, SLOT( stop_action_cbk() ) );
	play_commands->addAction(stop_action);
	play_control_menu->addAction(stop_action);

	QAction * play_next_action = new QAction(QPixmap((const char**)control_play_next_xpm),"Skip To &Next Order",Qt::ALT+Qt::Key_Plus,this,"Skip To Next Order");
	QObject::connect( play_next_action, SIGNAL( activated() ) , this, SLOT( play_next_action_cbk() ) );
	play_commands->addAction(play_next_action);
	play_control_menu->addAction(play_next_action);

	play_commands->addSeparator();
	play_control_menu->insertSeparator();

	QAction * play_pattern_action = new QAction(QPixmap((const char**)control_play_pattern_xpm),"Play Pa&ttern",Qt::Key_F6,this,"Play Pattern");
	QObject::connect( play_pattern_action, SIGNAL( activated() ) , this, SLOT( play_pattern_action_cbk() ) );
	play_commands->addAction(play_pattern_action);
	play_control_menu->addAction(play_pattern_action);

	QAction * play_song_cursor_action = new QAction(QPixmap((const char**)control_play_pattern_all_xpm),"Play Song From &Cursor",Qt::Key_F7,this,"Play Song From Cursor");
	QObject::connect( play_song_cursor_action, SIGNAL( activated() ) , this, SLOT( play_song_cursor_action_cbk() ) );
	play_commands->addAction(play_song_cursor_action);
	play_control_menu->addAction(play_song_cursor_action);

	QAction * play_pattern_cursor_action = new QAction(QPixmap((const char**)control_play_pattern_all_xpm),"Play Song From &Cursor",Qt::SHIFT+Qt::Key_F6,this,"Play Pattern From Cursor");
	QObject::connect( play_pattern_cursor_action, SIGNAL( activated() ) , this, SLOT( play_pattern_cursor_action_cbk() ) );
	play_control_menu->addAction(play_pattern_cursor_action);

	/*** Windows window */

	windowsMenu = new Q3PopupMenu( this );
	windowsMenu->setCheckable( TRUE );

	connect( windowsMenu, SIGNAL( aboutToShow() ), this, SLOT( about_to_show_window_menu() ) );
	menuBar() ->insertItem( "Windows", windowsMenu,WindowItemID );
	menuBar() ->insertSeparator();


        /* Settings Menu */

	settings_edit_menu = new Q3PopupMenu( this );
	menuBar()->insertItem( "Settings", settings_edit_menu,SettingsItemID );
	connect( settings_edit_menu, SIGNAL( aboutToShow() ), this, SLOT( about_to_show_tools_menu() ) );
	connect( settings_edit_menu, SIGNAL( activated(int) ), this, SLOT( tools_menu_item_activated(int) ) );


	/* Editing Octave Toolbar */
	editing_octave_toolbar = new Q3ToolBar(this);
	addToolBar( editing_octave_toolbar, tr( "Octave" ), Qt::DockTop, false );

	new QLabel("Octave:",editing_octave_toolbar);
	editing_octave = new CSpinButton(editing_octave_toolbar);
	editing_octave->setFocusPolicy(Qt::NoFocus);
	editing_octave->setMaxValue(8);
	editing_octave->setMinValue(0);
	QObject::connect(editing_octave,SIGNAL(valueChanged(int)),this,SLOT(default_octave_changed_cbk(int)));


	statusBar() ->message( "Ready", 2000 );

	Q3VBox* vb = new Q3VBox( this );
	vb->setFrameStyle( Q3Frame::StyledPanel | Q3Frame::Sunken );
	ws = new QWorkspace( vb );
	setCentralWidget( vb );

        timer = new QTimer(this);

	QObject::connect( timer, SIGNAL(timeout()), this, SLOT(timer_callback()) );
	timer->start(widget_timer_interval,false);
//	last_timer_interval=widget_timer_interval;

	last_window=NULL;
	last_window_page=Interface::Page__Audio_Config; //just wathever

	menuBar()->setItemEnabled(PatternEditItemID,false);
	menuBar()->setItemEnabled(SampleEditItemID,false);
	menuBar()->setItemEnabled(InstrumentEditItemID,false);


       	help_menu = new Q3PopupMenu(this);
	menuBar()->insertItem( "Help", help_menu,HelpItemID );
	help_menu->insertItem("Pattern Edit Effect List",Help__Pattern);
	help_menu->insertSeparator();
	help_menu->insertItem("About CheeseTracker!",Help__About);
	QObject::connect(help_menu,SIGNAL(activated(int)),this,SLOT(help_menu_item_activated(int)));




	showing_play_toolbar=true;
	showing_octave_toolbar=true;
	showing_file_toolbar=true;

	settings_window = new Settings_Window;
	//settings_window->show();

	QWidget *desktop = QApplication::desktop();

	if( desktop )
	{
		move( (desktop->width() - width()) / 2, (desktop->height() - height()) / 2 );
	}

	setIcon(QPixmap((const char**)cheese_48x48_xpm));

	callback_count=0;
}

void MDI_Main_Window::help_menu_item_activated(int p_id) {

	switch (p_id) {

		case Help__Pattern: {

			Text_Area_Popup *textarea_popup = new Text_Area_Popup;
			textarea_popup->set_text(help_pattern);
			textarea_popup->setFixedSize(500,400);
//			textarea_popup->setHeight(500,400);
			textarea_popup->exec();
		} break;
		case Help__About: {

			string about_msg=(string)APP_NAME_ + (string)APP_VERSION_ +(string)"\n";
			about_msg+=(string)APP_AUTHOR_+(string)"\n";
			about_msg+="Released on ";
			about_msg+=(string)APP_RELEASE_DATE_+(string)"\n";
			about_msg+=(string)"by " + (string)APP_MAINTAINER_ + "\n";
			about_msg+=(string)APP_WEBSITE_;
			QMessageBox::information( this, "About",about_msg.c_str(),"Ok");
		} break;
	}
}

void MDI_Main_Window::open_song(string p_song) {

	MDI_Sub_Window* w = newDoc();
	w->open_song( p_song );
	w->get_interface()->stop_song();

}

MDI_Sub_Window *  MDI_Main_Window::newDoc() {
        MDI_Sub_Window *  w = new MDI_Sub_Window( ws, 0, Qt::WDestructiveClose );

	w->setCaption( "New Song" );
	//	w->setIcon( QPixmap("document.xpm") );
	// show the very first window in maximized mode
	w->set_sound_driver_manager(sound_driver_manager);
	w->set_info_lock(info_lock);
	w->get_interface()->stop_song();
	ws->addWindow(w);

	if ( ws->windowList().isEmpty() )
		w->showMaximized();
	else
		w->show();
	return w;
}

void MDI_Main_Window::closeEvent( QCloseEvent *e ) {
	QWidgetList windows = ws->windowList();
	if ( windows.count() ) {
		for ( int i = 0; i < int( windows.count() ); ++i ) {
			QWidget *window = windows.at( i );
			if ( !window->close() ) {
				e->ignore();
				return ;
				}
			}
		}
	Q3MainWindow::closeEvent( e );
}

void MDI_Main_Window::load()
{

    QString fn = Q3FileDialog::getOpenFileName( QString::null, "Module Files (*.ct *.CT *.it *.IT *.xm *.XM *.s3m *.S3M)", this );
    if ( !fn.isEmpty() ) {
        MDI_Sub_Window* w = newDoc();
	w->open_song( fn.ascii() );
	w->get_interface()->stop_song();

    }  else {
        statusBar()->message( "Loading aborted", 2000 );
    }
}

void MDI_Main_Window::save()
{
    MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
    if ( m )
	m->get_interface()->save_song();
}


void MDI_Main_Window::save_as()
{
    MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
    if ( m )
	m->get_interface()->save_song_as();
}


void MDI_Main_Window::set_sound_driver_manager(Sound_Driver_Manager * p_sound_driver_manager) {

	sound_driver_manager=p_sound_driver_manager;
	settings_window->set_sound_driver_manager(p_sound_driver_manager);

}


void MDI_Main_Window::close_window()
{
    MDI_Sub_Window* m = (MDI_Sub_Window*)ws->activeWindow();
    if ( m )
        m->close();
}

void MDI_Main_Window::about()
{
    QMessageBox::about( this, "CheeseTracker",
                        "blahblah\n "
                        "and more blah");
}

MDI_Main_Window::~MDI_Main_Window()
{
	list<QAction*>::iterator ix;

	for(ix=hidden_actions.begin(); ix != hidden_actions.end(); ix++) {
		delete (*ix);
	}

}
