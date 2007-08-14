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

#include <QColorGroup>
#include "instrument_edit.h"

#include <q3filedialog.h>
#include <qmessagebox.h>
Instrument Instrument_Edit::instrument_clipboard;

void Instrument_Edit::load_instrument() {

	QString fn = Q3FileDialog::getOpenFileName( QString::null, "Instrument Files (*.ci *.CI *.ii *.II *.xi *.XI)", this );
	if (fn.isEmpty())
		return; //nothing


	file_manager->load_instrument( fn.ascii() , selected_instrument );

	update();
	update_selected_instrument();

	update_sample_editor();
}

void Instrument_Edit::save_instrument() {

	QString s = Q3FileDialog::getSaveFileName(
			QString::null,
			"Instrument Formats (*.ci *.CI *.ii *.II)",
			this,
			"Save"
			"Choose a File Name" );

	if (s==QString::null)
		return; //nothing selected

	FILE *f=fopen(s.ascii(),"rb");
	if (f) { //file exists
		fclose(f);
		if ( QMessageBox::warning( this, "Question:","File exists! Overwrite?","Yes", "No") ) {
			return ;
		}
	}

        if ( file_manager->save_instrument( s.ascii() , selected_instrument ) ) {

		QMessageBox::warning( this, "Oops!","Error saving instrument!","Ok");
		return ;
	}

	return ;

}


void Instrument_Edit::copy_instrument() {

	Instrument * inst=song->get_instrument(selected_instrument);
	if (!inst)
		return;

	instrument_clipboard=*inst;

}

void Instrument_Edit::cut_instrument() {

	copy_instrument();
	delete_instrument();

}

void Instrument_Edit::paste_instrument() {

	Instrument * inst=song->get_instrument(selected_instrument);
	if (!inst)
		return;


	*inst=instrument_clipboard;
	update();
	update_selected_instrument();

}

void Instrument_Edit::delete_instrument() {

	Instrument * inst=song->get_instrument(selected_instrument);
	if (!inst)
		return;

	inst->reset();
	update();
	update_selected_instrument();

}

void Instrument_Edit::delete_instrument_with_samples() {

	Instrument * inst=song->get_instrument(selected_instrument);
	if (!inst)
		return;

	for (int j=0;j<Instrument::MAX_LAYERS;j++) {

		for (int i=0;i<Note::NOTES;i++) {

			Sample * samp=song->get_sample(inst->get_layer(j)->sample_number[i]);
			if (samp==NULL)
				continue;
			if (samp->in_use) {

				player->lock_player_variables(__FILE__, __LINE__);
				player->erase_voices_using_sample(samp);

				samp->reset(true);

				player->unlock_player_variables();

			}
		}
	}

	inst->reset();

	update();
	update_selected_instrument();

}


// FIXME: Qt4: All this must now be handled in a paint event. GOD DAMMIT!!!!!

void Instrument_Edit::timer_callback() {

	time_counter++;

	if (!(time_counter%2)) {

		vol_envelope_positions.clear();
		pan_envelope_positions.clear();
		pitch_envelope_positions.clear();

		for (int i=0;i<(int)voice_status_info->size();i++) {

			if ((*voice_status_info)[i].instrument_layer.ptr==song->get_instrument(selected_instrument)->get_layer(selected_layer)) {

				vol_envelope_positions.push_back( (*voice_status_info)[i].instrument_layer.vol_env_index );
				pan_envelope_positions.push_back( (*voice_status_info)[i].instrument_layer.pan_env_index );
				pitch_envelope_positions.push_back( (*voice_status_info)[i].instrument_layer.frq_env_index );
			}
		}

                volume.envelope_editor->get_point_editor()->update_position_list(vol_envelope_positions);
                panning.envelope_editor->get_point_editor()->update_position_list(pan_envelope_positions);
                pitch_filter.envelope_editor->get_point_editor()->update_position_list(pitch_envelope_positions);
	}
}

void Instrument_Edit::set_player(Player_Data *p_player) {

	player=p_player;
}

void Instrument_Edit::set_variables_lock(Mutex_Lock *p_lock) {

	volume.envelope_editor->get_point_editor()->set_envelope_lock(p_lock);
	panning.envelope_editor->get_point_editor()->set_envelope_lock(p_lock);
	pitch_filter.envelope_editor->get_point_editor()->set_envelope_lock(p_lock);
}
void Instrument_Edit::item_selected_cbk(Q3ListViewItem *p_item) {

	ListviewItem * item = dynamic_cast<ListviewItem *>(p_item);
	if (item==NULL) {
		//printf("Item is null? is RTTI enabled?\n");
		return;
	} else {

		selected_instrument=item->get_id();
		if (editor) editor->set_instrument_mask_value(selected_instrument);
		update_selected_instrument();
	}
}

void Instrument_Edit::set_selected_instrument(int p_which) {

	if ((p_which<0) || (p_which>=(int)instrument_items.size()))
		return;


	instrument_list->setSelected(instrument_items[p_which],true);
}

void Instrument_Edit::item_renamed_cbk( Q3ListViewItem * p_item, int col ) {

	ListviewItem * item = dynamic_cast<ListviewItem *>(p_item);
	if (item==NULL) {
		//printf("Item is null? is RTTI enabled?\n");
		return;
	} else {

		string text=item->text(col).ascii();
                if (text.length()>25)
			text=text.substr(0,25);

		item->setText(col,text.c_str());

		song->get_instrument(item->get_id())->name=text;
/*		select_sample(item->get_id());
		selected_sample=item->get_id();*/
	}

}

void Instrument_Edit::load_instrument_from_keyboard(void) {
	load_instrument();
}

void Instrument_Edit::update_selected_instrument() {



	Instrument::Layer *inst=song->get_instrument(selected_instrument)->get_layer(selected_layer);
	volume.envelope_editor->set_envelope(&inst->volume.envelope);
	panning.envelope_editor->set_envelope(&inst->panning.envelope);
	pitch_filter.envelope_editor->set_envelope(&inst->pitch.envelope);

	volume.property_global_amount->set_bridge(&inst->prop_volume_global_amount);
	volume.property_fadeout->set_bridge(&inst->prop_volume_fadeout);
	volume.property_random->set_bridge(&inst->prop_volume_random);

	property_output_buffer->set_bridge(&song->get_instrument(selected_instrument)->prop_buffer);


	panning.property_use_default->set_bridge(&inst->prop_panning_use_default);
	panning.property_default_amount->set_bridge(&inst->prop_panning_default_amount);
	panning.property_pitch_separation->set_bridge(&inst->prop_panning_pitch_separation);
	panning.property_pitch_center->set_bridge(&inst->prop_panning_pitch_center);
	panning.property_random_variation->set_bridge(&inst->prop_panning_random_variation);

	pitch_filter.property_envelope_use_as_filter->set_bridge(&inst->pitch_envelope_use_as_filter);
	pitch_filter.property_use_default_cutoff->set_bridge(&inst->pitch_use_default_cutoff);
	pitch_filter.property_use_default_Q->set_bridge(&inst->pitch_use_default_Q);
	pitch_filter.property_default_cutoff->set_bridge(&inst->pitch_default_cutoff);
	pitch_filter.property_default_Q->set_bridge(&inst->pitch_default_Q);
	pitch_filter.property_filter_type->set_bridge(&inst->pitch_filter_type);

	nna.nna_group->setButton(inst->NNA_type);
	nna.dct_group->setButton(inst->duplicate_check_type);
	nna.dct_group_action->setButton(inst->duplicate_check_action);


	note_sample_edit->link_to_instrument_layer(inst);
	note_sample_edit->link_to_editor(editor);




}
void Instrument_Edit::update() {


	for (int i=1;i<99;i++) {

		ListviewItem * instrument_item = instrument_items[i-1];
		instrument_item->setText(1,song->get_instrument(i-1)->name.c_str());
	}
	update_selected_instrument();
}

void Instrument_Edit::set_song(Song *p_song) {

	song=p_song;
	update();
}

void Instrument_Edit::set_editor(Editor *p_editor) {

	editor=p_editor;
}

void Instrument_Edit::layer_selected(int p_which) {

	selected_layer=p_which;
	update_selected_instrument();

}


void Instrument_Edit::nna_selected(int p_which) {

	song->get_instrument(selected_instrument)->get_layer(selected_layer)->NNA_type=p_which;
}

void Instrument_Edit::dct_selected(int p_which) {

	song->get_instrument(selected_instrument)->get_layer(selected_layer)->duplicate_check_type=p_which;


}
void Instrument_Edit::dct_action_selected(int p_which) {

	song->get_instrument(selected_instrument)->get_layer(selected_layer)->duplicate_check_action=p_which;


}

void Instrument_Edit::set_voice_status_info(vector<Player_Data::VoiceStatusInfo> *p_voice_status_info) {

	voice_status_info=p_voice_status_info;
}


Instrument_Edit::Instrument_Edit(QWidget *p_parent) : Q3HBox(p_parent)  {

	song=NULL;
	editor=NULL;

	Q3VBox * instr_props_aux_vbox = new Q3VBox(this);

	instrument_list_group = new Q3GroupBox ( 1, Qt::Vertical,"Instrument List",instr_props_aux_vbox);

	selected_instrument=0;
	/* listview */
	instrument_list = new Q3ListView(instrument_list_group);
	instrument_list->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	instrument_list->setDefaultRenameAction(Q3ListView::Accept);

	instrument_list->addColumn("#",20);
	instrument_list->addColumn("Name");

	instrument_list->setAllColumnsShowFocus(true);

	QObject::connect(instrument_list, SIGNAL(returnPressed ( Q3ListViewItem *)),
			this, SLOT(load_instrument_from_keyboard(void)));

	QObject::connect(instrument_list, SIGNAL(selectionChanged ( Q3ListViewItem * )),this, SLOT(item_selected_cbk ( Q3ListViewItem * )) );
	QObject::connect(instrument_list, SIGNAL(itemRenamed ( Q3ListViewItem *,int )),this, SLOT(item_renamed_cbk ( Q3ListViewItem *,int )) );

	for (int i=1;i<99;i++) {

		ListviewItem * instrument_item = new ListviewItem(instrument_list);
		char text[3];
		text[2]=0;
		text[0]='0'+i/10;
		text[1]='0'+i%10;
		instrument_item->setText(0,text);
		instrument_items.push_back(instrument_item);
		instrument_item->set_id(i-1);
		instrument_item->setRenameEnabled(1,true);
	}

	Q3HBox *aux_layer_hbox = new Q3HBox(instr_props_aux_vbox);
	aux_layer_hbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	Q3GroupBox *auxgroup = new Q3GroupBox ( 1, Qt::Vertical,"Output:",aux_layer_hbox);
	property_output_buffer = new Q_Property_Bridge_Int_CSpinButon(auxgroup);

	layer_group = new Q3ButtonGroup ( 1, Qt::Vertical,"Current Layer:",aux_layer_hbox);
	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		char text[2];
		text[0]='1'+i;
		text[1]=0;

		QRadioButton * rb = new QRadioButton(text,layer_group);
		layer_options.push_back(rb);
	}
	layer_group->setButton(0);
	QObject::connect(layer_group, SIGNAL(pressed ( int )),this, SLOT(layer_selected( int )) );


	instrument_props_notebook = new QTabWidget (this);
	instrument_props_notebook->setMargin(5);

	instrument_general_hbox = new Q3HBox(instrument_props_notebook);
	instrument_props_notebook->addTab(instrument_general_hbox ,"General");
	instrument_general_hbox ->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	instrument_volume_vbox = new Q3VBox(instrument_props_notebook);
	instrument_props_notebook->addTab(instrument_volume_vbox ,"Volume");
	instrument_volume_vbox ->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	instrument_pan_vbox = 	new Q3VBox(instrument_props_notebook);
	instrument_props_notebook->addTab(instrument_pan_vbox  ,"Pan");
	instrument_pan_vbox  ->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	instrument_pitchfilter_vbox = new Q3VBox(instrument_props_notebook);
	instrument_props_notebook-> addTab(instrument_pitchfilter_vbox ,"Pitch/Filter");
	instrument_pitchfilter_vbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));


	/* General properties */

	note_sample_table_group = new Q3GroupBox( 1, Qt::Vertical,"Note/Sample:",instrument_general_hbox);
	note_sample_table_group->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding));
	note_sample_edit = new Sample_Instrument_Table(note_sample_table_group);
	note_sample_edit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));


	nna.vbox = new Q3VBox(instrument_general_hbox);
	nna.vbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	nna.nna_group = new Q3ButtonGroup ( 1, Qt::Horizontal,"New Note Action:", nna.vbox);
	nna.nna_group->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	QObject::connect(nna.nna_group, SIGNAL(pressed ( int )),this, SLOT(nna_selected( int )) );

	nna.hbox = new Q3HBox(nna.vbox);
	nna.hbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	nna.dct_group = new Q3ButtonGroup ( 1, Qt::Horizontal,"Dup. Check Type", nna.hbox);
	nna.dct_group->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	QObject::connect(nna.dct_group, SIGNAL(pressed ( int )),this, SLOT(dct_selected( int )) );

	nna.dct_group_action = new Q3ButtonGroup ( 1, Qt::Horizontal,"Dup. Check Action:", nna.hbox);
	nna.dct_group_action->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	QObject::connect(nna.dct_group_action, SIGNAL(pressed ( int )),this, SLOT(dct_action_selected( int )) );


	nna.nna_cut = new QRadioButton("Note Cut",nna.nna_group);
	nna.nna_continue = new QRadioButton("Note Continue",nna.nna_group);
	nna.nna_off = new QRadioButton("Note Off",nna.nna_group);
	nna.nna_fade = new QRadioButton("Note Fade",nna.nna_group);


	nna.dct_disabled = new QRadioButton("Disabled",nna.dct_group);
	nna.dct_note = new QRadioButton("Note",nna.dct_group);
	nna.dct_sample = new QRadioButton("Sample",nna.dct_group);
	nna.dct_instrument = new QRadioButton("Instrument",nna.dct_group);

	nna.dct_off = new QRadioButton("Note Cut",nna.dct_group_action);
	nna.dct_cut = new QRadioButton("Note Off",nna.dct_group_action);
	nna.dct_fade = new QRadioButton("Note Fade",nna.dct_group_action);

	/* Volume Properties */
	volume.envelope_editor = new Envelope_Editor(instrument_volume_vbox);
	volume.group = new Q3GroupBox ( 1, Qt::Vertical,"Volume Parameters",instrument_volume_vbox);
	volume.grid = new Q3Grid(2,volume.group);


	volume.property_global_amount = new Q_Property_Bridge_Int_CSpinButon(volume.grid);
	volume.property_fadeout = new Q_Property_Bridge_Int_CSpinButon(volume.grid);
	volume.property_random = new Q_Property_Bridge_Int_CSpinButon(volume.grid);


	/* Panning Properties */
	panning.envelope_editor = new Envelope_Editor(instrument_pan_vbox);

	panning.group = new Q3GroupBox ( 1, Qt::Vertical,"Panning Parameters",instrument_pan_vbox);
	panning.grid = new Q3Grid(2,panning.group);


	Q3HBox *auxhv = new Q3HBox(panning.grid);
	panning.property_use_default = new Q_Property_Bridge_Bool(auxhv);
	panning.property_use_default->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	panning.property_default_amount = new Q_Property_Bridge_Int(auxhv);
	panning.property_default_amount->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	panning.property_pitch_separation = new Q_Property_Bridge_Int_CSpinButon(panning.grid);
	panning.property_pitch_center = new Q_Note_Bridge(panning.grid);
	panning.property_random_variation = new Q_Property_Bridge_Int_CSpinButon(panning.grid);


	/* Pitch / Filter properties */

	pitch_filter.envelope_editor = new Envelope_Editor(instrument_pitchfilter_vbox);
	pitch_filter.group = new Q3GroupBox ( 1, Qt::Vertical,"Filter Parameters",instrument_pitchfilter_vbox);
	pitch_filter.grid = new Q3Grid(2,pitch_filter.group);

	pitch_filter.property_envelope_use_as_filter = new Q_Property_Bridge_Bool(pitch_filter.envelope_editor->get_control_box());
	pitch_filter.property_filter_type = new Q_Property_Bridge_Options(pitch_filter.envelope_editor->get_control_box());

	pitch_filter.property_use_default_cutoff = new Q_Property_Bridge_Bool(pitch_filter.grid);
	pitch_filter.property_use_default_Q = new Q_Property_Bridge_Bool(pitch_filter.grid);

	pitch_filter.property_default_cutoff= new Q_Property_Bridge_Int_CSpinButon(pitch_filter.grid);
	pitch_filter.property_default_Q= new Q_Property_Bridge_Int_CSpinButon(pitch_filter.grid);

        /* properties */

	setStretchFactor(instr_props_aux_vbox,1);
	setStretchFactor(instrument_props_notebook,2);
	time_counter=0;
	vol_envelope_positions.reserve(500);
	pan_envelope_positions.reserve(500);
	pitch_envelope_positions.reserve(500);
	selected_layer=0;

}

Instrument_Edit::~Instrument_Edit()
{
}
