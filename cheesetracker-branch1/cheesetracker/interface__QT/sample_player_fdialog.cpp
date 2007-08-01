//
// C++ Implementation: sample_player_fdialog
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sample_player_fdialog.h"


void Sample_Player_FDialog::filename_hilite_slot(const QString& p_file) {

	vars.rt_keyboard->sample_stop_all();
	printf("Loading sample %s\n",p_file.ascii());
	if (vars.sample)
		delete vars.sample;
	vars.sample=NULL;

	vars.sample = vars.file_manager->load_sample(p_file.ascii());
	if (!vars.sample)
		printf("Invalid sample couldnt load this\n");
}


bool Sample_Player_FDialog::eventFilter(QObject *p_obj,QEvent *p_event) {

	if (vars.sample==NULL) {

		return QFileDialog::eventFilter(p_obj,p_event);
	}

	if (p_event->type()!=QEvent::KeyPress) {

		return QFileDialog::eventFilter(p_obj,p_event);
	}

	QKeyEvent*e=(QKeyEvent *)p_event;
	if (e->isAutoRepeat()) {

		return QFileDialog::eventFilter(p_obj,p_event);
	}


	int key=e->ascii();
	if ((key>='a') && (key<='z')) key-='a'-'A';

	if ( key=='/') {

		Editor aux_editor;
		aux_editor.default_octave_lower();

	} else if ( key=='*') {

		Editor aux_editor;
		aux_editor.default_octave_raise();

	} else if ( ((key>='A') && (key<='Z')) || ((key>='0') && (key<='9')) ) {

		Editor aux_editor;
		aux_editor.get_note_from_key(key);
		Uint8 note=aux_editor.get_note_from_key(key);

		vars.rt_keyboard->sample_stop_key(note);
		vars.rt_keyboard->sample_set(vars.sample);
		vars.rt_keyboard->sample_press_key(note);

		return TRUE;
	}

	return QFileDialog::eventFilter(p_obj,p_event);
}

void Sample_Player_FDialog::set_binds(File_Format_Manager *p_file_manager,Player_Realtime_Keyboard *p_rt_keyboard) {

	vars.file_manager=p_file_manager;
	vars.rt_keyboard=p_rt_keyboard;

}

Sample_Player_FDialog::Sample_Player_FDialog() {

	vars.sample=NULL;
	QObject::connect(this,SIGNAL(fileHighlighted(const QString&)),this,SLOT(filename_hilite_slot(const QString&)));
}


Sample_Player_FDialog::~Sample_Player_FDialog() {

	if (vars.sample)
		delete vars.sample;
}


