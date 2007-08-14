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
#include "envelope_editor.h"
//Added by qt3to4:
#include <QLabel>

void Envelope_Editor::envelope_changed() {

	update_envelope_variables();
}


void Envelope_Editor::loop_begin_changed(int p_val) {

	if (updating) return;

	envelope->set_loop_begin(p_val);
	update_envelope_variables();

}

void Envelope_Editor::loop_end_changed(int p_val) {

	if (updating) return;
	envelope->set_loop_end(p_val);

	update_envelope_variables();
}

void Envelope_Editor::sustain_loop_begin_changed(int p_val) {

	if (updating) return;
	envelope->set_sustain_loop_begin(p_val);

	update_envelope_variables();
}

void Envelope_Editor::sustain_loop_end_changed(int p_val) {
	if (updating) return;
	envelope->set_sustain_loop_end(p_val);

	update_envelope_variables();
}

void Envelope_Editor::envelope_enabled_check(bool p_on) {
	if (updating) return;

	envelope->set_enabled(p_on);
	envelope_point_editor->update();
}

void Envelope_Editor::loop_enabled_check(bool p_on) {

	if (updating) return;

	envelope->set_loop_enabled(p_on);
	envelope_point_editor->update();

}
void Envelope_Editor::sustain_enabled_check(bool p_on) {
	if (updating) return;

	envelope->set_sustain_loop_enabled(p_on);
	envelope_point_editor->update();

}


void Envelope_Editor::update_envelope_variables() {

	if (!envelope) return;

	updating=true;
	enabled->setChecked(envelope->is_enabled());

	loop_begin->setMinValue(0);
	loop_begin->setMaxValue(envelope->get_loop_end());
	loop_begin->setValue(envelope->get_loop_begin());
	loop_end->setMinValue(envelope->get_loop_begin());
	loop_end->setMaxValue(envelope->get_node_count()-1);
	loop_end->setValue(envelope->get_loop_end());

	loop_enabled->setChecked(envelope->is_loop_enabled());

	sustain_loop_begin->setMinValue(0);
	sustain_loop_begin->setMaxValue(envelope->get_sustain_loop_end());
	sustain_loop_begin->setValue(envelope->get_sustain_loop_begin());
	sustain_loop_end->setMinValue(envelope->get_sustain_loop_begin());
	sustain_loop_end->setMaxValue(envelope->get_node_count()-1);
	sustain_loop_end->setValue(envelope->get_sustain_loop_end());

	sustain_loop_enabled->setChecked(envelope->is_sustain_loop_enabled());

	envelope_point_editor->update();
	updating=false;

}


void Envelope_Editor::set_envelope(Envelope *p_envelope) {


	envelope=p_envelope;
	envelope_point_editor->set_envelope(envelope);
	update_envelope_variables();
}

Q3HBox * Envelope_Editor::get_control_box() {

	return control_hbox;
}



Envelope_Editor::Envelope_Editor(QWidget * p_parent) : Q3GroupBox ( 1, Qt::Vertical,"Envelope:",p_parent) {


	vbox = new Q3VBox(this);
	vbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	envelope_point_editor_frame = new Q3HBox (vbox);
	envelope_point_editor_frame->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	envelope_point_editor_frame->setFrameStyle(WinPanel|Sunken);
	envelope_point_editor_frame->setMargin(0);

	envelope_point_editor = new Envelope_Point_Editor(envelope_point_editor_frame);
	envelope_point_editor->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));


	control_group = new Q3GroupBox(1,Qt::Vertical,"Control:",vbox);
	control_group->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	control_hbox = new Q3HBox(control_group);
	enabled = new QCheckBox("Enable Envelope",control_hbox);

	control_loop = new Q3GroupBox(1,Qt::Vertical,"Loop:",vbox);
	control_loop->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	table = new Q3Grid(4,control_loop);
	table->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	loop_enabled = new QCheckBox("Enable Loop",table);
        new QLabel(table); //empty
	sustain_loop_enabled = new QCheckBox("Sustain Loop:",table);
        new QLabel(table); //empty

	label_loop_begin=new QLabel("    Begin ",table);
	loop_begin=new CSpinButton(table);
	label_sustain_loop_begin=new QLabel("    Begin ",table);
	sustain_loop_begin=new CSpinButton(table);

	label_loop_end=new QLabel("    End ",table);
	loop_end=new CSpinButton(table);
	label_sustain_loop_end=new QLabel("    End ",table);
	sustain_loop_end=new CSpinButton(table);

	envelope=NULL;

	QObject::connect(envelope_point_editor, SIGNAL(envelopeChanged (void)),this, SLOT(envelope_changed (void)) );

	QObject::connect(loop_begin, SIGNAL(valueChanged (int)),this, SLOT(loop_begin_changed (int)) );
	QObject::connect(loop_end, SIGNAL(valueChanged (int)),this, SLOT(loop_end_changed (int)) );

	QObject::connect(sustain_loop_begin, SIGNAL(valueChanged (int)),this, SLOT(sustain_loop_begin_changed (int)) );
	QObject::connect(sustain_loop_end, SIGNAL(valueChanged (int)),this, SLOT(sustain_loop_end_changed (int)) );

	QObject::connect(sustain_loop_enabled, SIGNAL(toggled (bool)),this, SLOT(sustain_enabled_check (bool)) );
	QObject::connect(loop_enabled, SIGNAL(toggled (bool)),this, SLOT(loop_enabled_check (bool)) );
	QObject::connect(enabled, SIGNAL(toggled (bool)),this, SLOT(envelope_enabled_check (bool)) );


}

Envelope_Editor::~Envelope_Editor()
{
}
