//
// C++ Interface: ladspa_effect_source
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LADSPA_EFFECT_SOURCE_H
#define LADSPA_EFFECT_SOURCE_H

#ifdef LADSPA_ENABLED


#include "common/plugins/effects/ladspa/ladspa.h"
#include "components/audio/effect_source.h"
#include <vector>
	using std::vector;


/**
@author Juan Linietsky
*/
class LADSPA_Effect_Source : public Effect_Source {

friend class Sorter;

        struct PluginData {

		string filename;
		string unique_name;
		string displayed_name;
		int subplugin_index;
		bool stereo;
	};

	void scan_plugins(string p_dir);


	vector<PluginData> plugins;

public:

	int get_effect_count();
	string get_effect_identifier_name(int p_which);
	string get_effect_display_name(int p_which);

	virtual Effect *instantiate_effect(int p_which,Effect::Parameters *p_params); //null params means use internal
	virtual Effect::Parameters *instantiate_effect_parameters(int p_which);
	
	string get_name();

	LADSPA_Effect_Source();
	virtual ~LADSPA_Effect_Source();

};

#endif //ladspa enabled

#endif
