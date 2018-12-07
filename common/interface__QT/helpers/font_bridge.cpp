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
#include "font_bridge.h"
#include <cstdio>
#include <qfontdialog.h>
//Added by qt3to4:
#include <QLabel>


void Font_Bridge::select_new_font() {

    bool ok;
    QFont font = QFontDialog::getFont(&ok, *bridge, this );
    if ( ok ) {
        *bridge=font;
	update_font_descr();
    }
}

void Font_Bridge::update() {

	update_font_descr();
}

void Font_Bridge::update_font_descr() {

	static char buf[500];
	buf[499]=0;

	snprintf(buf,499,"%s - %i",bridge->family().ascii(),bridge->pointSize());
	label_name->setText(buf);

}


Font_Bridge::Font_Bridge(QFont *p_bridge,QWidget *p_parent) : Q3HBox(p_parent)
{
	setFrameStyle(Box+Raised);
	setMargin(4);
	label_name = new QLabel("Font: none 0",this);
	label_name->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	font_change = new QPushButton("Change",this);
	font_change->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding));

	bridge=p_bridge;

	QObject::connect(font_change,SIGNAL(clicked()),this,SLOT(select_new_font()));

	update_font_descr();
}


Font_Bridge::~Font_Bridge()
{
}


