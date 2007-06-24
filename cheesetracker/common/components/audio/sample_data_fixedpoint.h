#include "typedefs.h"
#include "fixedpoint_defs.h"

/***************************************************************************
    This file is part of CheeseTracker
    url                  : http://cheesetracker.sourceforge.net/
    copyright            : (C) 2003 by Juan Linietsky
    maintainer           : J Phelps
    email                : godless@users.sf.net
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
                          fixedpoint.h
                             -------------------
    begin                : Wed Jun 13 2007
    copyright            : © 2007 by Jeremy Phelps
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// THIS FILE SHOULD ONLY BE INCLUDED BY sample_data.cpp.

COMPILER_ASSERT(IN_SAMPLE_DATA_CPP);

// use_fixedpoint - Set whether or not to use the special fixed-point processing mode.
// 
// notes          - Fixed-point mode is a special mode required by the resamplers
//                  found in common/plugins/resamplers/. In fixed-point mode, a
//                  channel's position indicator doesn't move when the read/write
//                  interface is used. This behavior is required because the mixer
//                  may need to mix the same sample into multiple channels.
//
//                  Instead, the position indicator is moved explicitly with
//                  fixedpoint_move_cursor() when all channels at the current position
//                  have been mixed. 
//
//                - When the indicator does move, it moves according to the resample
//                  ratio calculated by fixedpoint_set_resample_rate().
//
//                - Fixed-point mode supports forward and backward cursor movement, whereas
//                  normal mode only supports forward movement.
//
// see also       - fixedpoint_set_resample_rate, fixedpoint_move_cursor, fixedpoint_aboutface,
//                  fixedpoint_is_backwards

void
Sample_Data::use_fixedpoint(bool yes_or_no)
{
	fixedpoint_mode=yes_or_no;
}

void
Sample_Data::fixedpoint_set_resample_rate(size_t current_freq, size_t mix_freq, bool backwards)
{
	fixedpoint_inc = INT_TO_FIXED(current_freq)/mix_freq;
	fixedpoint_backwards = backwards;
}

// fixedpoint_move_cursor - Move the cursor in fixed-point mode.
//
// warning                - Does not make sure that the sample is actually in fixed-point mode.
//                          If the sample is in fixed-point mode, the read/write interface
//                          will advance the cursor by one for each sample read or written.
//
// see also               - use_fixedpoint

void
Sample_Data::fixedpoint_move_cursor() {
	fixedpoint_offset += fixedpoint_inc;

	size_t int_batch = FIXED_TO_INT(fixedpoint_offset);	// Convert fixed-point to int.

	if(int_batch >= 1) {
		if(fixedpoint_backwards) {
			if(current_pos < int_batch) {
				Sample_EOF_Error E;
				E.set_error_pfx("fixedpoint_move_cursor");
				E.set_error("Beginning of sample reached while incrementing backwards");
				throw E;
			}
			current_pos -= int_batch;
		}
		else {
			current_pos += int_batch;
			if(eof_reached())
				current_pos = size-1;
		}

		// Though one would naïvely expect
		// INT_TO_FIXED(FIXED_TO_INT(fixedpoint_offset)) to
		// equal fixedpoint_offset, FIXED_TO_INT() loses the
		// data to the right of the decimal point. As a
		// result, INT_TO_FIXED(int_batch) is equal to
		// FIXEDPOINT_FLOOR(fixedpoint_offset). The
		// INT_TO_FIXED() calculation takes fewer
		// instructions and is therefore preferred.

		fixedpoint_offset -= INT_TO_FIXED(int_batch);
	}
}

void
Sample_Data::fixedpoint_aboutface() {
	fixedpoint_backwards = !fixedpoint_backwards;
}

bool
Sample_Data::fixedpoint_is_backwards() {
	return fixedpoint_backwards;
}


bool
Sample_Data::fixedpoint_loop() {
	int jump_size = FIXED_TO_INT(fixedpoint_offset + fixedpoint_inc);
	if(fixedpoint_backwards) {
		// Check if the sample will reach the loop-begin
		// on the next increment, because it will never
		// actually be at the loop beginning if the loop
		// begins at the beginning of the sample.
		if(!loop_on) {
			return (current_pos >= jump_size);
		}
		if(current_pos - jump_size <= loop_begin) {
			if (pingpong_loop) {
				fixedpoint_backwards = false;
				current_pos -= (2*(current_pos-loop_begin)-jump_size);
			} else {
				// Normal looping.
				current_pos = loop_end - (jump_size-(current_pos-loop_begin));
			}
			// Sample should continue playing, unless it reaches
			// the opposite boundary in a single jump.
			return !eof_reached() && current_pos <= loop_end && current_pos > loop_begin;
		}
	} else {
		if(!loop_on) {
			return (current_pos+jump_size < size);
		}
		if(current_pos + jump_size >= loop_end) {
			if (pingpong_loop) {
				fixedpoint_backwards = true;
				current_pos += ((2*loop_end-current_pos)-jump_size);
			} else {
				current_pos = loop_begin + (jump_size-(loop_end-current_pos));
			}
			return !eof_reached() && current_pos < loop_end && current_pos >= loop_begin;
		}
	}
	return true;
}
