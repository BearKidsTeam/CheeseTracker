#include <cstring>
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
                          sample_data_fixedpoint.h
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


// use_fixedpoint - Set whether or not to use the special "fixed-point"
//                  processing mode.
//
// notes          - Fixed-point mode is a special mode required by
//                  the resamplers found in common/plugins/resamplers/. In
//                  fixed-point mode, a channel's position indicator
//                  doesn't move when the read/write interface is
//                  used. This behavior is required because the mixer may
//                  need to mix the same sample into multiple channels.
//
//                  Instead, the position indicator is moved explicitly
//                  with fixedpoint_move_cursor() when all channels at
//                  the current position have been mixed.
//
//                - When the indicator does move, it moves according to
//                  the resample
//                  ratio calculated by fixedpoint_set_resample_rate().
//
//                - Fixed-point mode supports forward and backward cursor
//                  movement, whereas
//                  normal mode only supports forward movement.
//
// see also       - fixedpoint_set_resample_rate, fixedpoint_move_cursor,
//                  fixedpoint_aboutface, fixedpoint_is_backwards

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

	if(fixedpoint_backwards) {
		size_t temp_inc = fixedpoint_inc; // Destructive operations follow.
		if(fixedpoint_offset < temp_inc) {
			size_t int_temp_inc = FIXED_TO_INT(temp_inc);
			current_pos -= int_temp_inc;
			temp_inc -= INT_TO_FIXED(int_temp_inc);
			if(fixedpoint_offset < temp_inc) {
				current_pos--;
				fixedpoint_offset = INT_TO_FIXED(1) - (temp_inc - fixedpoint_offset);
			} else {
				fixedpoint_offset -= temp_inc;
			}
		} else {
			fixedpoint_offset -= fixedpoint_inc;
		}
	} else { // Incrementing forward
		fixedpoint_offset += fixedpoint_inc;
		if(fixedpoint_offset >= INT_TO_FIXED(1)) {
			current_pos += FIXED_TO_INT(fixedpoint_offset);
			fixedpoint_offset -= FIXEDPOINT_FLOOR(fixedpoint_offset);
		}
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


inline sample_int_t * sample_dup(const sample_int_t *buffer, size_t length, size_t channels) {
	sample_int_t *ret = new sample_int_t[length*channels];
	memcpy(ret, buffer, length*sizeof(sample_int_t)*channels);
	return ret;
}

// fixedpoint_loop - Prepare a sample to loop before mixing a bit of it.
//
//                   Modifies the current position indicator and the
//                   fixedpoint_backwards flag.
//
// arguments       - posinfo is allocated with create_position_info()
//                   and deallocated with destroy_position_info. It holds
//                   some state information between calls to fixedpoint_loop()
//                   in a way that can be reasonably expected to be
//                   thread-safe.
//
// return value    - Returns 'true' if the sample should continue playing,
//                   otherwise returns false.
//
// context         - Used in Tracker_Voice::add_to_mix_buffer(), in the
//                   file tracker_voice.cpp

bool
Sample_Data::fixedpoint_loop(bool sustaining)
{
	size_t jump_size = FIXED_TO_INT(fixedpoint_offset + fixedpoint_inc);
	bool loop_on_local = false;
	bool pingpong_local = false;
	size_t loop_begin_local;
	size_t loop_end_local;

	printf("LOOP: START %u\n", (size_t)current_pos);

	if(sustaining && sustain_loop_on) {
		loop_begin_local	= sustain_loop_begin;
		loop_end_local		= sustain_loop_end;
		loop_on_local 		= true;
		pingpong_local		= sustain_pingpong_loop;
	} else if(loop_on) {
		pingpong_local		= pingpong_loop;
		loop_begin_local	= loop_begin;
		loop_end_local		= loop_end;
		loop_on_local 		= true;
	}

	// current_pos + jump_size is the expected value of
	// current_pos after the next call to fixedpoint_move_cursor().
	//
	// The following if-block checks where current_pos WOULD BE
	// after the next call to fixedpoint_move_cursor(), if
	// fixedpoint_move_cursor() did not check for EOF.

	if(fixedpoint_backwards) {
		if(loop_on_local && current_pos < bigint(loop_begin_local)) {
			if(pingpong_local) {
				current_pos = loop_begin_local+(size_t)(bigint(loop_begin_local)-current_pos);
				fixedpoint_aboutface();
			} else {
				current_pos = loop_end_local - (size_t)(bigint(loop_begin_local) - current_pos);
			}
		} else {
			if(current_pos < bigint((size_t)0)) {
				current_pos = (size_t) 0;
				return false;
			}
		}
	} else { /* The sample is playing forward */
		if(loop_on_local && current_pos >= bigint(loop_end_local)) {
			/* The sample is looping */
			if(pingpong_local) {
				fixedpoint_aboutface();
				current_pos = loop_end_local - (size_t)(current_pos - loop_end_local);
			} else {
				/* The sample is not looping. */
				return (current_pos < bigint(size));
			}
		}
	}
	printf("LOOP: END %u\n", (size_t)current_pos);
	return true;
}

size_t
Sample_Data::get_fixedpoint_offset()
{
	return fixedpoint_offset;
}

void
Sample_Data::set_fixedpoint_offset(size_t new_offset)
{
	fixedpoint_offset=new_offset;
}
