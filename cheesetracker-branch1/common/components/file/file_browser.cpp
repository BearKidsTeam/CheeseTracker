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
                          file_browser.cpp  -  description
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

#include "file_browser.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

void FileBrowser::regenerate_listing() {

	char real_current_dir_name[500];
        typedef struct stat Stat;
	DIR *dir_stream;
	dirent *entry;
	Stat flags;

	getcwd(real_current_dir_name,500);
	chdir(current_dir.c_str());
	dir_stream = opendir(current_dir.c_str());
				
	file_list.clear();
	dir_list.clear(); //TODO -- add C:/D:/E:/etc for DOS!
	
	while (dir_stream!=NULL) {
	
		string aux_name;
		entry=readdir(dir_stream);
		if (entry==NULL) break;
		if (stat(entry->d_name,&flags)) break;
		aux_name=entry->d_name;

		if (S_ISDIR(flags.st_mode)) {
			
			dir_list.push_back(aux_name);
						
		} else if (S_ISREG(flags.st_mode)) {

			file_list.push_back(aux_name);
		}
	}
	
	closedir(dir_stream);
	chdir(real_current_dir_name);	
		
	dir_list.sort();	
	file_list.sort();
}

list<string>::const_iterator FileBrowser::get_file_list() {

	return file_list.begin();
}
list<string>::const_iterator FileBrowser::get_file_list_end() {

	return file_list.end();

}

list<string>::const_iterator FileBrowser::get_dir_list() {

	return dir_list.begin();

}

list<string>::const_iterator FileBrowser::get_dir_list_end() {

	return dir_list.end();

}

		
void FileBrowser::change_dir(string p_dir) {


	char fake_current_dir_name[500];
	char real_current_dir_name[500];
	string old_dir;
	old_dir=current_dir;
	if (p_dir.length()==0) return;

	//TODO - detect dos C:/D:/E:/F:/etc
	
	if (p_dir.substr(0,1)=="/") {
	
		current_dir=p_dir;
	} else {
	
		current_dir+="/"+p_dir;
	}

	getcwd(real_current_dir_name,500);
	
	if (chdir(current_dir.c_str())){ //invalid directory!! (YARR!!)

	 	current_dir=old_dir;
	 	return; //no luck!
	}

	getcwd(fake_current_dir_name,500);

	chdir(real_current_dir_name);	
	current_dir=fake_current_dir_name;

	
	regenerate_listing();
}

string FileBrowser::get_dir() {

	return current_dir;
}

void FileBrowser::create_dir(string cd) {

		
}
	
void FileBrowser::is_file(string p_fname) {


}

void FileBrowser::is_dir(string p_fname) {


}

void FileBrowser::exists(string p_fname) {


}

	
void FileBrowser::set_extension_filter(string p_extension) {


}



FileBrowser::FileBrowser(){


current_dir=".";

}
FileBrowser::~FileBrowser(){


}
