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
                          sample.h  -  description
                             -------------------
    begin                : Sat Jan 20 2001
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

#ifndef SAMPLE_H
#define SAMPLE_H


#include "typedefs.h"
#include "components/audio/sample_data.h"
#include "components/data/property_bridges.h"

/**
  *@author Juan Linietsky
  */

/******************************
 sample.h
 --------

Contains the sample modifiers
and attributes. The actual
tech data of it is contained in
sample_data.h
********************************/

struct Sample {

	string name;
	string filename;

	int  def_volume;
	int  glb_volume;

	bool   def_panning_on;
        int  def_panning;

	int  vibrato_type;
	int  vibrato_speed;
	int  vibrato_depth;
	int  vibrato_rate;

	Sample_Data data;

	bool in_use;

	const Sample& operator=(const Sample& r_sample);

        void reset(bool try_to_free_data=true);


	String_Property_Bridge prop_name;
	String_Property_Bridge prop_filename;


	Int_Property_Bridge prop_def_volume;
	Int_Property_Bridge prop_glb_volume;

	Bool_Property_Bridge prop_def_panning_on;
	Int_Property_Bridge prop_def_panning;

	Options_Property_Bridge prop_vibrato_type;
	Int_Property_Bridge prop_vibrato_speed;
	Int_Property_Bridge prop_vibrato_depth;
	Int_Property_Bridge prop_vibrato_rate;

	Sample();
};

#endif
