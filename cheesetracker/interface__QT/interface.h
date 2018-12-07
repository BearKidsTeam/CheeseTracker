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

#ifndef INTERFACE_H
#define INTERFACE_H


#include "interface_binds/tracker_instance.h"
#include "interface__QT/sample_edit.h"
#include "interface__QT/instrument_edit.h"
#include "interface__QT/order_and_defaults_editor.h"
#include "interface__QT/variables_edit.h"
#include "interface__QT/pattern_edit_widget.h"
#include "interface__QT/audio/mixer_effects_manager.h"


#include <qtabwidget.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QEvent>
#include <QKeyEvent>
/**
 *
 * Juan Linietsky
 **/
class Interface : public QTabWidget {
public:
	enum PageList {

        	Page__Pattern_Editor,
        	Page__Sample_Editor,
        	Page__Instrument_Editor,
        	Page__Order_And_Defaults,
        	Page__Variables,
        	Page__Audio_Config
	};

private:


	Q_OBJECT

        static int widget_timer_interval;
	int last_timer_interval;

	Pattern_Edit_Widget *pattern_editor;
	Sample_Edit *sample_editor;
	Instrument_Edit *instrument_editor;
	Order_And_Defaults_Editor *order_and_defaults_editor;
	Variables_Edit *variables_editor;
	Mixer_Effects_Manager *buffers_editor;

	Tracker_Instance tracker;

	void update_song_widgets();

	virtual void keyPressEvent ( QKeyEvent * e );
	virtual void keyReleaseEvent ( QKeyEvent * e );
	bool eventFilter( QObject *o, QEvent *e );

	//not the best approach to updating widgets, but by far the simpler to program!
	//I will simply be checking if the value is correct every N miliseconds and correct is necesary
	//It does not use any CPU
	QTimer *timer;
	QTimer *timer_midi;

	vector<Player_Data::VoiceStatusInfo> voice_status_info;


protected slots:
	void midi_check_timer();
	void widget_update_timer();
	void selected_sample_in_editor(int p_which);
	void selected_instrument_in_pattern(int p_which);
	void update_sample_editor_request();
	void update_instrument_mode_callback();


public:

	static Int_Property_Bridge timer_interval_bridge;
	void play_song();
	void stop_song();
	void play_pattern();
        void play_pattern_from_cursor();
	void play_from_mark();
	void play_from_cursor();
	void play_from_order();

	void play_next_order();
	void play_previous_order();


	Sample_Edit *get_sample_editor() { return sample_editor; };
	Instrument_Edit *get_instrument_editor() { return instrument_editor; };
	Pattern_Edit_Widget *get_pattern_editor() { return pattern_editor; }

	void update_pattern_editor();
	Tracker_Instance * get_tracker();

	string get_song_name();
	PageList get_current_page();

	Loader::Error  open_song(string p_name);
	Saver::Error  save_song();
	Saver::Error  save_song_as();


    Interface(QWidget *p_widget);
    ~Interface();
};

#endif
