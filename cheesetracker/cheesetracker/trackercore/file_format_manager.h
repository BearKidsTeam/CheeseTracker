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
                          file_format_manager.h  -  description
                             -------------------
    begin                : Thu Apr 12 2001
    copyright            : (C) 2001 by Juan Linietsky
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

#ifndef FILE_FORMAT_MANAGER_H
#define FILE_FORMAT_MANAGER_H

#include <list>
	using std::list;
#include "loader.h"
#include "saver.h"

/**Collection of loaders/savers/etc
  *@author Juan Linietsky
  */

/******************************
 file_format_manager.h
 ----------

Abstracts file access, so it can handle
multiple file formats for one kind
of operation.
********************************/




class File_Format_Manager {

	typedef list<Loader*> Loader_List;
	typedef list<Saver*> Saver_List;

	Loader_List loader_list;
	Saver_List saver_list;

	Song * song;


	
public:

	void register_loader(Loader *p_loader);
        Loader::Error load_module(const char *p_filename);

        int load_sample(const char *p_filename,int p_dest_index);
        Sample_Data* load_sample(const char *p_filename);
        int load_instrument(const char *p_filename,int p_dest_index);

        Loader* load_sample_library(const char *p_filename); // Usually, from an instrument, but soundfonds would be cool!

	void register_saver(Saver *p_saver);
        int save_module(const char *p_filename);

	int save_sample(const char *p_filename, int p_index);
	int save_instrument(const char *p_filename, int p_index);

	void link_to_song(Song *p_song) { song=p_song; };
	Loader* check_file_in_loaders(const char *p_filename);
	
	File_Format_Manager();
	~File_Format_Manager();
};



#endif
