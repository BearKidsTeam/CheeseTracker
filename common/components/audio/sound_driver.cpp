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

#include "sound_driver.h"

bool Sound_Driver::accepts_blocking() {

	return true;

}

void Sound_Driver::set_mixer_list(vector<Mixer_Base*> *p_mixer_list) {

	mixer_list=p_mixer_list;

}
bool Sound_Driver::does_perform_disk_write() {

	return false;
}
string Sound_Driver::get_disk_writer_extension() {

	return "";
}

string Sound_Driver::get_disk_writer_file() {

	return "";
}

vector<Mixer_Base*> * Sound_Driver::get_mixer_list() {

	return mixer_list;
}
void Sound_Driver::set_disk_writer_file(string p_file) {

}

Sound_Driver::Sound_Driver(){

	mixer_list=NULL;
	mix_frequency=-1;
	process_mode=POLL;
}
Sound_Driver::~Sound_Driver(){
}
