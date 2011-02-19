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
                          property_bridges.cpp  -  description
                             -------------------
    begin                : dom dic 1 2002
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

#include <cstring>
#include "property_bridges.h"


void Property_Bridge::set_display_preference(string p_pref) {

	display_preference=p_pref;
}
string Property_Bridge::get_display_preference() {

	return display_preference;
}


void Int_Property_Bridge::set(int p_val) {

	if (p_val>max) p_val=max;
	if (p_val<min) p_val=min;

	*property=p_val;
	changed_signal.emit();

}

int Int_Property_Bridge::get() {

	return *property;
}
Int_Property_Bridge::Int_Property_Bridge(string p_name,int *p_property,int p_min,int p_max,string p_shortname) {

	property=p_property;
	max=p_max;
	min=p_min;
	name=p_name;
	shortname=p_shortname;
	type="int";

}


void Float_Property_Bridge::set(float p_val) {

	if (p_val>max) p_val=max;
	if (p_val<min) p_val=min;

	*property=p_val;
	changed_signal.emit();

}

float Float_Property_Bridge::get() {

	return *property;
}
Float_Property_Bridge::Float_Property_Bridge(string p_name,float *p_property,float p_min,float p_max,string p_shortname) {

	property=p_property;
	max=p_max;
	min=p_min;
	name=p_name;
	shortname=p_shortname;
	type="float";

}


string IntListPropertyBridge::stype = "int_array";

void IntListPropertyBridge::set(int p_pos, int p_val) {

	if (p_pos < 0 || p_pos >= size) {

		ERROR("Index out of range!");
		return;
	};

	if (p_val>max) p_val=max;
	if (p_val<min) p_val=min;

	property[p_pos] = p_val;

	changed_signal.emit();
};

bool IntListPropertyBridge::copy_value(const Property_Bridge& rvalue) {

	IntListPropertyBridge* rcast = dynamic_cast<IntListPropertyBridge*>((IntListPropertyBridge*)&rvalue);
	if (!rcast) return true;
	if (size < rcast->size) return true;

	memset(property, 0, size);
	memcpy(property, rcast->property, rcast->size);

	return false;
};

int IntListPropertyBridge::get(int p_pos) {

	if (p_pos < 0 || p_pos >= size) {

		ERROR("Index out of range!");
		return -1;
	};

	return property[p_pos];
};

IntListPropertyBridge::IntListPropertyBridge(string p_name, int *p_property, int p_size, int p_min, int p_max, string p_shortname) {

	property=p_property;
	max=p_max;
	min=p_min;
	size = p_size;
	name=p_name;
	shortname=p_shortname;
	type=stype;
};



string FloatListPropertyBridge::stype = "float_array";

bool FloatListPropertyBridge::copy_value(const Property_Bridge& rvalue) {

	FloatListPropertyBridge* rcast = dynamic_cast<FloatListPropertyBridge*>((FloatListPropertyBridge*)&rvalue);
	if (!rcast) return true;
	if (size < rcast->size) return true;

	memset(property, 0, size);
	memcpy(property, rcast->property, rcast->size);

	return false;
};

void FloatListPropertyBridge::set(int p_pos, float p_val) {

	if (!*property) {
		ERROR("Property NULL");
		return;
	}

	if (p_pos < 0 || p_pos >= size) {

		ERROR("Index out of range!");
		return;
	};

	if (p_val>max) p_val=max;
	if (p_val<min) p_val=min;

	(*property)[p_pos] = p_val;

	changed_signal.emit();
};
float FloatListPropertyBridge::get(int p_pos) {

	if (!*property) {
		ERROR("Property NULL");
		return 0;
	}


	if (p_pos < 0 || p_pos >= size) {

		ERROR("Index out of range!");
		return -1;
	};

	return (*property)[p_pos];
};

FloatListPropertyBridge::FloatListPropertyBridge(string p_name, float **p_property, int p_size, float p_min, float p_max, string p_shortname) {

	property=p_property;
	max=p_max;
	min=p_min;
	size = p_size;
	name=p_name;
	shortname=p_shortname;
	type=stype;
};



void Bool_Property_Bridge::set(bool p_val) {

	*property=p_val;
	changed_signal.emit();
}
bool Bool_Property_Bridge::get() {

	return *property;
}


Bool_Property_Bridge::Bool_Property_Bridge(string p_name,bool *p_property,string p_shortname) {

	name=p_name;
	property=p_property;
	shortname = p_shortname;
	type="bool";
}


void String_Property_Bridge::set(const string& p_text) {


	if (p_text.length()>(unsigned int)max_size) {

		*property=p_text.substr(0,max_size);
	} else {

	        *property=p_text;
	}

        changed_signal.emit();
}

int String_Property_Bridge::get_max_length() {

	return max_size;
}
const string& String_Property_Bridge::get() {

	return *property;
}

//max_size == -1 for no maximum

String_Property_Bridge::String_Property_Bridge(string p_name,string *p_property,int p_max_size,string p_shortname) {

	property=p_property;
	max_size=p_max_size;
	name=p_name;
	shortname=p_shortname;
	type="string";

}

String_Property_Bridge::String_Property_Bridge(string p_name,string *p_property,string p_shortname) {

	property=p_property;
	max_size=-1;
	name=p_name;
	shortname=p_shortname;
	type="string";
}

/* */

const char * Options_Property_Bridge::get_option_text(int p_which) {

	if ( (p_which<0) || (p_which>=options_count)) return NULL;

	return options_text[p_which];
}
int Options_Property_Bridge::get_option_count() {
	return options_count;

}

Options_Property_Bridge::Options_Property_Bridge(string p_name,int *p_property,const char **p_options_text,int p_options_count,string p_shortname) : Int_Property_Bridge(p_name,p_property,0,p_options_count-1,p_shortname) {

	options_text=p_options_text;
	options_count=p_options_count;
	type="options";
}
