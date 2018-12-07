//
// C++ Implementation: ladspa_effect_source
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ladspa_effect_source.h"
#include "ladspa_effect.h"



#ifdef LADSPA_ENABLED
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>

#include<algorithm>

int LADSPA_Effect_Source::get_effect_count() {

	return plugins.size();
}

string LADSPA_Effect_Source::get_effect_identifier_name(int p_which) {

        if ((p_which<0) || (p_which>=(int)plugins.size()))
		return "";

	return plugins[p_which].unique_name;
}

string LADSPA_Effect_Source::get_effect_display_name(int p_which) {

        if ((p_which<0) || (p_which>=(int)plugins.size()))
		return "";

	return plugins[p_which].displayed_name + (plugins[p_which].stereo?" (STEREO)":"");
}

Effect *LADSPA_Effect_Source::instantiate_effect(int p_which,Effect::Parameters *p_params) {

        if ((p_which<0) || (p_which>=(int)plugins.size()))
		return NULL;


	if (plugins[p_which].unique_name!=p_params->get_identifier_name()) {
		ERROR("Plugin parameters dont match effect!");
		return NULL;
	}
	
	return new LADSPA_Effect(plugins[p_which].filename,plugins[p_which].subplugin_index,p_params);


}

Effect::Parameters *LADSPA_Effect_Source::instantiate_effect_parameters(int p_which) {
	
        if ((p_which<0) || (p_which>=(int)plugins.size()))
		return NULL;
	
	return 	LADSPA_Effect::create_parameters( plugins[p_which].filename,plugins[p_which].subplugin_index );
	
}

string LADSPA_Effect_Source::get_name() {

	return "LADSPA";

}


void LADSPA_Effect_Source::scan_plugins(string p_dir) {

	DIR *dir;
	struct dirent *dirent;

	dir= opendir(p_dir.c_str());
	if (dir == NULL) return;

	while ((dirent= readdir(dir))) {

		void *library = NULL;
		char lib_name[PATH_MAX];
		snprintf(lib_name, PATH_MAX, "%s/%s", p_dir.c_str(), dirent->d_name);
		library = dlopen(lib_name, RTLD_LAZY);
		if (library == NULL)
			continue;

		LADSPA_Descriptor_Function descriptor_fn;
		descriptor_fn = (LADSPA_Descriptor_Function)dlsym(library, "ladspa_descriptor");
		if (descriptor_fn == NULL)
			descriptor_fn = (LADSPA_Descriptor_Function)dlsym(library, "_ladspa_descriptor");

		if (descriptor_fn == NULL) {
			dlclose(library);
			continue;
		}

		for (int k= 0;; ++k) {
			const LADSPA_Descriptor *descriptor;
			descriptor= descriptor_fn(k);
			if (descriptor == NULL) {
				break;
			}

			PluginData plugin_data;
			plugin_data.filename=lib_name;
			plugin_data.displayed_name=descriptor->Name;
			plugin_data.subplugin_index=k;
			plugin_data.unique_name="LADSPA ID: " + (string)itostr(descriptor->UniqueID);

			int out_count=0,in_count=0;
			for (int i=0; i < (int)descriptor->PortCount; i++) {

				if (LADSPA_IS_PORT_AUDIO(descriptor->PortDescriptors[i])) {

					if (LADSPA_IS_PORT_INPUT(descriptor->PortDescriptors[i]))
						in_count++;
					if (LADSPA_IS_PORT_OUTPUT(descriptor->PortDescriptors[i]))
						out_count++;
				} else if (LADSPA_IS_PORT_CONTROL(descriptor->PortDescriptors[i])) {
				}
			}

			if (in_count >= 2 && out_count >= 2)
				plugin_data.stereo= true;
			else
				plugin_data.stereo= false;

			plugins.push_back(plugin_data);
		}
	    	dlclose(library);
	}

	closedir(dir);
}

struct Sorter {

	bool operator()(const LADSPA_Effect_Source::PluginData&p_l, const LADSPA_Effect_Source::PluginData& p_r ) {

		return (p_r.displayed_name>p_l.displayed_name);
	}
};

LADSPA_Effect_Source::LADSPA_Effect_Source() {

	char *ladspa_path;

	ladspa_path= getenv("LADSPA_PATH");

	if (!ladspa_path) {
		scan_plugins("/usr/lib/ladspa");
		scan_plugins("/usr/local/lib/ladspa");
		scan_plugins("/opt/ladspa/lib");
	} else {
		scan_plugins(ladspa_path);
	}

#ifdef MACOSX_ENABLED
	scan_plugins("../Resources/ladspa");
	scan_plugins("~/Library/Application Support/ladspa");
	scan_plugins("/Library/Application Support/ladspa");
	scan_plugins("/System/Library/Application Support/ladspa");
#endif

	Sorter sorter;
	std::sort(plugins.begin(),plugins.end(),sorter);
}




LADSPA_Effect_Source::~LADSPA_Effect_Source() {


}




#endif //ladpsa enabled
