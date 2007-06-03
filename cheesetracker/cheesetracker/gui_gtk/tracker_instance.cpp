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

#include "tracker_instance.h"


Tracker_Instance::Tracker_Instance() {

	pack_start(main_notebook);
	
	main_notebook.append_page(sample_editor,"Sample Editor");
	main_notebook.append_page(instrument_editor,"Instrument Editor");
	
	
}


Tracker_Instance::~Tracker_Instance()
{
}
