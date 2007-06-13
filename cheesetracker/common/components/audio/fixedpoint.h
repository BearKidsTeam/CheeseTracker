/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://www.geocities.com/godlessinfidelus/
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
    copyright            : � 2007 by Jeremy Phelps
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


/* What follows is an explanation of fixed-point math, as it is used
 * in this program.

 * A "fixed-point" number is an integer that represents a non-integer
 * number. Unlike floating-point numbers, which contain a "mantissa"
 * that indicates where the decimal point is at the moment, fixed-point
 * numbers have a set number of bits after the decimal point.

 * Any integer can be converted into a fixed-point number with a simple
 * bit shift:
 *
 *     int my_fixedpoint = my_int << FIXED_POINT_INT_PART_BITS
 *
 * Conversely, a fixedpoint number can be converted back to a
 * normal integer with the opposite bit shift:
 *
 *    int my_int = my_fixedpoint >> FIXED_POINT_INT_PART_BITS
 *
 * We define two convenient macros for performing these operations:
 *
 *    int my_fixedpoint = INT_TO_FIXED(my_int);
 *
 *    int my_int = FIXED_TO_INT(my_fixedpoint);
 *
 * Also defined are two macros that convert a variable in-place:
 *
 *    int xx = SOMETHING;
 *
 *    IP_INT_TO_FIXED(xx);
 *    IP_FIXED_TO_INT(xx);
 *
 * The arguments to the IP_ prefixed macros must be lvalues.
 *
 * Doing math on fixed-point numbers is tricky because bit-shifting
 * by FIXEDPOINT_INT_PART_BITS is equivalent to multiplying by (1 <<
 * FIXDPOINT_INT_PART_BITS).  As a result, sometimes the result of an
 * operation involving fixed-point numbers will be a normal integer.
 *
 * For example, let F equal (1 << FIXEDPOINT_INT_PART_BITS):
 *
 *   int result = 20*F / 10*F;
 *
 * In the above case, 'F' cancels of the division, so the result is a
 * normal integer.  Therefore, if you want to divide the fixed-point
 * number 20 by 10 and have the result be another fixed-point number,
 * you should divide by a normal integer:
 *
 *   int result = 20*F / 10;
 *
 * In that case, the result is the fixed-point value.
 *
 * There are also operations for which the result is not likely to be
 * useful. Consider this:
 *
 *   int result = 10*F * 2*F;
 *
 * The result is 20*F*F, which is neither fixed-point nor integer.
 * Therefore, the result is meaningless. 
 *
 * The following list shows all possible ways in which integers and
 * fixed-point numbers can be used with the four basic math operators,
 * and whether the results will be fixed-point, integer, or meaningless:
 * 
 * fixed / fixed 	= int
 * fixed / int		= fixed
 * int / fixed		= meaningless
 *
 * fixed * fixed	= meaningless
 * fixed * int		= fixed
 *
 * fixed + int		= meaningless
 * fixed + fixed	= fixed
 *
 * fixed - int		= meaningless
 * int - fixed		= meaningless
 * fixed - fixed	= fixed
 */

#define FIXEDPOINT_INT_PART_BITS 11
#define FIXED_TO_INT(ff) (ff >> FIXEDPOINT_INT_PART_BITS)
#define INT_TO_FIXED(ii) (ii << FIXEDPOINT_INT_PART_BITS)
#define IP_FIXED_TO_INT(ff) ff >>= FIXEDPOINT_INT_PART_BITS
#define IP_INT_TO_FIXED(ii) ii <<= FIXEDPOINT_INT_PART_BITS
#define FRACTIONAL_MASK ((1 << FIXEDPOINT_INT_PART_BITS)-1)
#define FIXEDPOINT_FLOOR(number) (number & (~0 << FIXEDPOINT_INT_PART_BITS))
#define COSINE_LEN (1 << FIXEDPOINT_INT_PART_BITS)


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

		// Though one would na�vely expect
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
