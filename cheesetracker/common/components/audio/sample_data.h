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

#define SAMPLE_INT_T_TO_FLOAT(x) ((float)(x)/(float)SAMPLE_INT_T_MAX)
#define FLOAT_TO_SAMPLE_INT_T(x) ((float)SAMPLE_INT_T_MAX*x)

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

	// Interleaved sample data. Each "frame" of the sample
	// contains {channels} samples.

	sample_int_t *data_ptr;

	void sd_realloc(size_t new_size);
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
	
	// The use_fixedpoint() function overrides this
	// behavior, causing the position stay put unless
	// fixedpoint_move_cursor() is explicitly called.

	size_t num_channels() const;
	void set_num_channels(size_t num);
	size_t get_current_pos() const;

	// Several methods of getting and setting
	// data from the sample buffer are supported.
	//
	// The ones that return const pointers are returning
	// pointers into the actual sample buffer.
	//
	// All the functions deal with buffers of sample
	// data that contain one sample_int_t for each
	// channel.

        const sample_int_t *get_int_sample();
        void put_sample(const sample_int_t *smp);

	void get_f_sample(float *dest);
	void put_f_sample(const float *p_val);

	const sample_int_t *get_data_value(size_t p_pos);
	void put_data_value(size_t p_pos, const sample_int_t *p_val);

        size_t get_sample_array(sample_int_t *dest, size_t len);
        void put_sample_array(const sample_int_t *src, size_t len);

	void get_sample(size_t p_index, float *dest)  const;
	void set_sample(size_t p_idx, const float *p_val);

	void set_size(size_t new_size);
	inline size_t get_size()  const;

        void seek(size_t new_pos);
	void truncate();
        bool eof_reached();
	bool is_empty();

	void get_sample_for_cosine_mixer(float *dest, bool use_cosine_mode);
	void do_cubic_mixer_voodoo(float *dest);
	void get_sample_for_linear_mixer(float *dest);

	void use_fixedpoint(bool yes_or_no);
	void fixedpoint_aboutface();
	void fixedpoint_set_resample_rate(size_t current_freq, size_t mix_freq, bool backwards=false);
	bool fixedpoint_is_backwards();
	void fixedpoint_move_cursor();

	const Sample_Data& operator=(const Sample_Data &r_data);


	inline bool is_16bit()  const;


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
	Sample_Data(const Sample_Data &rhs);
	~Sample_Data();
};


inline int Sample_Data::get_c5_freq()  const{

	return c5_freq;
}

inline size_t Sample_Data::get_size()   const{

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
