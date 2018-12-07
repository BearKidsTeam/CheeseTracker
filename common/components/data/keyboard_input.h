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
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H


#include <string>
	using std::string;
#include <vector>
	using std::vector;

#include "components/data/config_handler.h"
/**
@author Juan Linietsky
*/
class Keyboard_Input {

	static Keyboard_Input *singleton_instance;

	struct Bind {

		string name;
		string caption_name;
		char key; //key, ascii 33 to 127
	};


        vector<Bind> bind_list;

public:

	void load_settings(ConfigHandler *p_handler);
	void save_settings(ConfigHandler *p_handler);

	static Keyboard_Input* get_singleton_instance();

	void add_key_bind(string p_name,string p_caption_name,char p_key);

	int get_key_bind_count();

	const Bind& get_key_bind(int p_idx);
        void set_key_bind_key(int p_idx,char p_key);

	char get_key_bind_key(int p_idx);
	char get_key_bind_key_nocase(int p_idx);

	Keyboard_Input();
	~Keyboard_Input();

};

#endif
