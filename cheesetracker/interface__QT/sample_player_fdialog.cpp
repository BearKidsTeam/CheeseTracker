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
#include <QEvent>
#include <QKeyEvent>

void
Sample_Player_FDialog::audition_slot(bool p_on)
{
	if(vars.sample && !p_on) {
		vars.rt_keyboard->sample_stop_all();
		delete vars.sample;
		vars.sample = NULL;
	}
	audition_mode = p_on;
}

void Sample_Player_FDialog::filename_hilite_slot(const QString& p_file) {

	if(!audition_mode) return;
	vars.rt_keyboard->sample_stop_all();
	printf("Loading sample %s\n",p_file.ascii());
	if (vars.sample)
		delete vars.sample;
	vars.sample=NULL;

	vars.sample = vars.file_manager->load_sample(p_file.ascii());
	if (!vars.sample)
		printf("Invalid sample couldnt load this\n");
}


bool Sample_Player_FDialog::eventFilter(QObject *p_obj,QEvent *p_event)
{
	if(p_event->type() == QEvent::Resize) {
		position_audition_checkbox();
	}

	if (vars.sample==NULL) {
		return Q3FileDialog::eventFilter(p_obj,p_event);
	}

	if (p_event->type()!=QEvent::KeyPress) {

		return Q3FileDialog::eventFilter(p_obj,p_event);
	}

	QKeyEvent*e=(QKeyEvent *)p_event;
	if (e->isAutoRepeat()) {

		return Q3FileDialog::eventFilter(p_obj,p_event);
	}


	int key=e->ascii();
	if ((key>='a') && (key<='z')) key-='a'-'A';

	if ( key=='/') {

		Editor aux_editor;
		aux_editor.default_octave_lower();

	} else if ( key=='*') {

		Editor aux_editor;
		aux_editor.default_octave_raise();

	} else {

		Editor aux_editor;
		Uint8 note=aux_editor.get_note_from_key(key);

		if(note != EMPTY_FIELD) {
			vars.rt_keyboard->sample_stop_key(note);
			vars.rt_keyboard->sample_set(vars.sample);
			vars.rt_keyboard->sample_press_key(note);

			return TRUE;
		}
	}

	return Q3FileDialog::eventFilter(p_obj,p_event);
}

void Sample_Player_FDialog::set_binds(File_Format_Manager *p_file_manager,Player_Realtime_Keyboard *p_rt_keyboard)
{

	vars.file_manager=p_file_manager;
	vars.rt_keyboard=p_rt_keyboard;

}


void
Sample_Player_FDialog::position_audition_checkbox()
{
	audition->move(5,this->height()-20);
}

Sample_Player_FDialog::Sample_Player_FDialog()
{

	vars.sample=NULL;
	QObject::connect(this,SIGNAL(fileHighlighted(const QString&)),this,SLOT(filename_hilite_slot(const QString&)));
	audition = new QCheckBox("Audition samples", this);
	audition->resize(audition->width()+40, audition->height());
	position_audition_checkbox();
	QObject::connect(audition,SIGNAL(toggled(bool)),this,SLOT(audition_slot(bool)));
	audition->setChecked(true);
	audition_mode=true;
}


Sample_Player_FDialog::~Sample_Player_FDialog()
{

	if (vars.sample) {
		vars.rt_keyboard->sample_stop_all();
		delete vars.sample;
	}
}


