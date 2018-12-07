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
#include "cspindialog.h"
#include <qpushbutton.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3VBoxLayout>



CSpinButton *CSpinDialog::get_spin() {

	return spinbutton;
}

void CSpinDialog::set_description(const QString &p_desc) {

	description->setText(p_desc);
}

CSpinDialog::CSpinDialog(QWidget *p_parent) : QDialog(p_parent) {

	main_vbox = new Q3VBoxLayout(this);
	main_vbox->setAutoAdd(true);
	//main_vbox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	value_hbox = new Q3HBox(this);

	options_hbox = new Q3HBox(this);

//	value_hbox->insertSpacing(-1,4);
	description = new QLabel(value_hbox);
	spinbutton = new CSpinButton(value_hbox);
//	value_hbox->insertSpacing(-1,4);

	ok_button = new QPushButton("Ok",options_hbox);
	cancel_button = new QPushButton("Cancel",options_hbox);

	QObject::connect(ok_button,SIGNAL(clicked()),this,SLOT(accept()));
	QObject::connect(cancel_button,SIGNAL(clicked()),this,SLOT(reject()));


}

CSpinDialog::~CSpinDialog() {

}
