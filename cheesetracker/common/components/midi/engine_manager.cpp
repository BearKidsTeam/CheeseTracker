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
                          engine_manager.cpp  -  description
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

#include "engine_manager.h"



void Engine_Manager::register_engine(Engine *p_engine) {

	engine_list.push_back(p_engine);
}

Engine *Engine_Manager::get_engine(int p_index) {
	
	if ((p_index<0) && (p_index>=(int)engine_list.size())) return NULL;

	return engine_list[p_index];
}

int Engine_Manager::get_engine_count() {

	return engine_list.size();
}


string Engine_Manager::get_engine_name(int p_index) {

	if ((p_index<0) && (p_index>=(int)engine_list.size())) return "<Invalid Engine>";

	return engine_list[p_index]->get_name();
}

void Engine_Manager::passive_poll() {

	for (int i=0;i<engine_list.size();i++)
		engine_list[i]->passive_poll();

}


Engine_Manager::Engine_Manager(){
}
Engine_Manager::~Engine_Manager(){
}
