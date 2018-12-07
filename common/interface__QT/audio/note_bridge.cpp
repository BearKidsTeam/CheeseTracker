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
#include "note_bridge.h"
//Added by qt3to4:
#include <QLabel>

void Q_Note_Bridge::value_changed(int p_newval) {

	if (!bridge) return;
	bridge->set(p_newval);
	string suffix;

	int note=p_newval % 12;
	int octave = p_newval / 12;

	static char buf[40];
	buf[39]=0;
	static const char* note_str[12] = {
		"C-",
		"C#",
		"D-",
		"D#",
		"E-",
		"F-",

		"F#",
		"G-",
		"G#",
		"A-",
		"A#",
		"B-"
	};
	snprintf(buf, sizeof(buf)-1," - %s%i  ",note_str[note],octave);

	spin->setSuffix(buf);
}

void Q_Note_Bridge::set_bridge(Int_Property_Bridge *p_bridge) {

	bridge=p_bridge; //now finally we can set the real one and the value
	int bridge_val=bridge->get();
	spin->setMaxValue(bridge->get_max());
	spin->setMinValue(bridge->get_min());
	spin->setValue(bridge_val);

//	spin->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	label->setText(bridge->get_name().c_str());
	value_changed(bridge_val);
}

Q_Note_Bridge::Q_Note_Bridge(QWidget *p_parent) :  Q3HBox(p_parent) {

	bridge=NULL;
	label = new QLabel("Bridge:",this);
	spin = new CSpinButton(this);

	QObject::connect( spin, SIGNAL(valueChanged(int)),this, SLOT(value_changed(int)) );
}

