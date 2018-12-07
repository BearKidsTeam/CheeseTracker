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
                          editor_selection.cpp  -  description
                             -------------------
    begin                : Thu Jan 25 2001
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

// NOTE - The use of the <= operator in for loops in this file
//        is not a fencepost bug. Changing it to < results in
//        the impossibility of doing anything useful with single-row
//        or single-column selections.

#include "editor.h"

void Editor::normalize_selection()
{

	int tmpval;

	if (selection_begin_x>selection_end_x) {

		tmpval=selection_begin_x;
		selection_begin_x=selection_end_x;
		selection_end_x=tmpval;

	}

	if (selection_begin_y>selection_end_y) {

		tmpval=selection_begin_y;
		selection_begin_y=selection_end_y;
		selection_end_y=tmpval;
	}

	if (selection_begin_y<0) selection_begin_y=0;
	if (selection_end_y>(song->get_pattern(current_pattern)->get_length()-1)) selection_end_y=(song->get_pattern(current_pattern)->get_length()-1);

	set_flag_redraw_all();
}



void Editor::mark_block_begining()
{


	selection_begin_x=cursor_x;
	selection_begin_y=cursor_y;

	if (!selection_active) {

		selection_end_x=cursor_x;
		selection_end_y=cursor_y;
		selection_active=1;
	}

	normalize_selection();

}

void Editor::selection_begin_at_cursor()
{
	mouse_selection_begin_x=cursor_x;
	mouse_selection_begin_y=cursor_y;
}

void Editor::selection_end_at_cursor()
{

	selection_begin_x=mouse_selection_begin_x;
	selection_begin_y=mouse_selection_begin_y;

	selection_end_x=cursor_x;
	selection_end_y=cursor_y;
	selection_active=1;
	normalize_selection();

}

void Editor::mark_block_end()
{
	selection_end_x=cursor_x;
	selection_end_y=cursor_y;

	if (!selection_active) {

		selection_begin_x=cursor_x;
		selection_begin_y=cursor_y;
		selection_active=1;
	}

	normalize_selection();

}

void Editor::selection_release()
{
	selection_active=false;
}

void Editor::mark_column_sixteen()
{
	selection_begin_x = selection_end_x = cursor_x;
	selection_begin_y = cursor_y;
	selection_end_y	= cursor_y + 15;
	selection_active= true;
	normalize_selection();
}

void Editor::mark_column_quarter_half()
{
  int ll = get_hl_major();
  if(selection_end_y-selection_begin_y==ll-1 && selection_begin_x==cursor_x && selection_begin_y == cursor_y)
    ll*=2;

  selection_begin_x = selection_end_x = cursor_x;
  selection_begin_y = cursor_y;
  selection_end_y	= cursor_y + (ll-1);
  selection_active= true;
  normalize_selection();
}


void Editor::mark_column_all()
{
	if ( selection_active
	    && (selection_begin_x==selection_end_x)
	    && (selection_begin_x==cursor_x)	
            && (selection_begin_y==0)
            && (selection_end_y==(song->get_pattern(current_pattern)->get_length()-1)) ) {

		selection_begin_x=0;
		selection_end_x=PATTERN_WIDTH-1;
	} else {

         	selection_begin_x=cursor_x;
         	selection_end_x=cursor_x;
		selection_begin_y=0;
		selection_end_y=(song->get_pattern(current_pattern)->get_length()-1);
		selection_active=true;
	}

	normalize_selection();

	

}

	

void Editor::shift_selection_begin()
{
	shift_selection_in_progress=true;
	shift_selection_begin_x=cursor_x;
	shift_selection_begin_y=cursor_y;
}


void Editor::shift_selection_update()
{
	if (shift_selection_in_progress) {
	        selection_active=true;
		selection_begin_x=shift_selection_begin_x;
		selection_begin_y=shift_selection_begin_y;	
		selection_end_x=cursor_x;
		selection_end_y=cursor_y;
		
		normalize_selection();
	}
}

void Editor::shift_selection_end()
{
	shift_selection_in_progress=false;		
}



void Editor::selection_raise()
{
	int i,j;

	if ( selection_active ) {


		push_current_pattern_to_undo_list("Raise notes on selection");

		for (i=selection_begin_y;i<=selection_end_y;i++)

			for (j=selection_begin_x;j<=selection_end_x;j++) {

				song->get_pattern(current_pattern)->get_note_ref(j,i).raise();

			}

	} else perform_raise_at_cursor();

}

void Editor::selection_lower()
{
	int i,j;

	if ( selection_active ) {

		push_current_pattern_to_undo_list("Lower notes on selection");

		for (i=selection_begin_y;i<=selection_end_y;i++)
			for (j=selection_begin_x;j<=selection_end_x;j++) {

				song->get_pattern(current_pattern)->get_note_ref(j,i).lower();

		}

	} else perform_lower_at_cursor();

}

void Editor::selection_zap()
{

	int i,j;

	push_current_pattern_to_undo_list("Selection Zap");

	selection_copy();
	
	for (i=selection_begin_y;i<=selection_end_y;i++)
		for (j=selection_begin_x;j<=selection_end_x;j++) {
			song->get_pattern(current_pattern)->get_note_ref(j,i).clear();

		}

}

	
void Editor::selection_parameter_ramp()
{

	int i,j;
	int value_begin,value_end;

	push_current_pattern_to_undo_list("Selection ramp parameter column");


        if (selection_active && selection_begin_y<selection_end_y) {


		for (j=selection_begin_x;j<=selection_end_x;j++) {
		
				value_begin=song->get_pattern(current_pattern)->get_note(j,selection_begin_y).parameter;
				value_end=song->get_pattern(current_pattern)->get_note(j,selection_end_y).parameter;
	
			for (i=selection_begin_y;i<=selection_end_y;i++) {
	
	                        song->get_pattern(current_pattern)->get_note_ref(j,i).parameter=value_begin+(value_end-value_begin)*(i-selection_begin_y)/(selection_end_y-selection_begin_y);
				
			}
	
		}
	
	}

}
	
void Editor::selection_volume_ramp()
{

	int i,j;
	int value_begin,value_end;

	push_current_pattern_to_undo_list("Selection ramp volume column");

        if (selection_active && selection_begin_y<selection_end_y ) {
	
		for (j=selection_begin_x;j<=selection_end_x;j++) {
		
				value_begin=song->get_pattern(current_pattern)->get_note(j,selection_begin_y).volume;
				value_end=song->get_pattern(current_pattern)->get_note(j,selection_end_y).volume;
	
			if (value_begin<65 && value_end<65) {

				for (i=selection_begin_y;i<=selection_end_y;i++) {
		
		                        song->get_pattern(current_pattern)->get_note_ref(j,i).volume=value_begin+(value_end-value_begin)*(i-selection_begin_y)/(selection_end_y-selection_begin_y);
					
				}
			}
	
		}
	
	}
	
}

//ClipBoard

void Editor::selection_copy()
{

	int i,j;

	clipboard_width=((selection_end_x-selection_begin_x)+1);
	clipboard.set_length((selection_end_y-selection_begin_y)+1);

	for (i=selection_begin_x;i<=selection_end_x;i++)

		for (j=selection_begin_y;j<=selection_end_y;j++) {


			clipboard.get_note_ref(i-selection_begin_x,j-selection_begin_y)=song->get_pattern(current_pattern)->get_note(i,j);

		}

}


void Editor::selection_paste_overwrite()
{

	int i,j;
	int limit_x,limit_y;


	push_current_pattern_to_undo_list("Paste (overwriting)");

	limit_x=cursor_x+(clipboard_width-1);
	if (limit_x>PATTERN_WIDTH-1) limit_x=PATTERN_WIDTH-1;
	
	limit_y=cursor_y+(clipboard.get_length()-1);
	if (limit_y>(song->get_pattern(current_pattern)->get_length()-1)) limit_y=(song->get_pattern(current_pattern)->get_length()-1);
	
	for (i=cursor_x;i<=limit_x;i++)

		for (j=cursor_y;j<=limit_y;j++) {

			song->get_pattern(current_pattern)->get_note_ref(i,j)=clipboard.get_note(i-cursor_x,j-cursor_y);

	}
}

void Editor::selection_paste_mix()
{

	int i,j;
	int limit_x,limit_y;
	Note empty_note;


	push_current_pattern_to_undo_list("Paste (mix)");

	limit_x=cursor_x+(clipboard_width-1);
	if (limit_x>PATTERN_WIDTH-1) limit_x=PATTERN_WIDTH-1;
	
	limit_y=cursor_y+(clipboard.get_length()-1);
	if (limit_y>(song->get_pattern(current_pattern)->get_length()-1)) limit_y=(song->get_pattern(current_pattern)->get_length()-1);
	
	for (i=cursor_x;i<=limit_x;i++)
	
		for (j=cursor_y;j<=limit_y;j++) {

			if (song->get_pattern(current_pattern)->get_note(i,j)==empty_note) {

				song->get_pattern(current_pattern)->get_note_ref(i,j)=clipboard.get_note(i-cursor_x,j-cursor_y);
			}
		
		}
}


void Editor::selection_paste_insert()
{

	int i,j;
	int limit_x,limit_y;

	push_current_pattern_to_undo_list("Paste (inserting)");

	limit_x=cursor_x+(clipboard_width-1);
	if (limit_x>PATTERN_WIDTH-1) limit_x=PATTERN_WIDTH-1;

	limit_y=cursor_y+(clipboard.get_length()-1);
	if (limit_y>(song->get_pattern(current_pattern)->get_length()-1)) limit_y=(song->get_pattern(current_pattern)->get_length()-1);

	for (i=cursor_x;i<=limit_x;i++) {


		for(j=(song->get_pattern(current_pattern)->get_length()-1);j>(limit_y);j--) {

			song->get_pattern(current_pattern)->get_note_ref(i,j)=song->get_pattern(current_pattern)->get_note(i,j-clipboard.get_length());
		}

		for (j=cursor_y;j<=limit_y;j++) {

			song->get_pattern(current_pattern)->get_note_ref(i,j)=clipboard.get_note(i-cursor_x,j-cursor_y);

		}

	}
}

void Editor::selection_volume_scale(int p_percent)
{

	int i,j;
	int value;
	Note tmp_note;

	/* And you would think this little function should be easy... well.. no. */

	push_current_pattern_to_undo_list("Selection volume scale");

        if (selection_active && selection_begin_y<selection_end_y ) {

		for (j=selection_begin_x;j<=selection_end_x;j++) {

				for (i=selection_begin_y;i<=selection_end_y;i++) {

					tmp_note=song->get_pattern(current_pattern)->get_note(j,i);

					value=tmp_note.volume;

					if (value==EMPTY_FIELD) {

						if ((tmp_note.instrument!=EMPTY_FIELD) && (tmp_note.note<Note::NOTES)) {

							value=64;
						} else {

	                                         	continue;
						}

					}

					value=(value*p_percent)/100;

					if (value>64) value=64;

					song->get_pattern(current_pattern)->get_note_ref(j,i).volume=value;
			}
		}
	}
}
void Editor::selection_set_instrument_mask()
{

	int i,j;
	Note tmp_note;

	/* And you would think this little function should be easy... well.. yes. */

	push_current_pattern_to_undo_list("Selection set instrument mask");

        if (selection_active && selection_begin_y<selection_end_y ) {
		for (j=selection_begin_x;j<=selection_end_x;j++) {
			for (i=selection_begin_y;i<=selection_end_y;i++) {
				tmp_note=song->get_pattern(current_pattern)->get_note(j,i);

				if (tmp_note.instrument!=EMPTY_FIELD) {
					song->get_pattern(current_pattern)->get_note_ref(j,i).instrument=last_instrument;
				}
			}

		}
	}
}
void Editor::selection_set_volume_mask()
{

	int i,j;

	/* And you would think this little function should be easy... well.. yes. */

	push_current_pattern_to_undo_list("Selection set volume mask");

        if (selection_active && selection_begin_y<selection_end_y ) {

		for (j=selection_begin_x;j<=selection_end_x;j++) {

			for (i=selection_begin_y;i<=selection_end_y;i++) {

				song->get_pattern(current_pattern)->get_note_ref(j,i).volume=last_volume;
			}

		}
	}
}

void Editor::selection_expand()
{

	int i,j;
	push_current_pattern_to_undo_list("Selection Expand x 2");

        if (selection_active && selection_begin_y<selection_end_y ) {

		int expand_max=(selection_end_y-selection_begin_y);
		expand_max*=2;
		expand_max+=selection_begin_y;
		if (expand_max>=song->get_pattern(current_pattern)->get_length())
			expand_max=song->get_pattern(current_pattern)->get_length()-1;

		for (j=selection_begin_x;j<=selection_end_x;j++) {

			for (i=expand_max;i>=selection_begin_y;i--) {

				int src_note=i-selection_begin_y;

				if ((src_note%2)==0) {
					src_note/=2;
					src_note+=selection_begin_y;
        					song->get_pattern(current_pattern)->get_note_ref(j,i)=song->get_pattern(current_pattern)->get_note(j,src_note);
				} else
					song->get_pattern(current_pattern)->get_note_ref(j,i).clear();
			}

		}
	}
}

void Editor::selection_shrink()
{

	int i,j;
	push_current_pattern_to_undo_list("Selection Shrink x 2");

        if (selection_active && selection_begin_y<selection_end_y ) {

		for (j=selection_begin_x;j<=selection_end_x;j++) {

			for (i=selection_begin_y;i<=selection_end_y;i++) {

				int src_note=i-selection_begin_y;
				if (src_note>=((selection_end_y-selection_begin_y)/2)) {

					song->get_pattern(current_pattern)->get_note_ref(j,i).clear();
				} else {
					src_note*=2;
					src_note+=selection_begin_y;
					song->get_pattern(current_pattern)->get_note_ref(j,i)=song->get_pattern(current_pattern)->get_note(j,src_note);
				}
			}
		}
	}
}

void Editor::selection_wipe_stray_volumes()
{

	int i,j;
	Note tmp_note;

	/* And you would think this little function should be easy... well.. yes. */

	push_current_pattern_to_undo_list("Selection wipe stray volumes");

        if (selection_active && selection_begin_y<selection_end_y ) {

		for (j=selection_begin_x;j<=selection_end_x;j++) {

				for (i=selection_begin_y;i<=selection_end_y;i++) {

					tmp_note=song->get_pattern(current_pattern)->get_note(j,i);
					if ((tmp_note.instrument==EMPTY_FIELD) && ( (tmp_note.note==EMPTY_FIELD) || (tmp_note.note==Note::CUT) || (tmp_note.note==Note::OFF)) ) {

						song->get_pattern(current_pattern)->get_note_ref(j,i).volume=EMPTY_FIELD;
					}
				}
		}
	}
}

