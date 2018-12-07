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
#include "ccolor_bridge.h"
#include <qcolordialog.h>
//Added by qt3to4:
#include <QLabel>

void CColor_Bridge::color_select() {

	QColor new_color=QColorDialog::getColor(*bridge);
	*bridge=new_color;
	color->set_color(*bridge);

}


void CColor_Bridge::set_color(const QColor& p_color) {

	color->set_color(p_color);
	*bridge=p_color;
}

QColor * CColor_Bridge::get_color() {

	return bridge;
}

CColor_Bridge::CColor_Bridge(QWidget *p_parent,QColor *p_bridge,const QString &p_text) :Q3HBox(p_parent) {

	setFrameStyle(Box+Raised);
	
	bridge=p_bridge;
        label = new QLabel(p_text,this);

	color = new CColor_Panel(this);
	button = new QPushButton("Set",this);
	color->set_color(*p_bridge);

	setStretchFactor(label,4);
	setStretchFactor(button,1);
	setStretchFactor(color,1);

	QObject::connect(button,SIGNAL(clicked()),this,SLOT(color_select()));
}

CColor_Bridge::~CColor_Bridge()
{
}
