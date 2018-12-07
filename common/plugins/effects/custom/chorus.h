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
                          chorus.h  -  description
                             -------------------
    begin                : Wed Jul 17 2002
    copyright            : (C) 2002 by red
    email                : red@server
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHORUS_H
#define CHORUS_H

#include "typedefs.h"
#include <vector>

	using std::vector;
/**
  *@author red
  */

class Chorus {

	Chorus() {};

 	float delay;
   	float lfo_speed;
     	float lfo_depth;
       	float feedback;
 	float width;
  
  	int index; //osc index
    	int ring_buffer_index;

   	vector<Sint32> ringbuffer;

    	int mixfreq;
   
public:

	void set_mixfreq(int p_mixfreq);

	void process(Sint32 *p_dest,Sint32 *p_source,int p_amount);

	void set_chorus_params(float p_delay, float p_lfo_speed, float p_lfo_depth, float p_feedback, float p_width);
        void get_chorus_params(float &p_delay, float &p_lfo_speed, float &p_lfo_depth, float &p_feedback, float &p_width);
		
	Chorus(int p_ring_buffer_size);
	~Chorus();
};

#endif
