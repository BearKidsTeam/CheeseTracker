//
// C++ Interface: dds_helpers
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DDS_HELPERS_H
#define DDS_HELPERS_H


#include "components/audio/envelope.h"
#include "components/audio/sample_data.h"
#include "components/audio/mixer.h"
#include "components/data/dds.h"

/**
@author Juan Linietsky
*/

class DDS_Helpers{
public:

	static void set_property_bridge_list_data(vector_dds& p_dds_array,list<Property_Bridge*> *p_list);
	static void set_effect_chain_data(DDS* p_dds,Effect_Parameter_Chain *p_chain);
	static void set_mixer_data(DDS* p_dds,Mixer *p_mixer);
	static void set_sample_data(DDS* p_dds,Sample_Data *p_sample);
	static void set_envelope(DDS* p_dds,Envelope *p_envelope);


	static void get_property_bridge_list_data(list<Property_Bridge*> *p_list,vector_dds& p_dds_array);
	static void get_effect_chain_data(Effect_Parameter_Chain *p_chain,DDS* p_dds);
	static void get_mixer_data(Mixer *p_mixer,DDS* p_dds);
	static void get_sample_data(Sample_Data *p_sample,DDS* p_dds);
	static void get_envelope(Envelope *p_envelope,DDS* p_dds);

};

#endif
