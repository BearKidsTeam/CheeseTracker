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
                          effect.h  -  description
                             -------------------
    begin                : Sat Aug 17 2002
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

#ifndef EFFECT_H
#define EFFECT_H

#include <typedefs.h>
#include <sample_defs.h>
#include <list>
	using std::list;
#include "components/data/property_bridges.h"

/**
  *@author red
  */

class Effect : public SigC::Object {
public:	
	class Parameters {
	public:		
		virtual list<Property_Bridge*> get_property_list()=0;
		virtual string get_display_name()=0;
		virtual string get_identifier_name()=0;
		virtual void copy_from(Parameters* rvalue);

		virtual ~Parameters();
	};

private:

	float sampling_rate;
	Parameters *data;
	void sampling_rate_changed(int p_rate);



	Effect();
protected:

	virtual void sampling_rate_changed_notify(); // called by non RT Thread!
	float get_sampling_rate();
	Effect(Parameters *p_data);


public:



	virtual void reset()=0;
        virtual void process_buffer(sample_t* p_buffer,int p_length)=0;

	Parameters *get_parameters();
	
	virtual ~Effect();
	
//        static Effect* create_this(); <-you'd better add one of those to each effect!

};

#endif
