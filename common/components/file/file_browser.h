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
                          file_browser.h  -  description
                             -------------------
    begin                : Fri Jan 18 2002
    copyright            : (C) 2002 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_BROWSER_H
#define FILE_BROWSER_H

#include "typedefs.h"
#include <string>
using std::string;
#include <list>
using std::list;
/**
  *@author 
  */

class FileBrowser {

	list<string> file_list,dir_list;
	string extension_filter;
	string current_dir;

	void regenerate_listing();
public:
	list<string>::const_iterator get_file_list();
	list<string>::const_iterator get_file_list_end();

	list<string>::const_iterator get_dir_list();
	list<string>::const_iterator get_dir_list_end();
		
	void change_dir(string p_dir);
	void create_dir(string cd);
	string get_dir();
	void is_file(string p_fname);
	void is_dir(string p_fname);
	void exists(string p_fname);
	
	
	void set_extension_filter(string p_extension);
	

	FileBrowser();
	~FileBrowser();
};

#endif
