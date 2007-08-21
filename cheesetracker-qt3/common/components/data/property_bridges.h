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
                          property_bridges.h  -  description
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

#ifndef PROPERTY_BRIDGES_H
#define PROPERTY_BRIDGES_H


#include "typedefs.h"
#include <sigc++/sigc++.h>
/**
  *@author Juan Linietsky
  */

  #define BRIDGE_COPY_VALUE(T) \
	virtual bool copy_value(const Property_Bridge& rvalue) {\
		T* rcast = dynamic_cast<T*>((T*)&rvalue);\
		if (rcast) {\
			set(rcast->get());\
			return false;\
		};\
		return true;\
	};

class Property_Bridge {
protected:

	string type;
	string shortname;
	string name;
	Property_Bridge() {};
	string display_preference;

public:

	SigC::Signal0<void> changed_signal;

	virtual void set_display_preference(string p_pref);
	virtual string get_display_preference();

	virtual bool copy_value(const Property_Bridge& rvalue)=0;

	virtual string get_name() { return name; };
	virtual string get_short_name() { return shortname; };
	virtual string get_type() { return type; };

	virtual ~Property_Bridge() {};
};


class Int_Property_Bridge : public Property_Bridge {

//This one accepts "slider" or "spin" as display preferences

	int *property;
	int max,min;
public:

	virtual void set(int p_val);
	virtual int get();
	virtual int get_min() { return min; };
	virtual int get_max() { return max; };

	BRIDGE_COPY_VALUE(Int_Property_Bridge);

	Int_Property_Bridge(string p_name,int *p_property,int p_min,int p_max,string p_shortname="");
};

class IntListPropertyBridge : public Property_Bridge {

	int *property;
	int size, min, max;

public:
	static string stype;

	virtual void set(int p_pos, int p_val);
	virtual int get(int p_pos);
	virtual int get_min() { return min; };
	virtual int get_max() { return max; };

	virtual int get_size() { return size; };
	bool copy_value(const Property_Bridge& rvalue);

	IntListPropertyBridge(string p_name, int *p_property, int p_size, int p_min, int p_max, string p_shortname = "");
};

class FloatListPropertyBridge : public Property_Bridge {

	float **property;
	int size;
	float min, max;

public:
	static string stype;

	virtual void set(int p_pos, float p_val);
	virtual float get(int p_pos);
	virtual float get_min() { return min; };
	virtual float get_max() { return max; };

	virtual int get_size() { return size; };
	bool copy_value(const Property_Bridge& rvalue);

	FloatListPropertyBridge(string p_name, float **p_property, int p_size, float p_min, float p_max, string p_shortname = "");
};

class Float_Property_Bridge : public Property_Bridge {

	float *property;
	float max,min;
public:

	virtual void set(float p_val);
	virtual float get();
        virtual float get_min() { return min; };
        virtual float get_max() { return max; };

	BRIDGE_COPY_VALUE(Float_Property_Bridge);

	Float_Property_Bridge(string p_name,float *p_property,float p_min,float p_max,string p_shortname="");

};


class Bool_Property_Bridge : public Property_Bridge {

	bool *property;
public:

	virtual void set(bool p_val);
	virtual bool get();

	BRIDGE_COPY_VALUE(Bool_Property_Bridge);

	Bool_Property_Bridge(string p_name,bool *p_property,string p_shortname="");

};

class String_Property_Bridge : public Property_Bridge {

	string *property;
	int max_size;
	//This one accepts "single_line" or "multi_line" as display preferences

public:

	virtual void set(const string& p_text);
	virtual const string& get();
	int get_max_length();

	BRIDGE_COPY_VALUE(String_Property_Bridge);

	//max_size == -1 for no maximum
	String_Property_Bridge(string p_name,string *p_property,int p_max_size=-1,string p_shortname="");
	String_Property_Bridge(string p_name,string *p_property,string p_shortname);

};

class Options_Property_Bridge : public Int_Property_Bridge {

	int *property;
	const char ** options_text;
	int options_count;

public:

	virtual const char * get_option_text(int p_which);
	virtual int get_option_count();

	BRIDGE_COPY_VALUE(Options_Property_Bridge);

	Options_Property_Bridge(string p_name,int *p_property,const char *p_options_text[],int p_options_count,string p_shortname="");
};

#endif
