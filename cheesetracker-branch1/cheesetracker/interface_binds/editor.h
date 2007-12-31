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
                          editor.h  -  description
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

#ifndef EDITOR_H
#define EDITOR_H

#include "song.h"

/**
  *@author Juan Linietsky
  */

/************************************************
 editor.h
 --------
This class aims to be a pure abstraction to the
editing features. This way, programming a GUI
in other OS/Toolkit should be much easier
*************************************************/


/* GLOBAL DEFINITIONS */

#define CHARACTERS_PER_COLUMN 13
#define SEPARATOR_WIDTH font_width
#define ROW_SEPARATOR_PIXELS 1
#define MAX_PIANO_EDIT_KEYS 29 // total upper/lower piano-like-keys-

#define FIELDS_MAX 9
#define CURRENT_NOTE_SET song->get_pattern(current_pattern)->get_note_ref(cursor_x,cursor_y)
#define CURRENT_NOTE_GET song->get_pattern(current_pattern)->get_note(cursor_x,cursor_y)

#define MAX_UNDO_OPERATIONS 10

/* GLOBAL ENUMERATIONS */

enum {

	TRACKER_REDRAW_ROW=1,
	TRACKER_REDRAW_COLUMN=2,
	TRACKER_REDRAW_ALL=4,
	TRACKER_REDRAW_TOP=8,
	TRACKER_REDRAW_PLAYING_ROW=16,
 	MAX_NOTE_INCR=32,
	TRACKER_REPAINT=64,
	TRACKER_UPDATE_INFO_AREAS=128
};


class Editor {
public:
	enum Pattern_Actions {

		Pattern_MARK_BLOCK_BEGIN,
		Pattern_MARK_BLOCK_END,
		Pattern_MARK_COLUMN_SIXTEEN,
		Pattern_MARK_COLUMN_QUARTER_HALF,
		Pattern_MARK_COLUMN_ALL,
		Pattern_BLOCK_COPY,
		Pattern_BLOCK_PASTE_INSERT,
		Pattern_BLOCK_PASTE_OVERWRITE,
		Pattern_BLOCK_UNMARK,
		Pattern_BLOCK_CUT,
		Pattern_BLOCK_RAISE_NOTES,
		Pattern_BLOCK_LOWER_NOTES,
		Pattern_BLOCK_SET_CURRENT_INSTRUMENT,
		Pattern_BLOCK_SET_CURRENT_VOLUME,
		Pattern_BLOCK_WIPE_STRAY_VOLUMES,
		Pattern_BLOCK_RAMP_WIPE_VOLUMES,
		Pattern_BLOCK_RAMP_WIPE_EFFECTS,
		Pattern_BLOCK_DOUBLE_BLOCK_LENGTH,
		Pattern_BLOCK_HALVE_BLOCK_LENGTH,
		Pattern_FIELD_TOGGLE_MASK,
		Pattern_FIELD_CLEAR,
		Pattern_FIELD_COPY_CURRENT,
		Pattern_FIELD_PASTE_CURRENT,

		Pattern_MAX_ACTIONS
	};
private:

	/* POSITIONING VARIABLES */

	int column_offset,row_offset;

        int visible_columns,visible_rows;

	int window_width,window_height;

	int cursor_x,cursor_y,cursor_field,current_pattern;

	int previous_cursor_x,previous_cursor_y;

	int playing_row,playing_row_old;

	int font_height,font_width;

	bool playing;

	/* DATA INSERTION METHODS */

	bool insert_note_at_cursor(char keynote);
	void insert_octave_at_cursor(Uint8 octave);

	void insert_instrument_at_cursor(Uint8 parameter, int field);

	void insert_volume_at_cursor_field1(Uint8 volume);
	void insert_volume_at_cursor_field2(Uint8 volume);

	void insert_command_at_cursor(char command);
	void insert_parameter_at_cursor_field1(Uint8 parameter);
	void insert_parameter_at_cursor_field2(Uint8 parameter);


	/* USER_EDITING_VARIABLES */

	static int default_octave;

	int last_note;
	int last_instrument;	
	int last_volume;
	int last_command;
	int last_parameter;

	bool instrument_mask;
	bool volume_mask;
	bool command_mask;
        bool volume_pan_mode;

        int cursor_spacing;
	
	/* PLAYER MARK */

	int marked_pattern,marked_row;
	bool mark_active;


	/* SELECTION VARIABLES*/

	Pattern clipboard;
	
	int clipboard_width;

	int selection_begin_x,selection_begin_y,selection_end_x,selection_end_y;
	bool selection_active;
	int shift_selection_begin_x,shift_selection_begin_y,shift_selection_end_x,shift_selection_end_y;
	int mouse_selection_begin_x,mouse_selection_begin_y;
	bool shift_selection_in_progress;

	/* Undo Buffer */

	int undo_max;

	struct Undo_Operation {

		int pattern_index;
		string description;
		Pattern *pattern_data;
	};

	Undo_Operation undo_buffer[MAX_UNDO_OPERATIONS];
	bool undo_display_pending;

	void push_pattern_to_undo_list(int p_index,Pattern *p_pattern,string p_reason);

	/* REDRAW OPTIMIZATION FLAGS */

 	int redraw_flags;

	/* ORDERLIST EDITING */

	int orderlist_cursor_x,orderlist_cursor_y;
        /* VISUAL INFO SCREEN */

	int visual_channel;

	/* KEYBOARD CUSTOMIZATION */

	char *note_increments;

	int keyboard_type;
	int get_note_increment(char key);

	bool song_follow;

        static const char* notes[128];
	/* SONG DATA SOURCE */

	Song *song;

public:

	/* RAW DATA RETRIEVING */

	void get_note_string(int column,int row,char *buf);
	string get_single_note_string(int p_note);

	//The visual ones!
	string get_pattern_note_string(Note p_note);
	string get_pattern_instrument_string(Note p_note);
	string get_pattern_volume_string(Note p_note);
	string get_pattern_command_string(Note p_note);


	/* PSEUDO SCREEN INFO RETRIEVING */

	int get_column_width() { return font_width*CHARACTERS_PER_COLUMN+SEPARATOR_WIDTH; };
	int get_row_height() { return get_font_height()+ROW_SEPARATOR_PIXELS; };
	int get_top_height() { return get_row_height()*2; };

	int get_separator_width() { return SEPARATOR_WIDTH; };
	int left_numbers_width() { return 5*font_width+SEPARATOR_WIDTH; };

	int get_visible_columns();
	int get_visible_rows();

	int get_font_height() { return font_height; }
	int get_font_width() { return font_width; }
	int get_window_width() { return window_width; }
	int get_window_height() { return window_height; }
	int get_column_offset() { return column_offset; }
	int get_row_offset() { return row_offset; };

	/* PSEUDO SCREEN INFO SETTING */

	void set_font_height(int value) { font_height=value; };
	void set_font_width(int value) { font_width=value; };
	void set_window_width(int value) { window_width=value; };
	void set_window_height(int value) { window_height=value; };
	void set_column_offset(int value);
	void set_row_offset(int value);
        void adjust_view_to_cursor();

	/* EDITING VARIABLES RETRIEVING */

	int get_cursor_x() { return cursor_x; };
	int get_cursor_y() { return cursor_y; };
        void set_cursor_y(int p_cursor_y) { redraw_flags|=TRACKER_REDRAW_ROW; cursor_y=p_cursor_y; };
        void set_cursor_x(int p_cursor_x) { cursor_x=p_cursor_x; };
	int get_previous_cursor_x() { return previous_cursor_x; };
	int get_previous_cursor_y() { return previous_cursor_y; };

	void selection_begin_at_cursor();
	void selection_end_at_cursor();

	int get_current_pattern() { return current_pattern; };
        void set_current_pattern(int p_current_pattern) {  redraw_flags|=TRACKER_REDRAW_ALL; current_pattern=p_current_pattern;};

	void set_previous_cursor_x(int p_previous_cursor_x) { previous_cursor_x=p_previous_cursor_x; };
	void set_previous_cursor_y(int p_previous_cursor_y) { previous_cursor_y=p_previous_cursor_y; };

	int get_cursor_field() { return cursor_field; };
	void set_cursor_field(int p_f) { if ((p_f>=0) && (p_f<FIELDS_MAX)) cursor_field=p_f; }

        void set_cursor_spacing(int amount) { cursor_spacing=amount;};
        int get_cursor_spacing() { return cursor_spacing; };

        bool get_instrument_mask() { return instrument_mask; };
        void set_instrument_mask_value(int p_value);
        int get_instrument_mask_value();

        bool get_volume_mask() { return volume_mask; };
        bool get_command_mask() { return command_mask; };
	static char get_default_octave();
	static void set_default_octave(char value);

	bool get_volume_pan_mode() { return volume_pan_mode; };

	/* EDITING VARIABLES SETTING */

	void cursor_move_up(int amount);
	void cursor_move_left(int amount);
	void cursor_move_down(int amount);
	void cursor_move_right(int amount);
	void cursor_move_field_right();
	void cursor_move_field_left();
	void cursor_page_up();
	void cursor_page_down();
	void cursor_move_up();//uses cursor_spacing
	void cursor_move_down();//uses cursor_spacing
	void cursor_home();
	void cursor_end();
	void cursor_tab();	
	void cursor_back_tab();
	void cursor_insert();
	void cursor_delete();
        void toggle_mask_at_cursor();
	void clear_field_at_cursor();
	void cursor_next_pattern();
	void cursor_prev_pattern();
	void cursor_goto_pattern(int p_pattern);

	void default_octave_raise();
	void default_octave_lower();

	/* MARK */

	void set_mark();
	int get_marked_pattern() { return marked_pattern;};
	int get_marked_row() { return marked_row; };
	bool is_mark_active() { return mark_active; };


	/* UNDO OPTIONS */

	void undo_index(int p_undo_index);
	int get_undo_max();
	int get_undo_pattern(int p_undo_index);
	string get_undo_info(int p_undo_index);
	bool has_undo_display_pending() { return undo_display_pending;}
	void reset_pending_undo_display() { undo_display_pending=false; }
	void push_current_pattern_to_undo_list(string p_reason);


	/* SELECTION BASIC BLOCK FUNCTIONS */

	void normalize_selection();
	void mark_block_begining();
	void mark_block_end();
        void selection_release();
        void mark_column_all();
	void mark_column_sixteen();
	void mark_column_quarter_half();

	/* SELECTION INFO RETRIEVING */

	int get_selection_begin_x () { return selection_begin_x; };
	int get_selection_begin_y () { return selection_begin_y; };
	int get_selection_end_x () { return selection_end_x; };
	int get_selection_end_y () { return selection_end_y; };
	bool is_selection_active () { return selection_active; };

	/* SELECTION SHIFT/MOUSE SETTING */

	void shift_selection_begin();
	void shift_selection_end();
	void shift_selection_update();

	/* SELECTION BASIC CLIPBOARD OPERATIONS */

	void selection_copy();
	void selection_paste_overwrite();
	void selection_paste_insert();
        void selection_paste_mix();
        void selection_zap();

	/* SELECTION CLIPBOARD MODIFIERS */

        void selection_raise();
        void selection_lower();
        void selection_volume_ramp();
        void selection_volume_scale(int p_percent);
        void selection_parameter_ramp();
        void selection_wipe_stray_volumes();
	void selection_set_volume_mask();
        void selection_set_instrument_mask();
        void selection_expand();
        void selection_shrink();

	/* KEY PRESS HANDLERS */

 	void set_note_increment_table(Uint8* p_table);
        bool press_key_at_cursor(int key); // A-Z (caps) / 0-9
	Uint8 get_note_from_key(int key);
	void press_note_off_at_cursor();
	void press_note_cut_at_cursor();
	void press_mask_write_cursor();
	void get_mask_from_cursor();

	void perform_raise_at_cursor();
	void perform_lower_at_cursor();

	/* ORDERLIST KEY PRESS HANDLERS */

	void orderlist_move_up();
	void orderlist_move_down();
	void orderlist_move_left();
	void orderlist_move_right();
	void orderlist_page_up();
	void orderlist_page_down();

	void orderlist_insert_value(int number);
	void orderlist_add_separator();
	void orderlist_clear_order();
        void orderlist_insert();
	void orderlist_delete();

	/* ORDERLIST INFO RETRIEVING */

	int orderlist_get_cursor_x() { return orderlist_cursor_x; };
	int orderlist_get_cursor_y() { return orderlist_cursor_y; };
	void orderlist_set_cursor_y(int p_where) { orderlist_cursor_y=p_where; };
	void orderlist_set_cursor_x(int p_where) { orderlist_cursor_x=p_where; };
	string orderlist_get_cursor_string(int p_order);

	/* VISUAL INFO SCREEN */

	int get_visual_channel() { return visual_channel; };
	void set_visual_channel(int p_visual_channel) { visual_channel=p_visual_channel; }

	/* CHANNEL SOLO/MUTE */

        void channel_mute_toggle(int p_track);
        void channel_solo_toggle(int p_track);
        void current_channel_mute_toggle();
        void current_channel_solo_toggle();
        bool is_channel_muted(int p_track) { return song->initial_variables.channel[p_track].mute; };
	/* REDRAW OPTIMIZATION CHECKS */

	inline bool flag_redraw_row() { return (redraw_flags & TRACKER_REDRAW_ROW); };
	inline bool flag_redraw_all() { return (redraw_flags==0) || (redraw_flags & TRACKER_REDRAW_ALL); };
	inline bool flag_redraw_top() { return (redraw_flags & TRACKER_REDRAW_TOP); };
        inline bool flag_redraw_playing_row() { return (redraw_flags & TRACKER_REDRAW_PLAYING_ROW); };
	inline bool flag_repaint() { return redraw_flags & TRACKER_REPAINT; };
	inline bool flag_update_info_areas() { return redraw_flags & TRACKER_UPDATE_INFO_AREAS; };

	int get_playing_row() { return playing_row; };
	int get_old_playing_row() { return playing_row_old; };

	/* REDRAW OPTIMIZATIONS SET */

	void clear_redraw_flags() { redraw_flags=0; };

	inline void clear_flag_update_info_areas() { redraw_flags &= ~TRACKER_UPDATE_INFO_AREAS; };
	inline void set_flag_redraw_row() { redraw_flags|=TRACKER_REDRAW_ROW; };
	inline void set_flag_redraw_all() { redraw_flags|=TRACKER_REDRAW_ALL; };
	inline void set_flag_redraw_top() { redraw_flags|=TRACKER_REDRAW_TOP; };
	inline void set_flag_redraw_playing_row() { redraw_flags|=TRACKER_REDRAW_PLAYING_ROW; };
	inline void set_flag_repaint() { redraw_flags |= TRACKER_REPAINT; };
	inline void set_flag_update_info_areas() { redraw_flags |= TRACKER_UPDATE_INFO_AREAS; };

	void notify_set_playing_row(int whichrow) { playing_row_old=playing_row; playing_row=whichrow; playing=true; set_flag_redraw_playing_row(); };
	void notify_stopped_playing(int whichrow) { playing_row_old=playing_row; playing_row=-1; playing=false; set_flag_redraw_playing_row(); };

	bool insert_note_value_at_cursor(int p_note);
        /* MISC FLAGS */
	
	bool is_player_active () { return playing; };
	void set_song_follow(bool p_follow) { song_follow = p_follow; }
	bool is_song_follow() { return song_follow; }

	//row hilight
	int get_hl_major () { return song->variables.row_highlight_major; };
	int get_hl_minor () { return song->variables.row_highlight_minor; };

	void set_hl_major (int amount) { song->variables.row_highlight_major=amount; };
	void set_hl_minor (int amount) { song->variables.row_highlight_minor=amount; };

	void link_to_song(Song *p_song) {

		song=p_song;
	}

	void perform_action(Pattern_Actions p_action); //simplification, gui helper too

	Editor();
	~Editor();
};

#endif
