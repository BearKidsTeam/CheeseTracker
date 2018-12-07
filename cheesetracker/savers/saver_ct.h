//
// C++ Interface: saver_ct
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SAVER_CT_H
#define SAVER_CT_H

#include "trackercore/saver.h"
#include "components/audio/dds_helpers.h"

/**
@author Juan Linietsky
*/
class Saver_CT : public Saver {

	void get_dds_from_sample(Sample *p_sample,DDS *p_dds);
	void get_dds_from_instrument(Instrument *p_sample,DDS *p_dds,bool p_include_samples=false);
	void get_dds_from_pattern(Pattern *p_pattern,DDS *p_dds);
public:

	int save(const char *p_filename);
	int save_sample(const char *p_filename,int p_sample_index);
	int save_instrument(const char *p_filename,int p_instrument_index);

	~Saver_CT();

};

#endif
