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


#include <qhbox.h>
#include <qvbox.h>
#include <qlistbox.h>

/**
@author Juan Linietsky
*/

class Effect_Buffers_Editor : public QHBox {
	Q_OBJECT

	QListBox *buffer_list;
        QVBox *buffer_edit;
	QListBox *insert_effect_list;
	QListBox *insert_effect_move_up;

public:
	Effect_Buffers_Editor(QWidget *p_parent);
	~Effect_Buffers_Editor();

};

#endif
