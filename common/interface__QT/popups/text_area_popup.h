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
// C++ Interface: text_area_popup
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef TEXT_AREA_POPUP_H_
#define TEXT_AREA_POPUP_H_

#include <qdialog.h>
#include <q3vbox.h>
#include <q3hbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <q3textedit.h>
//Added by qt3to4:
#include <Q3VBoxLayout>


class Text_Area_Popup : public QDialog {

	Q3VBoxLayout *main_vbox;
	QPushButton *ok_button;
	Q3TextEdit *text_edit;
public:
	void set_text(const char *p_text);

	Text_Area_Popup();
};

#endif

