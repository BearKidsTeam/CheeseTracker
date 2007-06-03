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

#ifndef _TRACKER_INSTANCE_H_
#define _TRACKER_INSTANCE_H_


#include <gtkmm/box.h>
#include <gtkmm/notebook.h>
#include "gui_gtk/instrument_edit.h"
#include "gui_gtk/sample_edit.h"

/**
 * 
 * Juan Linietsky
 **/
class Tracker_Instance : public Gtk::HBox {

	/* Main Notebook*/
	Gtk::Notebook main_notebook;
	/* Editors */
	Instrument_Edit instrument_editor;
	Sample_Edit sample_editor;
	

public:
  Tracker_Instance();
  ~Tracker_Instance();
};

#endif
