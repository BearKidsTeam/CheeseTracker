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
                          lfo.h  -  description
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

#ifndef LFO_H
#define LFO_H


/**
  *@author Juan Linietsky
  */

class LFO {
public:
	
	enum Wave {
	
		SINE,
		SAW,
		SQUARE,
		NOISE
	};

protected:

	bool enable;
	bool sync;
	
	float delay;
	float speed;
	float depth;
	
	Wave wave;	



public:

	//if LFO is enabled
	void set_enabled(bool p_enable);
	bool is_enabled();

	//if LFO is the same for other voices (LFO SYNC)
	void set_sync(bool p_sync);
	bool is_synced();

	//delay before starting
	void set_delay(float p_delay);
	float get_delay();

	//speed (cycle length in second)
	void set_speed(float p_speed);
	float get_speed();

	//speed (wave depth in semitones or other unit)
	void set_depth(float p_depth);
	float get_depth();

	//speed (wave type)
	void set_wave(Wave p_wave);
	Wave get_wave();
	

																					
	void reset();

	LFO();
	~LFO();
};

#endif
