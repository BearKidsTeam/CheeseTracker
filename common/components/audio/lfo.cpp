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
                          lfo.cpp  -  description
                             -------------------
    begin                : Sun Mar 10 2002
    copyright            : (C) 2002 by Juan Linietsky
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

#include "lfo.h"


//if LFO is enabled
void LFO::set_enabled(bool p_enable) {

	enable=p_enable;
}
bool LFO::is_enabled() {

	return enable;
}

//if LFO is the same for other voices (LFO SYNC)
void LFO::set_sync(bool p_sync) {

	sync=p_sync;
}
bool LFO::is_synced() {

	return sync;
}

//delay before starting
void LFO::set_delay(float p_delay) {

	if (p_delay<0.0) return;
	if (p_delay>20.0) return;
	delay=p_delay;
}
float LFO::get_delay() {

	return delay;
}

//speed (cycle length in second)
void LFO::set_speed(float p_speed) {

	if (p_speed<0.05) return;
	if (p_speed>100.0) return;

	speed=p_speed;
}
float LFO::get_speed() {

	return speed;
}

//speed (wave depth in semitones or other unit)
void LFO::set_depth(float p_depth) {

	if (p_depth<0.0) return;
	if (p_depth>100.0) return;

	depth=p_depth;
}
float LFO::get_depth() {

	return depth;
}

//speed (wave type)
void LFO::set_wave(Wave p_wave) {

	wave=p_wave;
}

LFO::Wave LFO::get_wave() {

	return wave;
}


void LFO::reset() {

	enable=false;
	sync=false;
	speed=0.05;
	depth=0.01;
	delay=0.0;
	wave=SINE;
}


LFO::LFO(){

	reset();
}
LFO::~LFO(){
}
