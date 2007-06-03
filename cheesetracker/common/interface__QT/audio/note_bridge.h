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
#ifndef NOTE_BRIDGE_H
#define NOTE_BRIDGE_H
#include "common/components/data/property_bridges.h"
#include <qvbox.h>
#include <qlabel.h>
#include "interface__QT/helpers/cspinbutton.h"
/**
@author Juan Linietsky
*/
 class Q_Note_Bridge : public QHBox {

 Q_OBJECT

	QLabel *label;
	CSpinButton * spin;
	Int_Property_Bridge *bridge;
	Q_Note_Bridge();
private slots:
	void value_changed(int p_newval);


public:
	void set_bridge(Int_Property_Bridge *p_bridge);

	Q_Note_Bridge(QWidget *p_parent);
 };


#endif
