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

#include "channel_control.h"

void Channel_Control::reset() {

	memset(this,0,sizeof(*this));

	//slave_voice=NULL;
	mute=false;
	instrument_index=255;
	for (int i=0;i<Instrument::MAX_LAYERS;i++) {

		layers[i].sample_changed=false;

		layers[i].filter.it_cutoff=255;
		layers[i].filter.it_reso=0;
	}
	old_note=255;
	real_note=255;

	override_defaults=false;

}

Channel_Control::Channel_Control() {


	reset();
}


Channel_Control::~Channel_Control()
{
}
