//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MIXER_EFFECTS_MANAGER_H
#define MIXER_EFFECTS_MANAGER_H

#include "components/audio/mixer.h"
#include <qhbox.h>
#include <qvbox.h>
#include <qlistbox.h>
#include <qpopupmenu.h>
#include "interface__QT/helpers/clist_manager.h"
#include "interface__QT/audio/effect_chain_editor.h"

/**
@author Juan Linietsky
*/
class Mixer_Effects_Manager : public QHBox {

	Q_OBJECT

	struct Vars {

		int selected_buffer;
		int selected_effect;
		int selected_output;

		Mixer *mixer;
	} vars;

	bool updating;
	QListBox *channel_list;
	QListBox *buffer_list;

	Clist_Manager *buffer_list_manager;

	Effect_Chain_Editor *chain_editor;

	QComboBox *output_buffer;
	QPushButton *buffer_out_add;
	QPushButton *buffer_out_remove;
	CSpinButton *initial_send_level;
	CSpinButton *send_level_control;

	QPopupMenu * buffer_select_popup;
	void update_selected_buffer();
	void update_send_parameters();

	QLineEdit *selected_name;


public slots:

	void buffer_name_changed(const QString& p_str);

	void buffer_selected_slot(int p_which);
	void channel_selected_slot(int p_which);
	void create_buffer_slot();
	void erase_selected_buffer_slot();

	void output_send_level_changed_slot(int p_to_what);
	void output_send_control_changed_slot(int p_to_what);
	void output_selected_slot(int p_idx);

	void at_popup_show_slot();
	void popup_selected_buffer_slot(int p_which);

	void output_add_slot();
	void output_remove_slot();

public:

	void set_mixer(Mixer *p_mixer);
	void update();
	Mixer_Effects_Manager(QWidget *p_parent);
	~Mixer_Effects_Manager();
};

#endif
