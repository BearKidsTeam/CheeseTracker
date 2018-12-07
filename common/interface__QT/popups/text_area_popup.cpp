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
// C++ Implementation: text_area_popup
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "text_area_popup.h"
//Added by qt3to4:
#include <QFrame>
#include <QVBoxLayout>

void Text_Area_Popup::set_text(const char *p_text) {

	text_edit->setText(p_text);

}


Text_Area_Popup::Text_Area_Popup() {

	setCaption("Help");
	main_vbox = new QVBoxLayout(this);
	main_vbox->setAutoAdd(true);

	text_edit = new QTextEdit(this);
	text_edit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	text_edit->setReadOnly(true);

	QHBoxLayout *hv = new QHBoxLayout(this);
	hv->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum));
	QFrame *fr = new QFrame(hv);
	fr->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	ok_button = new QPushButton("Ok",hv);
	QObject::connect(ok_button,SIGNAL(clicked()),this,SLOT(accept()));
	setMinimumWidth(500);
	setMinimumHeight(400);
}

