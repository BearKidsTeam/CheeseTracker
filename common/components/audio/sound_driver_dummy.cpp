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
/***************************************************************************
                          sound_driver.cpp  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Juan Linietsky
    email                : reduz@anime.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sound_driver_dummy.h"

int Sound_Driver_Dummy::get_mix_frequency() {

    return mix_frequency;
}
bool Sound_Driver_Dummy::get_mix_stereo() {

    return mix_stereo;
}
bool Sound_Driver_Dummy::get_mix_16bits() {

    return mix_16bits;
}
int Sound_Driver_Dummy::get_mix_buffer_size() {

    return mix_buffersize;
}

string Sound_Driver_Dummy::get_name() {

    return "Dummy Sound Driver";
}

int Sound_Driver_Dummy::init() {


    active=true;
    return FUNCTION_SUCCESS;
}
bool Sound_Driver_Dummy::finish() {

    active=false;
    return true;
}
bool Sound_Driver_Dummy::is_active() {

    return active;
}

Sound_Driver::Status Sound_Driver_Dummy::process() {

    return IDLE;
}
void Sound_Driver_Dummy::flush() {

}

Sound_Driver_Dummy::Sound_Driver_Dummy(){

	mix_frequency=-1;
	process_mode=POLL;
}
Sound_Driver_Dummy::~Sound_Driver_Dummy(){
}
