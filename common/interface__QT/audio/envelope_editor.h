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

#ifndef ENVELOPE_EDITOR_H
#define ENVELOPE_EDITOR_H


#include <q3groupbox.h>
#include <q3hbox.h>
#include <qlabel.h>
#include <q3grid.h>
#include <q3frame.h>


#include "interface__QT/audio/envelope_point_editor.h"
#include "interface__QT/helpers/property_bridge_edit.h"


/**
 *
 * Juan Linietsky
 **/
class Envelope_Editor : public Q3GroupBox {

	Q_OBJECT


	Q3VBox *vbox;

	Envelope_Point_Editor *envelope_point_editor;
	Q3HBox  *envelope_point_editor_frame;

	Q3GroupBox * control_group;

        Q3HBox *control_hbox;
        QCheckBox *enabled;

        Q3Grid *table;


	Q3GroupBox * control_loop;

        QCheckBox *loop_enabled;

	QLabel *label_loop_begin;
	CSpinButton * loop_begin;
	QLabel *label_loop_end;
	CSpinButton * loop_end;

        QCheckBox *sustain_loop_enabled;


	QLabel *label_sustain_loop_begin;
	CSpinButton * sustain_loop_begin;
	QLabel *label_sustain_loop_end;
	CSpinButton * sustain_loop_end;

	bool updating;

	Envelope *envelope;

	void update_envelope_variables();

public Q_SLOTS:

	void envelope_changed();
	void loop_begin_changed(int p_val);
	void loop_end_changed(int p_val);
	void sustain_loop_begin_changed(int p_val);
	void sustain_loop_end_changed(int p_val);

	void envelope_enabled_check(bool p_on);
	void loop_enabled_check(bool p_on);
	void sustain_enabled_check(bool p_on);
public:

	Envelope_Point_Editor* get_point_editor() { return envelope_point_editor; }
	Q3HBox * get_control_box();
	void set_envelope(Envelope *p_envelope);

	Envelope_Editor(QWidget * p_parent);

    ~Envelope_Editor();
};

#endif
