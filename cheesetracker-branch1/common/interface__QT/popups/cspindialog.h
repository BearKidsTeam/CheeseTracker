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

#ifndef CSPINDIALOG_H
#define CSPINDIALOG_H


#include <qdialog.h>
#include <q3vbox.h>
#include <qlabel.h>
#include <q3hbox.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>


#include "interface__QT/helpers/cspinbutton.h"

/**
 *
 * Juan Linietsky
 **/
class CSpinDialog : public QDialog {


	Q3VBoxLayout *main_vbox;
	Q3HBox *value_hbox;
	Q3HBox *options_hbox;;

	CSpinButton *spinbutton;
	QLabel *description;

	QPushButton *ok_button;
	QPushButton *cancel_button;

public:

	CSpinButton *get_spin();

        void set_description(const QString &p_desc);

	CSpinDialog(QWidget *p_parent);
	~CSpinDialog();
};

#endif
