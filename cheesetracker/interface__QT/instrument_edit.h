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

#ifndef INSTRUMENT_EDIT_H
#define INSTRUMENT_EDIT_H

#include "trackercore/player_data.h"
#include "trackercore/file_format_manager.h"
#include <qwidget.h>
#include <q3listview.h>
#include <q3groupbox.h>
#include <q3hbox.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <q3grid.h>
#include <vector>
#include <qradiobutton.h>
#include <q3buttongroup.h>


	using std::vector;
#include "interface__QT/helpers/property_bridge_edit.h"
#include "interface__QT/audio/envelope_editor.h"
#include "interface__QT/audio/note_bridge.h"
#include "trackercore/song.h"
#include "interface__QT/sample_instrument_table.h"
#include "interface_binds/editor.h"

/**
 *
 * Juan Linietsky
 **/
class Instrument_Edit : public Q3HBox {

	Q_OBJECT //grah

	enum {
		MAX_INSTRUMENT_NAME_LEN=25
	};

	class ListviewItem : public  Q3ListViewItem {
        		int id;

	public:

			void set_id(int p_id) { id=p_id; }
			int get_id() { return id; }
			ListviewItem( Q3ListView * p_parent ) :  Q3ListViewItem(p_parent) {}

	};

	int selected_instrument;
	int selected_layer;

	vector<ListviewItem*> instrument_items;

	Q3ButtonGroup *layer_group;
	vector<QRadioButton*> layer_options;

	Q_Property_Bridge_Int_CSpinButon *property_output_buffer;

	Q3GroupBox *instrument_list_group;

	QTabWidget *instrument_props_notebook;

	Q3HBox * instrument_general_hbox;
	Q3VBox * instrument_volume_vbox;
	Q3VBox * instrument_pan_vbox;
	Q3VBox * instrument_pitchfilter_vbox;

	Q3GroupBox * note_sample_table_group;
	Sample_Instrument_Table *note_sample_edit;

	struct Volume {

		Envelope_Editor * envelope_editor;
		Q3GroupBox *group;
		Q3Grid *grid;

		Q_Property_Bridge_Int_CSpinButon * property_global_amount;
		Q_Property_Bridge_Int_CSpinButon * property_fadeout;
		Q_Property_Bridge_Int_CSpinButon * property_random;

	} volume;

	struct Panning {

		Envelope_Editor * envelope_editor;

		Q3GroupBox *group;
		Q3Grid *grid;

		Q_Property_Bridge_Bool *property_use_default;
		Q_Property_Bridge_Int *property_default_amount;

		Q_Property_Bridge_Int_CSpinButon *property_pitch_separation;
		Q_Note_Bridge *property_pitch_center;
		Q_Property_Bridge_Int_CSpinButon *property_random_variation;

	} panning;

	struct PitchFilter {

		Envelope_Editor * envelope_editor;

		Q3GroupBox *group;
		Q3Grid *grid;

		Q_Property_Bridge_Bool *property_envelope_use_as_filter;
		Q_Property_Bridge_Bool *property_use_default_cutoff;
		Q_Property_Bridge_Bool *property_use_default_Q;
		Q_Property_Bridge_Int_CSpinButon *property_default_cutoff;
		Q_Property_Bridge_Int_CSpinButon *property_default_Q;
		Q_Property_Bridge_Options *property_filter_type;
	} pitch_filter;

	struct NNAData {

		Q3VBox *vbox;
		Q3HBox *hbox;

		Q3ButtonGroup * nna_group;
		Q3ButtonGroup * dct_group;
		Q3ButtonGroup * dct_group_action;

		QRadioButton * nna_cut;
		QRadioButton * nna_off;
		QRadioButton * nna_continue;
		QRadioButton * nna_fade;

		QRadioButton * dct_disabled;
		QRadioButton * dct_note;
		QRadioButton * dct_sample;
		QRadioButton * dct_instrument;

		QRadioButton * dct_off;
		QRadioButton * dct_cut;
		QRadioButton * dct_fade;

	} nna;

	static Instrument instrument_clipboard;

	Song *song;
	Editor *editor;

	vector<Player_Data::VoiceStatusInfo> *voice_status_info;
	vector<int> vol_envelope_positions;
	vector<int> pan_envelope_positions;
	vector<int> pitch_envelope_positions;

	int time_counter;
	Player_Data *player;
	File_Format_Manager *file_manager;
protected Q_SLOTS:

	void item_selected_cbk(Q3ListViewItem *p_item);
	void item_renamed_cbk( Q3ListViewItem * item, int col );
	void load_instrument_from_keyboard(void);

	void update_selected_instrument();

	void nna_selected(int p_which);
	void dct_selected(int p_which);
	void dct_action_selected(int p_which);
	void layer_selected(int p_which);

Q_SIGNALS:

	void update_sample_editor();

public:
	void set_file_format_manager(File_Format_Manager *p_file_manager) { file_manager=p_file_manager; }
	void set_variables_lock(Mutex_Lock *p_lock);
	void set_player(Player_Data *p_player);

	void set_voice_status_info(vector<Player_Data::VoiceStatusInfo> *p_voice_status_info);
	//sorry for this, but Qt forces me to expose it, since it eats keypresses :(
	Q3ListView * instrument_list;

	void copy_instrument();
	void cut_instrument();
	void paste_instrument();
	void delete_instrument();
	void delete_instrument_with_samples();

	void load_instrument();
	void save_instrument();

	void set_selected_instrument(int p_which);
	void timer_callback();
	void set_song(Song *p_song);
	void set_editor(Editor *p_editor);
	void update();

	int get_selected_instrument(){ return selected_instrument; }

	Instrument_Edit(QWidget *p_parent);
	~Instrument_Edit();
};

#endif
