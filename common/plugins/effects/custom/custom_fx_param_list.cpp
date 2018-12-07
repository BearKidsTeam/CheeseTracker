//
// C++ Implementation: custom_fx_param_list
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "custom_fx_param_list.h"

int Custom_FX_Param_List::get_param_count() {

	return properties.size();
}


list<Property_Bridge*> Custom_FX_Param_List::get_property_list() {

	list<Property_Bridge*> bridget_from_GGX_is_a_boy;
	
	for (int i=0;i<properties.size();i++) {
		bridget_from_GGX_is_a_boy.push_back( properties[i].bridge );
	}
	
	return bridget_from_GGX_is_a_boy;
}

string Custom_FX_Param_List::get_display_name() {
	
	return name;	
}

string Custom_FX_Param_List::get_identifier_name() {
	
	return identifier;	
}


Custom_FX_Param_List::Custom_FX_Param_List(string p_name,string p_identifier) {

	name=p_name;
	identifier=p_identifier;
}


void Custom_FX_Param_List::add_float_param(string p_name,string p_identifier,float p_min,float p_max,float p_default) {
	
	properties.resize( properties.size() + 1 );
	Element &e = properties[ properties.size() -1 ];

	e.type=Element::TYPE_FLOAT;
	e.float_v=p_default;
	e.bridge = new Float_Property_Bridge(p_name,&e.float_v,p_min,p_max,p_identifier);
	
}


void Custom_FX_Param_List::add_int_param(string p_name,string p_identifier,int p_min,int p_max,int p_default) {
	
	properties.resize( properties.size() + 1 );
	Element &e = properties[ properties.size() -1 ];

	e.type=Element::TYPE_INT;
	e.int_v=p_default;
	e.bridge = new Int_Property_Bridge(p_name,&e.int_v,p_min,p_max,p_identifier);
	
}

void Custom_FX_Param_List::add_bool_param(string p_name,string p_identifier,bool p_default) {
	
	properties.resize( properties.size() + 1 );
	Element &e = properties[ properties.size() -1 ];

	e.type=Element::TYPE_BOOL;
	e.bool_v=p_default;
	e.bridge = new Bool_Property_Bridge(p_name,&e.bool_v,p_identifier);
}

void Custom_FX_Param_List:: add_options_param(string p_name,string p_identifier,const char* p_options[],int p_option_count,int p_default) {

	properties.resize( properties.size() + 1 );
	Element &e = properties[ properties.size() -1 ];

	e.type=Element::TYPE_INT;
	e.int_v=p_default;
	e.bridge = new Options_Property_Bridge(p_name,&e.int_v,p_options,p_option_count,p_identifier);

}

void Custom_FX_Param_List::set_param_float(int p_index,float p_val) {

	if (p_index<0 || p_index>=properties.size()) {
		ERROR("Parameter out of range!");
		return;
	}
	
	if (properties[p_index].type!=Element::TYPE_FLOAT) {
		ERROR("Wrong property type");
		return;
	}
	
	properties[p_index].float_v=p_val;
	
}

void Custom_FX_Param_List::set_param_bool(int p_index,bool p_val) {
	if (p_index<0 || p_index>=properties.size()) {
		ERROR("Parameter out of range!");
		return;
	}
	
	if (properties[p_index].type!=Element::TYPE_BOOL) {
		ERROR("Wrong property type");
		return;
	}

	properties[p_index].bool_v=p_val;
			
}

void Custom_FX_Param_List::set_param_int(int p_index,int p_val) {
	
	if (p_index<0 || p_index>=properties.size()) {
		ERROR("Parameter out of range!");
		return;
	}

	if (properties[p_index].type!=Element::TYPE_INT) {
		ERROR("Wrong property type");
		return;
	}
		
	properties[p_index].int_v=p_val;
	
}
	
float Custom_FX_Param_List::get_param_float(int p_index) {
	
	if (p_index<0 || p_index>=properties.size()) {
		ERROR("Parameter out of range!");
		return 0;
	}
	
	if (properties[p_index].type!=Element::TYPE_FLOAT) {
		ERROR("Wrong property type");
		return 0;
	}
	
	return properties[p_index].float_v;
	
}
bool Custom_FX_Param_List::get_param_bool(int p_index) {
	
	if (p_index<0 || p_index>=properties.size()) {
		ERROR("Parameter out of range!");
		return false;
	}
	
	if (properties[p_index].type!=Element::TYPE_BOOL) {
		ERROR("Wrong property type");
		return false;
	}
	
	return properties[p_index].bool_v;
}
int Custom_FX_Param_List::get_param_int(int p_index) {
	
	if (p_index<0 || p_index>=properties.size()) {
		ERROR("Parameter out of range!");
		return 0;
	}
	
	if (properties[p_index].type!=Element::TYPE_INT) {
		ERROR("Wrong property type");
		return 0;
	}
	
	return properties[p_index].int_v;
	
}


Custom_FX_Param_List::~Custom_FX_Param_List() {
	
	
	for (int i=0;i<properties.size();i++) {
		delete properties[i].bridge;
	}	
	
	properties.clear();
}


