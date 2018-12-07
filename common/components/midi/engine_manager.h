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
                          engine_manager.h  -  description
                             -------------------
    begin                : vie nov 1 2002
    copyright            : (C) 2002 by Juan Linietsky
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

#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include "engine.h"

#include <vector>

	using std::vector;
/**
  *@author Juan Linietsky
  */

class Engine_Manager {

	vector<Engine*> engine_list;

public:

	void register_engine(Engine *p_engine);
	int get_engine_count(); //get how many engines are aviable
	string get_engine_name(int p_index); //get the name of each
	Engine *get_engine(int p_index);
	
//	void load_bank(string p_filename); //functions like this can be added

	void passive_poll(); //passive polling

	Engine_Manager();
	~Engine_Manager();
};

#endif
