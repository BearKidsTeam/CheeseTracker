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
                          file_format_manager.cpp  -  description
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

#include "file_format_manager.h"
#include "Error.h"



File_Format_Manager::File_Format_Manager(){
}
File_Format_Manager::~File_Format_Manager(){
}

void File_Format_Manager::register_loader(Loader *p_loader) {

	if (song==NULL) {

		ERROR("Trying to link a loader when we dont have a song!");
	}

 	loader_list.push_back(p_loader);
	p_loader->set_song(song);
}

Loader::Error File_Format_Manager::load_module(const char *p_filename) {

	Loader_List::const_iterator I;
	Loader::Error  temperr=Loader::FILE_FORMAT_NOT_RECOGNIZED;

	for (I=loader_list.begin();I!=loader_list.end();I++) {

		if ((*I)->test(p_filename)) {

			if ((temperr=(*I)->load(p_filename,true))==Loader::SUCCESS) {

                                song->reset_data();
				(*I)->transfer_data_to_song();
				(*I)->free_info();
				return temperr;
			}
		} else {

			temperr=Loader::FILE_FORMAT_NOT_RECOGNIZED;
		}
	}
	return temperr;
}

void File_Format_Manager::register_saver(Saver *p_saver) {

	if (song==NULL) {

		ERROR("Trying to link a saver when we dont have a song!");
	}

 	saver_list.push_back(p_saver);
	p_saver->set_song(song);
}


int File_Format_Manager::save_module(const char *p_filename) {
	
	Saver_List::const_iterator I;
	int temperr=Saver::UNKNOWN_TYPE;

	for (I=saver_list.begin();I!=saver_list.end();I++) {

		if ((temperr=(*I)->save(p_filename))!=Saver::UNKNOWN_TYPE) {

				return temperr;
		}
	}	
	
	return temperr;
}

int File_Format_Manager::save_sample(const char *p_filename, int p_index) {

	Saver_List::const_iterator I;
	int temperr=Saver::UNKNOWN_TYPE;

	for (I=saver_list.begin();I!=saver_list.end();I++) {

		if ((temperr=(*I)->save_sample(p_filename,p_index))!=Saver::UNKNOWN_TYPE) {

				return temperr;
		}
	}
	return temperr;
}

int File_Format_Manager::save_instrument(const char *p_filename, int p_index) {

	Saver_List::const_iterator I;
	int temperr=Saver::UNKNOWN_TYPE;

	for (I=saver_list.begin();I!=saver_list.end();I++) {

		if ((temperr=(*I)->save_instrument(p_filename,p_index))!=Saver::UNKNOWN_TYPE) {

				return temperr;
		}
	}
	return temperr;
}

Loader* File_Format_Manager::check_file_in_loaders(const char *p_filename) {

	Loader_List::iterator I;

	for (I=loader_list.begin();I!=loader_list.end();I++) {

		if ((*I)->test(p_filename)) {

			if ((*I)->load(p_filename,false)==FUNCTION_SUCCESS) {


				return (*I);
			} else return NULL;

                }
	}

	return NULL;
}

int File_Format_Manager::load_sample(const char *p_filename,int p_dest_index) {

 	Loader_List::iterator I;

	for (I=loader_list.begin();I!=loader_list.end();I++) {

		try {
			if ((*I)->load_sample(p_filename,p_dest_index)==FUNCTION_SUCCESS) {
				return FUNCTION_SUCCESS;
			}
		} catch (File_Corrupt E) {
			ERROR(E.what());
		} catch (File_Error E) {
			return Loader::CANNOT_OPEN_FILE;
		}
	}

	return Loader::FILE_FORMAT_NOT_RECOGNIZED;
}

Sample_Data* File_Format_Manager::load_sample(const char *p_filename) {

 	Loader_List::iterator I;

	for (I=loader_list.begin();I!=loader_list.end();I++) {

		Sample_Data *sd;
		try {
			sd=(*I)->load_sample(p_filename);
			if (sd!=NULL) {
				return sd;
			}
		} catch (File_Corrupt E) {
			ERROR(E.what());
			// A file in one format may appear
			// "corrupt" to a loader designed for
			// another format. Fall through and
			// try the next loader.
		}
		catch (File_Error E) {
			ERROR(E.what());
			// Errors other than file corruption
			// signify problems such as nonexistent
			// files and directories specified instead
			// of regular files.
			return NULL;
		}
	}
	
	return NULL;
}

Loader* File_Format_Manager::load_sample_library(const char *p_filename) {

 	Loader_List::iterator I;

	for (I=loader_list.begin();I!=loader_list.end();I++) {

		if ((*I)->load_samples_from_instrument(p_filename)==FUNCTION_SUCCESS) {

			return (*I);
		}
	}

	return NULL ;
}

int File_Format_Manager::load_instrument(const char *p_filename,int p_dest_index) {

 	Loader_List::iterator I;

	for (I=loader_list.begin();I!=loader_list.end();I++) {

		if ((*I)->load_instrument(p_filename,p_dest_index)==FUNCTION_SUCCESS) {

			return FUNCTION_SUCCESS;

		}
	}

	return Loader::FILE_FORMAT_NOT_RECOGNIZED;
}



