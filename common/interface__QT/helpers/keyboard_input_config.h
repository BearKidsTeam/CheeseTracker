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
#ifndef KEYBOARD_INPUT_CONFIG_H
#define KEYBOARD_INPUT_CONFIG_H


#include <q3groupbox.h>
#include <q3scrollview.h>
#include <qlabel.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <qpushbutton.h>

#include <vector>
	using std::vector;

/**
@author Juan Linietsky
*/

class Key_Entry : public Q3HBox {

	Q_OBJECT

	QLabel *key_caption;
	QLabel *key_ascii;
	QPushButton *change;
	int key_idx;

protected slots:

	void keybind_change_request();
public:
	void update();
	Key_Entry(QWidget *p_parent,int p_key_idx);
};

class Keyboard_Input_Config : public Q3GroupBox {

        vector<Key_Entry*> key_entry_list;
        Q3ScrollView *scroll;
	Q3VBox *vbox;
public:

	void update();
	Keyboard_Input_Config(QWidget *p_parent);
	~Keyboard_Input_Config();

};

#endif
