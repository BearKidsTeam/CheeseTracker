//
// C++ Interface: ladspa_effect
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LADSPA_EFFECT_H
#define LADSPA_EFFECT_H


#ifdef LADSPA_ENABLED


#include "common/plugins/effects/ladspa/ladspa.h"
#include "components/audio/effect.h"
#include <cstring>
#include <vector>
	using std::vector;

/**
@author Juan Linietsky
*/
class LADSPA_Effect : public Effect {

	enum {
		MAX_SAMPLES=8192
	};


	float buffer_l_src[MAX_SAMPLES];
	float buffer_r_src[MAX_SAMPLES];
	float buffer_l_dst[MAX_SAMPLES];
	float buffer_r_dst[MAX_SAMPLES];
	float buffer_thrash[MAX_SAMPLES]; //seems this is needed?
	float buffer_zero[MAX_SAMPLES]; //seems this is needed?


	class LADSPA_Effect_Parameters : public Parameters {
	friend class LADSPA_Effect;

		bool working;	
		string name;
		string identifier;
	
		struct Property {	
			
			enum Type {
				TYPE_INT,
				TYPE_BOOL,
				TYPE_FLOAT
			};
			
			Type type;
					
			struct ParamControl {
				float float_control;
				int int_control;
				bool bool_control;
			} bridged;
			
			Property_Bridge *bridge;
			
			Property() {
				type=TYPE_FLOAT;
				bridge=NULL;
				memset(&bridged,0,sizeof(ParamControl));				
			}
		};
		
		vector<Property*> properties;		
		
		LADSPA_Effect_Parameters();
		LADSPA_Effect_Parameters(string p_filename,int p_index);
	public:
		
		list<Property_Bridge*> get_property_list();
		string get_display_name();
		string get_identifier_name();				
		~LADSPA_Effect_Parameters();
	};

	struct Property {

		float ladspa_control;

		Property() {

			ladspa_control=0;
		}
	};

	
	LADSPA_Effect_Parameters *parameters;
	
	vector<Property> properties;

        bool working;

	const LADSPA_Descriptor *descriptor;

	bool stereo;
	LADSPA_Handle handle;
	LADSPA_Handle handle2;

	string display_name;
	string name;

	float mixing_freq;
	void *library;

	void create(string p_filename,int p_index,LADSPA_Effect_Parameters *p_parameters);
	
public:

  	void set_sampling_rate(int p_rate);
	void reset();
        void process_buffer(sample_t* p_buffer,int p_length);
	
	static Effect::Parameters* create_parameters(string p_filename,int p_index);

	LADSPA_Effect(string p_filename,int p_index,Effect::Parameters *p_parameters); //from which file, which of the plugins
	~LADSPA_Effect();

};

#endif //ladspa enabled

#endif
