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
                          editor_undo.cpp  -  description
                             -------------------
    begin                : Sun Apr 15 2001
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

#include "editor.h"

void Editor::push_current_pattern_to_undo_list(string p_reason) {

	push_pattern_to_undo_list(current_pattern,song->get_pattern(current_pattern),p_reason);
}

void Editor::push_pattern_to_undo_list(int p_index,Pattern *p_pattern,string p_reason) {

	int i;

	if ((undo_max>=0) && (p_reason==undo_buffer[0].description)) return;
	// [[todo]]Avoid sending the same type of operation consecutively to the undo buffer


	if (undo_max==(MAX_UNDO_OPERATIONS-1)) {

		delete undo_buffer[undo_max].pattern_data;
	
	} else {

		undo_max++;
	}

	for (i=undo_max;i>0;i--) {

		undo_buffer[i]=undo_buffer[i-1];
	}


	
	undo_buffer[0].pattern_data = new Pattern();
	*undo_buffer[0].pattern_data=*p_pattern;
	undo_buffer[0].pattern_index=p_index;
	undo_buffer[0].description=p_reason;
	undo_display_pending=true;
}

void Editor::undo_index(int p_undo_index) {

	string reason;
	Pattern aux_pattern;

	if ((p_undo_index<0) || (p_undo_index>undo_max)) return;

	aux_pattern=*song->get_pattern(current_pattern);
	*song->get_pattern(current_pattern)=*undo_buffer[p_undo_index].pattern_data;

	push_pattern_to_undo_list(undo_buffer[p_undo_index].pattern_index,&aux_pattern,(string)"Redo "+undo_buffer[p_undo_index].description);
	undo_display_pending=true;
	
}

int Editor::get_undo_max() {

	return undo_max+1;
}

int Editor::get_undo_pattern(int p_undo_index) {

	if ((p_undo_index<0) || (p_undo_index>undo_max)) return 0;

	return undo_buffer[p_undo_index].pattern_index;

}

string Editor::get_undo_info(int p_undo_index) {

	if ((p_undo_index<0) || (p_undo_index>undo_max)) return "invalid pattern!";

	return undo_buffer[p_undo_index].description;
}

