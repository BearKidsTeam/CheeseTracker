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
#include "voice.h"

void Voice::clear_status() {

	removed=false;
}

void Voice::removed_from_mixer_override() {


}

void Voice::remove_from_mixer() {

	removed=true;
	on_removal.emit();
	removed_from_mixer_override();
}
bool Voice::was_removed_from_mixer() {

	return removed;
}

bool Voice::can_mix() {

	return true; //allways by default
}

Voice::Voice(){

	removed=false;
}

Voice::~Voice(){

}
