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
                          song.h  -  description
                             -------------------
    begin                : Sat Jan 20 2001
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

#ifndef SONG_H
#define SONG_H


/**Song Class
  *@author Juan Linietsky
  */
/******************************
 song.h
 --------

Overall Song structure!
********************************/

#include "order.h"
#include "pattern.h"
#include "sample.h"
#include "instrument.h"
#include "components/data/property_bridges.h"


#define MAX_PATTERNS 200
#define MAX_ORDERS 255
#define MAX_SAMPLES 200
#define MAX_INSTRUMENTS 200

class Song {

	Order order[MAX_ORDERS];
	Pattern pattern[MAX_PATTERNS];
	Sample sample[MAX_SAMPLES];
 	Instrument instrument[MAX_INSTRUMENTS];

	struct Song_Variables { // variables that wont change in playback

         	string name;
		string filename;
		string message;

		int row_highlight_minor;
		int row_highlight_major;

		int mixing_volume;
		int stereo_separation;

		bool use_stereo;
		bool use_linear_slides;

		bool old_effects;
		bool compatible_gxx;


		/* Property bridges, dont mind those */
		String_Property_Bridge prop_name;
		String_Property_Bridge prop_message;
		String_Property_Bridge prop_filename;

		/* Different spelling from above */
		Int_Property_Bridge prop_hilight_major;
		Int_Property_Bridge prop_hilight_minor;

		Int_Property_Bridge prop_mixing_volume;
		Int_Property_Bridge prop_stereo_separation;

		Bool_Property_Bridge prop_use_stereo;
		Bool_Property_Bridge prop_use_linear_slides;

		Bool_Property_Bridge prop_old_effects;
		Bool_Property_Bridge prop_compatible_gxx;

		Song_Variables();

	};

	struct Initial_Variables { // Initial values used for playback

        	struct Channel_State {

			int pan,volume; // 0--64
			bool surround;
			bool mute;

			Int_Property_Bridge prop_pan;
			Int_Property_Bridge prop_volume;

			Bool_Property_Bridge prop_surround;
			Bool_Property_Bridge prop_mute;
			Channel_State();
		};

		int global_volume;
		int speed;
		int tempo;

		Channel_State channel[PATTERN_WIDTH];


		Int_Property_Bridge prop_global_volume;
		Int_Property_Bridge prop_speed;
		Int_Property_Bridge prop_tempo;
		Initial_Variables();
	};

public:
	/* variables */

	Song_Variables variables;
	Initial_Variables initial_variables;

	/* retrieving */
	Pattern* get_pattern(int p_pattern);
	Sample* get_sample(int p_sample);
	Instrument* get_instrument(int p_instrument);

	Order get_order(int p_position);
	Order get_next_order(int p_position);
	int get_used_orders_amount();
	int get_patterns_used();
        int find_pattern_in_orderlist(int p_pattern);

	void clear_unused_samples();
	void clear_all_patterns();
	void clear_all_instruments();
	void clear_all_orders();
	void recreate_all_instruments();
	void clear_default_sample_pannings();
	void clear_default_instrument_pannings();

	/* setting */
	void set_order(int p_position,Order p_order);

	/* get max values */

	int get_max_orders() { return MAX_ORDERS; };
	int get_max_patterns() { return MAX_PATTERNS; };
	int get_max_samples() { return MAX_SAMPLES; };
	int get_max_instruments() { return MAX_INSTRUMENTS; };
	int get_max_used_pattern() { return 25; };

	/* utils */

	void set_instrument_mode();
	void set_sample_mode();
        int find_unused_instrument();
        int find_sample_in_instrument(int p_sample);
        int find_first_sample_in_instrument(int p_instrument);
        int find_unused_sample_slot();
	void make_instrument_out_of_sample_at(int p_sample_pos,int p_instr_pos);
	void make_instrument_out_of_sample(int p_sample_pos);

        void reset_data();

	/* Loading */


	/* PROPERTY BRIDGES */

	

	
	Song();
	~Song();

};

#endif
