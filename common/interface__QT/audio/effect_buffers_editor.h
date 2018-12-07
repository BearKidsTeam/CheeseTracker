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
#ifndef EFFECT_BUFFERS_EDITOR_H
#define EFFECT_BUFFERS_EDITOR_H


#include <q3hbox.h>
#include <q3vbox.h>
#include <q3listbox.h>

/**
@author Juan Linietsky
*/

class Effect_Buffers_Editor : public Q3HBox {
	Q_OBJECT

	Q3ListBox *buffer_list;
        Q3VBox *buffer_edit;
	Q3ListBox *insert_effect_list;
	Q3ListBox *insert_effect_move_up;

public:
	Effect_Buffers_Editor(QWidget *p_parent);
	~Effect_Buffers_Editor();

};

#endif
