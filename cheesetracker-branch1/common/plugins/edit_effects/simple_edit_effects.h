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
// C++ Interface: simple_edit_effects
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SIMPLE_EDIT_EFFECTS_H_
#define SIMPLE_EDIT_EFFECTS_H_

#include "components/audio/edit_effect.h"


class Edit_Effect_Reverse : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);

	/* Info */
	virtual bool is_destructive() { return false; }
	virtual string get_name() { return "Reverse"; }
	virtual string get_description() { return "Flips the sample horizontally"; }


};

class Edit_Effect_Center : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);


	/* Info */
	virtual bool is_destructive() { return false; }
	virtual string get_name() { return "Center"; }
	virtual string get_description() { return "Centers the sample"; }
};

class Edit_Effect_Toggle_Depth : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);


	/* Info */
	virtual bool is_destructive() { return true; }
	virtual string get_name() { return "Toggle 16/8bits"; }
	virtual string get_description() { return "Toggles between 8bits/16bits mode"; }
};

class Edit_Effect_Toggle_Sign : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);


	/* Info */
	virtual bool is_destructive() { return false; }
	virtual string get_name() { return "Toggle Signed/Unsigned"; }
	virtual string get_description() { return "Toggles between unsigned (AMIGA/PPC) and signed (PC)"; }
};


class Edit_Effect_SelToLoop : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);

	/* Info */
	virtual bool is_destructive() { return false; }
	virtual string get_name() { return "Selection -> Loop"; }
	virtual string get_description() { return "Moves loop points to the selection area"; }


};

class Edit_Effect_FadeIn : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);

	/* Info */
	virtual bool is_destructive() { return false; }
	virtual string get_name() { return "Fade In"; }
	virtual string get_description() { return "Peforms a crescent volume ramp"; }


};

class Edit_Effect_FadeOut : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);

	/* Info */
	virtual bool is_destructive() { return false; }
	virtual string get_name() { return "Fade Out"; }
	virtual string get_description() { return "Perform a decrescent volume ramp"; }


};

class Edit_Effect_Tunner : public Edit_Effect {

	size_t cycles;
	Int_Property_Bridge cycle_prop;
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) { cycles=4; }
	virtual list<Property_Bridge*> get_property_list();

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);

	/* Info */
	virtual bool is_destructive() { return false; }
	virtual string get_name() { return "C5 Tunner"; }
	virtual string get_description() { return "Given the Cycles inside the selection\nTunes the sample to C5"; }

	Edit_Effect_Tunner();
};

class Edit_Effect_Normalize : public Edit_Effect {

	int amp;
	enum {
		MULT_MAX=999999
	};
	Int_Property_Bridge amp_prop;
public:

	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);
	virtual list<Property_Bridge*> get_property_list();


	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);

	/* Info */
	virtual bool is_destructive() { return false; }
	virtual string get_name() { return "Amp/Normalize"; }
	virtual string get_description() { return "Performs Amplification,\n defaults to normalize"; }

        Edit_Effect_Normalize();
};

class Edit_Effect_PostLoop_Cut : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);

	/* Info */
	virtual bool is_destructive() { return true; }
	virtual string get_name() { return "Post-Loop Cut"; }
	virtual string get_description() { return "Removes data after Loop-Endpoint"; }


};

class Edit_Effect_PreLoop_Cut : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);

	/* Info */
	virtual bool is_destructive() { return true; }
	virtual string get_name() { return "Pre-Loop Cut"; }
	virtual string get_description() { return "Removes data before Loop-Beginpoint"; }


};


class Edit_Effect_Trim : public Edit_Effect {
public:

	/* On Select info.. */
	virtual void selected_notify(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end) {}
	virtual list<Property_Bridge*> get_property_list() { list<Property_Bridge*> empty; return empty; }

	/* Processing */
	virtual void process(Sample_Data *p_data,ptrdiff_t p_begin,ptrdiff_t p_end);

	/* Info */
	virtual bool is_destructive() { return true; }
	virtual string get_name() { return "Trim to selectionn"; }
	virtual string get_description() { return "Removes data before and after selection"; }


};





#endif

