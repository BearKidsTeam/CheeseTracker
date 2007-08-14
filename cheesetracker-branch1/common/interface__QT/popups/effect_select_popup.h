//
// C++ Interface: effect_select_popup
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EFFECT_SELECT_POPUP_H
#define EFFECT_SELECT_POPUP_H

#include "components/audio/effect.h"
#include <qdialog.h>
#include <q3vbox.h>
#include <q3hbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <q3listbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
/**
@author Juan Linietsky
*/
class Effect_Select_Popup : public QDialog {
public:

	Q_OBJECT

	Q3VBoxLayout *main_vbox;
	QPushButton *ok_button;
	QPushButton *cancel_button;
	Q3ListBox *list_box;
	int selected_effect_index;
public slots:

	void selected_effect(int p_which);

public:

	Effect::Parameters *get_selected_effect_params();

	Effect_Select_Popup();
	~Effect_Select_Popup();

};

#endif
