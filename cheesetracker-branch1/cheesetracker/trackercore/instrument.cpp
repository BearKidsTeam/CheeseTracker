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
                          instrument.cpp  -  description
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

#include <cassert>
#include <cstring>
#include "instrument.h"

void
Instrument::Layer::reset()
{

    int i;

    NNA_type = NNA_NOTE_CUT;

    duplicate_check_type = DCT_DISABLED;
    duplicate_check_action = DCA_NOTE_CUT;

    volume.envelope.reset();
    volume.envelope.set_max_value(64);
    volume.envelope.set_min_value(0);
    volume.envelope.add_node_at_offset(0, 64);
    volume.envelope.add_node_at_offset(20, 64);
    volume.envelope.set_max_nodes(25);
    volume.envelope.set_min_nodes(2);
    volume.envelope.set_loop_begin(0);
    volume.envelope.set_loop_end(0);
    volume.envelope.set_sustain_loop_begin(0);
    volume.envelope.set_sustain_loop_end(0);

    volume.global_amount = 128;
    volume.fadeout = 0;
    volume.random_variation = 0;

    panning.envelope.reset();
    panning.envelope.set_max_value(32);
    panning.envelope.set_min_value(-32);
    panning.envelope.add_node_at_offset(0, 0);
    panning.envelope.add_node_at_offset(20, 0);
    panning.envelope.set_max_nodes(25);
    panning.envelope.set_min_nodes(2);
    panning.envelope.set_loop_begin(0);
    panning.envelope.set_loop_end(0);
    panning.envelope.set_sustain_loop_begin(0);
    panning.envelope.set_sustain_loop_end(0);


    panning.default_amount = 32;
    panning.use_default = true;
    panning.pitch_separation = 0;
    panning.pitch_center = 48;
    panning.random_variation = 0;

    pitch.envelope.reset();
    pitch.envelope.set_max_value(32);
    pitch.envelope.set_min_value(-32);
    pitch.envelope.add_node_at_offset(0, 0);
    pitch.envelope.add_node_at_offset(20, 0);
    pitch.envelope.set_max_nodes(25);
    pitch.envelope.set_min_nodes(2);
    pitch.envelope.set_loop_begin(0);
    pitch.envelope.set_loop_end(0);
    pitch.envelope.set_sustain_loop_begin(0);
    pitch.envelope.set_sustain_loop_end(0);

    pitch.use_as_filter = false;
    pitch.use_default_cutoff = false;
    pitch.filter_type = 0;
    pitch.default_cutoff = 127;
    pitch.use_default_Q = false;
    pitch.default_Q = 127;

    for (i = 0; i < Note::NOTES; i++) {

	sample_number[i] = EMPTY_FIELD;
	note_number[i] = i;
    }

    override_instrument_output = false;
    output_index = 0;

}

const
Instrument::Layer &
Instrument::Layer::operator = (const Instrument::Layer & p_rval) {


    memcpy(sample_number, p_rval.sample_number, Note::NOTES);
    memcpy(note_number, p_rval.note_number, Note::NOTES);

    NNA_type = p_rval.NNA_type;
    duplicate_check_type = p_rval.duplicate_check_type;
    duplicate_check_action = p_rval.duplicate_check_action;

    volume = p_rval.volume;
    panning = p_rval.panning;
    pitch = p_rval.pitch;
    override_instrument_output = p_rval.override_instrument_output;
    output_index = p_rval.output_index;

    return *this;

}
static const char *
    filter_types[5] = {
    "LowPass (High Resonance)",
    "BandPass",
    "HighPass",
    "LowPass2",
    "Notch"
};
Instrument::Layer::Layer():
prop_override_instrument_output("Override Instrument Output",
				&override_instrument_output),
prop_output_index("Channel#", &output_index, 0, 15),
prop_volume_global_amount("Global Send", &volume.global_amount, 0, 128),
prop_volume_fadeout("Fadeout", &volume.fadeout, 0, 256),
prop_volume_random("Random Send", &volume.random_variation, 0, 100),
prop_panning_use_default("Use Default", &panning.use_default),
prop_panning_default_amount("Default Pan", &panning.default_amount, 0, 64),
prop_panning_pitch_separation("Pitch/Pan Sep.", &panning.pitch_separation,
			      -32, 32),
prop_panning_pitch_center("Pitch/Pan Center", &panning.pitch_center, 0,
			  119),
prop_panning_random_variation("Random Send", &panning.random_variation, 0,
			      64),
pitch_envelope_use_as_filter("Use As Filter", &pitch.use_as_filter),
pitch_use_default_cutoff("Use Default Cutoff", &pitch.use_default_cutoff),
pitch_use_default_Q("Use Default Q", &pitch.use_default_Q),
pitch_default_cutoff("Default Cutoff", &pitch.default_cutoff, 0, 127),
pitch_default_Q("Default Q", &pitch.default_Q, 0, 127),
pitch_filter_type("Type", &pitch.filter_type, filter_types, 5)
{

    reset();
}

const
Instrument &
Instrument::operator = (const Instrument & p_rval) {

    name = p_rval.name;
    filename = p_rval.filename;
    buffer = p_rval.buffer;

    for (int i = 0; i < MAX_LAYERS; i++) {

	layers[i] = p_rval.layers[i];
    }

    return *this;
}

void
Instrument::reset()
{

    name = "";
    filename = "";
    buffer.output_index = 0;

    for (int i = 0; i < MAX_LAYERS; i++) {

	layers[i].reset();
    }
}
Instrument::Layer * Instrument::get_layer(int p_index)
{
	assert(p_index >= 0 && p_index < MAX_LAYERS);
    	return &layers[p_index];
}

Instrument::Instrument():
prop_name("Name", &name, 25),
prop_filename("File Name", &filename),
prop_buffer("Channel#", &buffer.output_index, 0, 15)
{
    buffer.output_index = 0;

}


Instrument::~Instrument()
{
}
