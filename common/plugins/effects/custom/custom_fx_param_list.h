//
// C++ Interface: custom_fx_param_list
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CUSTOM_FX_PARAM_LIST_H
#define CUSTOM_FX_PARAM_LIST_H

#include "components/audio/effect.h"
#include "pvector.h"
/**
@author Juan Linietsky
*/
class Custom_FX_Param_List : public Effect::Parameters {

	
        struct Element {
		
		enum Type {
			TYPE_FLOAT,
			TYPE_BOOL,
			TYPE_INT
		};
		
		Type type;
		
		union {
			float float_v;
			bool bool_v;
			int int_v;
		};
		
		Property_Bridge *bridge;
		
		Element() { 
			type=TYPE_INT;
			int_v=0;
		}		
	};
	
	VectorPointer<Element> properties;
	
	string name,identifier;
	
	list<Property_Bridge*> get_property_list();
	string get_display_name();
	string get_identifier_name();
	

	Custom_FX_Param_List();
public:
	
	void add_float_param(string p_name,string p_identifier,float p_min,float p_max,float p_default=0);
	void add_int_param(string p_name,string p_identifier,int p_min,int p_max,int p_default=0);
	void add_bool_param(string p_name,string p_identifier,bool p_default=0);
	void add_options_param(string p_name,string p_identifier,const char* p_options[],int p_option_count,int p_default);

	
	void set_param_float(int p_index,float p_val);
	void set_param_bool(int p_index,bool p_val);
	void set_param_int(int p_index,int p_val);
		
	float get_param_float(int p_index);
	bool get_param_bool(int p_index);
	int get_param_int(int p_index);
	
	int get_param_count();
	
	Custom_FX_Param_List(string p_name,string p_identifier);
	~Custom_FX_Param_List();

};

#endif
