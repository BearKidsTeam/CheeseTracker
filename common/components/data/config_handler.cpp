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
#include "config_handler.h"

void ConfigHandler::add_section(string p_name) {

	Section new_section;

	if (section_exists(p_name)) return;

	section_list[p_name]=new_section;
}

void ConfigHandler::add_property(string p_section,string p_name,string p_value,string p_comment) {

	section_list[p_section].property_list[p_name].value=p_value;
	section_list[p_section].property_list[p_name].comment=p_comment;
}

void ConfigHandler::add_property(string p_section,string p_name,int p_value,string p_comment) {

	Property new_prop;
	static char name[40];
	string svalue;

	name[39]=0;

	if (property_exists(p_section,p_name)) return;

	snprintf(name,39,"%i",p_value);
	svalue=name;

	section_list[p_section].property_list[p_name].value=svalue;
	section_list[p_section].property_list[p_name].comment=p_comment;
}

int ConfigHandler::get_int_property(string p_section,string p_name) {

	if (!property_exists(p_section,p_name)) return 0;

	return atoi(section_list[p_section].property_list[p_name].value.c_str());
}

string ConfigHandler::get_str_property(string p_section,string p_name) {

	if (!property_exists(p_section,p_name)) return " ";

	return section_list[p_section].property_list[p_name].value;
}

bool ConfigHandler::section_exists(string p_name) {

	return section_list.find(p_name)!=section_list.end();
}

bool ConfigHandler::property_exists(string p_section,string p_name) {

	if (!section_exists(p_section)) return false;

	return section_list[p_section].property_list.find(p_name)!=section_list[p_section].property_list.end();
}

void ConfigHandler::set_header_check(string p_header_check) {

	header_check=p_header_check;
}

int ConfigHandler::save_to_file(string p_filename) {


	return 1;
}

int ConfigHandler::load_from_file(string p_filename) {

	return 0;
}


int ConfigHandler::save_to_text(string p_filename) {
	FILE *f;

	if ( !(f=fopen(p_filename.c_str(),"wb")) ) return false;


 	fprintf(f,"[%s]\n\n",header_check.c_str());

	for (Section_List::iterator I=section_list.begin();I!=section_list.end();I++) {

	 	fprintf(f,"\n\n[%s]\n\n",I->first.c_str());

		for (Property_List::iterator J=I->second.property_list.begin();J!=I->second.property_list.end();J++) {

			if (J->second.comment!="") {

	        		fprintf(f,"; %s\n",J->second.comment.c_str());
			}

			string prop=J->second.value;
			if (prop.find(";")<prop.length())
				prop.replace( prop.find(";"), 1,"\\;");
			if (prop.find("\n")<prop.length())
				prop.replace( prop.find("\n"), 1,"\\n");

                        fprintf(f,"%s = %s\n",J->first.c_str(),prop.c_str());

		}

	}

	fclose(f);

	return 1;
}

void ConfigHandler::remove_trailing_space(string *p_string) {

	while ((p_string->length()>0) && ((*p_string)[0]==' ')) {

		*p_string=p_string->substr(1,p_string->length()-1);
	}

	while ((p_string->length()>0) && ((*p_string)[p_string->length()-1]==' ')) {

		*p_string=p_string->substr(0,p_string->length()-1);
	}

}

int ConfigHandler::load_from_text(string p_filename) {

	string section="";

	FILE *f;

	if ( !(f=fopen(p_filename.c_str(),"rb")) ) return false;

	while(1) {

		string auxstring;
		char axloadzone[500];

		fscanf(f," %[^\r\n]",axloadzone);

		if (feof(f)) break;

		auxstring=axloadzone;

		auxstring=auxstring.substr(0,auxstring.find(";"));

		if (auxstring.length()==0) continue;

		remove_trailing_space(&auxstring);

		if (auxstring[0]=='[') {

			auxstring=auxstring.substr(1,auxstring.length()-2);

			section=auxstring;
			////printf("got section %s",section.c_str());
			add_section(section);
		} else if (auxstring.find("=")<auxstring.length()) {

			string key,data;

			key=auxstring.substr(0,auxstring.find("="));

			remove_trailing_space(&key);
			data=auxstring.substr(auxstring.find("=")+1,(auxstring.length()-auxstring.find("="))-1);

			remove_trailing_space(&data);

			////printf("got \"%s\":\"%s\"\n",key.c_str(),data.c_str());

			add_property(section,key,data);
		}
	}

	fclose(f);

	return true;
}

ConfigHandler::ConfigHandler(){
}

ConfigHandler::~ConfigHandler(){
}
