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

#ifndef SAMPLE_EDIT_H
#define SAMPLE_EDIT_H

#include "trackercore/player_data.h"
#include "trackercore/file_format_manager.h"
#include "interface_binds/player_rt_keyboard.h"

#include <qwidget.h>
#include <q3listview.h>
#include <q3groupbox.h>
#include <q3hbox.h>
#include <vector>
	using std::vector;
#include "interface__QT/helpers/property_bridge_edit.h"
#include "trackercore/song.h"
#include "interface__QT/audio/sample_editor.h"

#include "plugins/edit_effects/simple_edit_effects.h"
#include "sample_player_fdialog.h"
/**
 *
 * Juan Linietsky
 **/



class Sample_Edit : public Q3HBox {

	Q_OBJECT //grah




	enum {
		MAX_SAMPLE_NAME_LEN=25
	};

	bool play_level[MAX_SAMPLES]; //play levels for display, to track status changes
	
	class ListviewItem : public  Q3ListViewItem {
        		int id;

	public:

			void set_id(int p_id) { id=p_id; }
			int get_id() { return id; }
			ListviewItem( Q3ListView * p_parent ) :  Q3ListViewItem(p_parent) {}

	};


	vector<ListviewItem*> sample_items;

	Q3GroupBox *sample_list_group;

	Q3VBox *sample_props_vbox;
	Q3HBox *sample_vars_hbox;

	Q3GroupBox *sample_data_group;

	Q3GroupBox *sample_volpan_group;

	Q_Property_Bridge_Int *bridge_default_volume;
	Q_Property_Bridge_Int *bridge_global_volume;
	Q_Property_Bridge_Bool *bridge_use_default_pan;
	Q_Property_Bridge_Int *bridge_default_pan;

	Q3GroupBox *sample_vibrato_group;


	Q_Property_Bridge_Int *bridge_vibrato_speed;
	Q_Property_Bridge_Int *bridge_vibrato_depth;
	Q_Property_Bridge_Int *bridge_vibrato_rate;

	Q_Property_Bridge_Options *bridge_vibrato_type;

	Sample_Editor * sample_editor;
	Song *song;
	int selected_sample;
	void select_sample(int p_which);

	vector<Player_Data::VoiceStatusInfo> *voice_status_info;
	Sample_Viewer::PositionList viewer_pos_list;
	static Sample sample_clipboard;

        int timer_count;
	Player_Realtime_Keyboard *rt_keyboard;
	Player_Data *player;

	File_Format_Manager *file_manager;

	Sample_Player_FDialog *fd;

	struct Edit_Effects {

		Edit_Effect_Reverse reverse;
		Edit_Effect_PreLoop_Cut pre_cut;
		Edit_Effect_PostLoop_Cut post_cut;
		Edit_Effect_SelToLoop sel_to_loop;
		Edit_Effect_Center center;
		Edit_Effect_Normalize normalize;
		Edit_Effect_Toggle_Sign toggle_sign;
		Edit_Effect_Toggle_Depth toggle_depth;
		Edit_Effect_Tunner tunner;

		Edit_Effect_FadeIn fade_in;
		Edit_Effect_FadeOut fade_out;

	} edit_effects;

protected slots: //protect your sluts

	void item_selected_cbk(Q3ListViewItem *p_item);
	void item_renamed_cbk( Q3ListViewItem * item, int col );

	void load_sample_from_keyboard(void);
	void destructive_operation_begin_cbk();
	void destructive_operation_end_cbk();

	void sample_freq_changed();
signals:

	void selected_sample_signal(int p_which);

public:
	void set_player(Player_Data *p_player) { player=p_player; }

	void set_rt_keyboard(Player_Realtime_Keyboard *p_rt_keyboard);

	void set_voice_status_info(vector<Player_Data::VoiceStatusInfo> *p_voice_status_info);

	void set_file_format_manager(File_Format_Manager *p_file_manager) { file_manager=p_file_manager; }

	//qt forces me to expose this, simply so i can install an eventfilter.. i'm sorry for doing it.
	Q3ListView * sample_list;

	void copy_sample();
	void paste_sample();
	void cut_sample();
	void delete_sample();
	void load_sample();
	void save_sample();


	int get_selected_sample_index() { return selected_sample; }


	void set_selected_sample(int p_which);
	void timer_callback();
	void set_song(Song *p_song);
	void update_samples();
	void update_selected_sample();


    Sample_Edit(QWidget *p_parent);
    ~Sample_Edit();
};

#endif
