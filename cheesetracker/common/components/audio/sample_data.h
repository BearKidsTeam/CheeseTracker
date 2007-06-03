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
                          sample_data.h  -  description
                             -------------------
    begin                : Mon Feb 5 2001
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

#ifndef SAMPLE_DATA_H
#define SAMPLE_DATA_H

// #include "../../../cheesetracker/trackercore/Error.h"
#include "cheesetracker/trackercore/Error.h"
#include "typedefs.h"
#include "sample_defs.h"
#include "tables.h"
//#include "tables.h"

/**Sample Data.
All data needed to modify a Sample

  *@author Juan Linietsky
  */


/******************************
 sample_data.h
 --------

This contains the real "data"
of a sample. It is used both by the
song and the player.

Also contains some useful editing functions.
that will make your life easier
********************************/


#define IS_16_BIT true

#define SAMPLE_INT_T_TO_FLOAT(x) ((float)(x)/SAMPLE_INT_T_MAX)

DERIVE_EMPTY(Error, Sample_Error);
DERIVE_EMPTY(Sample_Error, Sample_EOF_Error);
DERIVE_EMPTY(Sample_Error, Sample_Readonly_Error);

class Sample_Data {


	/* loop flags */

	bool loop_on;
	bool sustain_loop_on;

	/* loop data */
	size_t loop_begin;
	size_t loop_end;
	bool pingpong_loop;

	/* sustain loop data */
	size_t sustain_loop_begin;
	size_t sustain_loop_end;
	bool sustain_pingpong_loop;

	/* format Data */
	bool is_16bits;

	int c5_freq;

	// Memory management variables
	//
	// fixedpoint_mode and friends are used during resampling.
	// During resampling, it is often necessary to move the
	// current_pos by a fraction. Integer math is preferred
	// over floating-point, so when fixedpoint_mode is false,
	// the fractional incrementation scheme is ignored (as
	// is the ability to move backwards).
	//
	// While in fixedpoint_mode, the Sample_Data is in read-only
	// mode. All attempts to put data in a fixedpoint_mode Sample_Data
	// will cause an exception.

	bool fixedpoint_mode;

	// The number of bytes to add to the current position when
	// fixedpoint_move_cursor() is called.
	size_t fixedpoint_inc;

	// The fixed-point offset from the previous whole index into the
	// sample buffer.
	size_t fixedpoint_offset;

	bool fixedpoint_backwards;

	// The position indicator, used both in fixed-point and normal mode.
	size_t current_pos;

	size_t size; // in samples, not bytes.
	size_t channels;
	sample_int_t *data_ptr;
	void sd_realloc(size_t chan, size_t new_size);
	void correct_loop_pointers();

	// DEPRECATED!!!!

	inline sample_16s_t *get_data_16() const;
	inline sample_8s_t *get_data_8()  const;
	void release_data_ptr();
	void set_data_ptr(void *p_data,int p_size=0,bool p_16bits=true);

public:
	// The "paragraph" of functions below are the
	// Single Sample Data Type interface. They allow
	// the distinction between 8-bit and 16-bit samples,
	// to be ignored by the rest of the code. 

	// This shall become the only interface to the
	// Sample_Data structure, and then multiple-channel
	// samples will be implemented.

	// They present the sample data as if it was a
	// file. There is a current position, and getting
	// or putting data advances this position.

	// Putting data past the end of the "file" causes
	// the sample to become longer. 

	// Using get_sample() after the end of the sample
	// has been reached results in an exception being
	// thrown.

	// Using get_sample_array() too close to the end of
	// a sample causes a short number of samples to
	// be retrieved. The actual number of samples
	// that were stored in the output buffer is the
	// return value.
	//

	size_t num_channels() const;
	void alloc_channels(size_t num);
	size_t get_current_pos(size_t chan) const;
        sample_int_t get_int_sample(size_t chan);
	float get_f_sample(size_t chan);
	void put_f_sample(size_t chan, float p_val);
        void put_sample(size_t chan, sample_int_t smp);
	void set_size(size_t chan, size_t new_size);
        void seek(size_t chan, size_t new_pos);

	sample_int_t get_data_value(size_t chan, size_t p_pos);
	void put_data_value(size_t chan, size_t p_pos,sample_int_t p_val);

        size_t get_sample_array(size_t chan, sample_int_t *dest, size_t len);
        void put_sample_array(size_t chan, const sample_int_t *src, size_t len);
	void truncate(size_t chan);
        bool eof_reached(size_t chan);
	bool is_empty();

	float get_sample_for_cosine_mixer(size_t chan, bool use_cosine_mode);
	float do_cubic_mixer_voodoo(size_t chan);
	float get_sample_for_linear_mixer(size_t chan);

	void use_fixedpoint(bool yes_or_no);
	void fixedpoint_aboutface();
	void fixedpoint_set_resample_rate(size_t current_freq, size_t mix_freq, bool backwards=false);
	bool fixedpoint_is_backwards();
	void fixedpoint_move_cursor();

	const Sample_Data& operator=(const Sample_Data &r_data);


	inline size_t get_size(size_t chan=0)  const;
	inline bool is_16bit()  const;

	float get_sample(size_t p_index)  const;
	void set_sample(size_t p_idx,float p_val);

        void set_c5_freq(int p_c5_freq);
        inline int get_c5_freq()  const;

	void set_loop_enabled(bool p_enabled);
	inline bool is_loop_enabled()  const;

	void set_loop_ping_pong(bool p_enabled);
	inline bool is_loop_ping_pong()  const;

	inline int get_loop_begin()  const;
	inline int get_loop_end()  const;

	void set_loop_begin(int p_idx);
	void set_loop_end(int p_idx);

	void copy_sample_data(const Sample_Data *p_sdata);


	void change_sign();
	void toggle_quality();

	/*
        void reverse();
        void post_loop_cut();
        void pre_loop_cut();
	void amplify(int p_percent);
	int get_value_to_normalize();
	void resize(int p_new_size);
	void resize_interpolated(int p_new_size);
	void centralize();
          */
        //void import_frequency(int p_freq);
	//int export_frequency();

	Sample_Data();
	~Sample_Data();
};


inline int Sample_Data::get_c5_freq()  const{

	return c5_freq;
}

inline size_t Sample_Data::get_size(size_t chan)   const{

	return size;
}



inline bool Sample_Data::is_16bit()  const{

	return is_16bits;
}
inline sample_16s_t *Sample_Data::get_data_16()  const{


		return  data_ptr;
}
inline sample_8s_t *Sample_Data::get_data_8()  const{

		return  (sample_8s_t*)data_ptr;
}

inline bool Sample_Data::is_loop_enabled()  const{

	return loop_on;
}

inline bool Sample_Data::is_loop_ping_pong()  const{

	return pingpong_loop;
}

inline int Sample_Data::get_loop_begin()  const{

	return loop_begin;
}
inline int Sample_Data::get_loop_end()  const{

	return loop_end;

}

#endif
