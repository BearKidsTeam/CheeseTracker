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
//
//
// C++ Interface: edit_effect
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef EDIT_EFFECT_H_
#define EDIT_EFFECT_H_

#include <string>
	using std::string;
#include <list>
	using std::list;

#include "components/audio/sample_data.h"
#include "components/data/property_bridges.h"

class Edit_Effect {

public:


	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)=0;
	virtual list<Property_Bridge*> get_property_list()=0;

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end)=0;


	/* Info */
	virtual bool is_destructive()=0;
	virtual string get_name()=0;
	virtual string get_description()=0;

	Edit_Effect();
	virtual ~Edit_Effect();
};


#endif
