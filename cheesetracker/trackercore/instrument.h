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
                          instrument.h  -  description
                             -------------------
    begin                : Sat Jan 20 2001
    copyright            : (C) 2001 by c0del4rvi4
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

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "typedefs.h"
#include "note.h"
#include "components/audio/envelope.h"

#include "components/data/property_bridges.h"

/******************************
 instrument.h
 ----------

this class describes an instrument,
and some constant values of it
********************************/

/**
  *@author Juan Linietsky
  */

struct Instrument {

	//constants

        enum {

		MAX_LAYERS=4
	};


	enum {

		NNA_NOTE_CUT=0,
		NNA_NOTE_CONTINUE=1,
		NNA_NOTE_OFF=2,
		NNA_NOTE_FADE=3
	};


	enum {

		DCT_DISABLED,
		DCT_NOTE,
		DCT_SAMPLE,
		DCT_INSTRUMENT
	};

	enum {

		DCA_NOTE_CUT,
		DCA_NOTE_OFF,
		DCA_NOTE_FADE,
	};


	/* variables */

	string name;
	string filename;

	struct Buffer {

		int output_index;
	} buffer;

	struct Layer {

        	bool override_instrument_output;
		int output_index;

		Uint8 sample_number[Note::NOTES];
		Uint8 note_number[Note::NOTES];

		int NNA_type;
		int duplicate_check_type;
		int duplicate_check_action;

		struct Volume {

			Envelope envelope;
			int global_amount;
			int fadeout;
			int random_variation;

		} volume;

		struct Panning {

			Envelope envelope;
			bool use_default;
			int default_amount;
			int pitch_separation;
			int pitch_center;
			int random_variation;

		} panning;

		struct Pitch {

			Envelope envelope;
			bool use_as_filter;
			int filter_type; /* 0 - lowpass , 1 - bandpass, 2 - highpass */
			bool use_default_cutoff;
			int default_cutoff;
			bool use_default_Q;
			int default_Q;

		} pitch;

		Bool_Property_Bridge prop_override_instrument_output;
		Int_Property_Bridge prop_output_index;

		Int_Property_Bridge prop_volume_global_amount;
		Int_Property_Bridge prop_volume_fadeout;
		Int_Property_Bridge prop_volume_random;

		Bool_Property_Bridge prop_panning_use_default;
		Int_Property_Bridge prop_panning_default_amount;

		Int_Property_Bridge prop_panning_pitch_center;
		Int_Property_Bridge prop_panning_pitch_separation;
		Int_Property_Bridge prop_panning_random_variation;

		Bool_Property_Bridge pitch_envelope_use_as_filter;
		Bool_Property_Bridge pitch_use_default_cutoff;
		Bool_Property_Bridge pitch_use_default_Q;
		Int_Property_Bridge pitch_default_cutoff;
		Int_Property_Bridge pitch_default_Q;
		Options_Property_Bridge pitch_filter_type;


		const Layer& operator=(const Layer &p_rval);
		void reset();

		Layer();

	};

private:
	 Layer layers[MAX_LAYERS];


public:
	Layer * get_layer(int p_index);

	String_Property_Bridge prop_name;
	String_Property_Bridge prop_filename;
	Int_Property_Bridge prop_buffer;

	const Instrument& operator=(const Instrument &p_rval);
	void reset();

	Instrument();
	~Instrument();
};

#endif
