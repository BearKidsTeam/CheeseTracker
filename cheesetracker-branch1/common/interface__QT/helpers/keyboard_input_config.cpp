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
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "keyboard_input_config.h"

#include "components/data/keyboard_input.h"
#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>
#include <QKeyEvent>


class GetKeyDialog : public QDialog {

	char key;

        void keyPressEvent ( QKeyEvent * e ) {

		if ( (e->ascii()>32) && (e->ascii()<128))  {

			key=e->ascii();
			accept();

		}
		if (e->ascii()==27)
			accept();
	}
public:

	char get_key() {

		return key;
	}

	GetKeyDialog(QWidget *p_parent) : QDialog(p_parent) {
		key=0;
		new QLabel("Press a Key",this);
		setCaption("Change Key");
	};
};

void Key_Entry::keybind_change_request() {
	Keyboard_Input *kinput = Keyboard_Input::get_singleton_instance();

	GetKeyDialog *key_dialog = new GetKeyDialog(this);

//	//printf("changeme!\n");

	key_dialog->exec();

	kinput->set_key_bind_key(key_idx,key_dialog->get_key());
	update();

        delete key_dialog;
}

void Key_Entry::update() {

	Keyboard_Input *kinput = Keyboard_Input::get_singleton_instance();

	char a[2] = { 0,0 };
	a[0]=kinput->get_key_bind_key_nocase(key_idx);
	key_ascii->setText( a );
}


Key_Entry::Key_Entry(QWidget *p_parent,int p_key_idx) : Q3HBox(p_parent) {

	setFrameStyle(Box+Raised);
	Keyboard_Input *kinput = Keyboard_Input::get_singleton_instance();

	key_caption= new QLabel( kinput->get_key_bind(p_key_idx).caption_name.c_str() ,this );
	setStretchFactor(key_caption,4);
	key_ascii= new QLabel( this );
	setStretchFactor(key_ascii,1);
	change = new QPushButton("Set",this);
	setStretchFactor(change,1);

	key_idx=p_key_idx;

	update();

	setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));

	QObject::connect(change,SIGNAL(clicked()),this,SLOT(keybind_change_request()));
}


void Keyboard_Input_Config::update() {

	for (int i=0;i< (int)key_entry_list.size();i++) {

		key_entry_list[i]->update();
	}

}

Keyboard_Input_Config::Keyboard_Input_Config(QWidget *p_parent) : Q3GroupBox ( 1, Qt::Vertical,"Tracker Key Bindings",p_parent) {

	Keyboard_Input *kinput = Keyboard_Input::get_singleton_instance();

	scroll = new Q3ScrollView(this);

	scroll->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	scroll->setResizePolicy(Q3ScrollView::AutoOneFit);
	scroll->enableClipper(true);

	vbox = new Q3VBox(scroll);
	scroll->addChild(vbox);

	for (int i=0;i< kinput->get_key_bind_count();i++) {

		key_entry_list.push_back( new Key_Entry(vbox,i)  );
	}
}


Keyboard_Input_Config::~Keyboard_Input_Config()
{
}


