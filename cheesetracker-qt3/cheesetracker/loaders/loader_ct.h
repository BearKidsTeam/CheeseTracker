//
// C++ Interface: loader_ct
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LOADER_CT_H
#define LOADER_CT_H
#include "loader.h"
#include "components/audio/dds_helpers.h"
/**
@author Juan Linietsky
*/
class Loader_CT : public Loader {

	DDS data;
	void set_dds_to_sample(DDS *p_dds,Sample *p_sample);
	void set_dds_to_instrument(DDS *p_dds,Instrument *p_inst);
	void set_dds_to_pattern(DDS *p_dds,Pattern *p_pattern);

public:

	Error load(const char *p_filename,bool p_load_patterns);
	bool test(const char *p_filename);


	/* Compute Sample Info */
	int get_amount_of_samples();
	Sample_Data *get_sample_data(int p_sample_index);
	string get_sample_name(int p_sample_index);
	void add_sample_to_song(int p_sample_index,int p_dest_index,bool create_instrument);

	Error load_sample(const char *p_filename,int p_dest_index);
	Sample_Data *load_sample(const char *p_filename);
        Error load_samples_from_instrument(const char *p_filename);

	/* Compute Instrument Info */
	Error load_instrument(const char *p_filename,int p_dest_index);

	void transfer_data_to_song();

	void free_info(bool free_sampledata=false);
	Loader_CT();
	~Loader_CT();

};

#endif
