//
// C++ Implementation: ladspa_effect
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ladspa_effect.h"

#ifdef LADSPA_ENABLED
#include "typedefs.h"

#include <cstring>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>



/************ PARAMETERS ********************************/


LADSPA_Effect::LADSPA_Effect_Parameters::LADSPA_Effect_Parameters() {

	working=false;

}

LADSPA_Effect::LADSPA_Effect_Parameters::LADSPA_Effect_Parameters(string p_filename,int p_index) {
	
	working=false;
	void *library;
	const LADSPA_Descriptor *descriptor;
//	LADSPA_Handle handle;
	
		
	library=NULL;
	library = dlopen(p_filename.c_str(), RTLD_LAZY);
	
	if (library == NULL) {

		ERROR("Cant open LADSPA plugin at: " << p_filename);
		return;
	}

	LADSPA_Descriptor_Function descriptor_fn;
	descriptor_fn = (LADSPA_Descriptor_Function)dlsym(library, "ladspa_descriptor");
	if (descriptor_fn == NULL)
		descriptor_fn = (LADSPA_Descriptor_Function)dlsym(library, "_ladspa_descriptor"); //little hax0r

	if (descriptor_fn == NULL) {
		dlclose(library);
		ERROR("INVALID LADSPA Plugin at: " << p_filename);
		return;
	}

	descriptor= descriptor_fn(p_index);
	if (descriptor == NULL) {
		dlclose(library);
		ERROR("Wrong index of subplugin for LADSPA: " << p_index);
		return;
	}


	name=descriptor->Name;
	identifier="LADSPA ID: " + (string)itostr(descriptor->UniqueID);

	printf("opening: %s\n",name.c_str());

	for (int i=0; i < (int)descriptor->PortCount; i++) {

		const LADSPA_PortRangeHint *hints = descriptor->PortRangeHints;

		if (LADSPA_IS_PORT_CONTROL(descriptor->PortDescriptors[i])) {

			if (LADSPA_IS_PORT_OUTPUT(descriptor->PortDescriptors[i]))
				continue; // I gots nothing to show
			bool gay_sample_rate=false;
			if (LADSPA_IS_HINT_SAMPLE_RATE(hints[i].HintDescriptor))
				gay_sample_rate=true;

			string port_name=descriptor->PortNames[i];
			string port_var_name="descr_" + (string)itostr(i);
			float min,max,def;
			bool is_int=false;
			printf("parameter %s\n",port_name.c_str());

			Property * new_prop= new Property;

			if (LADSPA_IS_HINT_TOGGLED(hints[i].HintDescriptor)) {

				new_prop->type=Property::TYPE_BOOL;
				new_prop->bridge = new Bool_Property_Bridge(port_name,&new_prop->bridged.bool_control,port_var_name);
				properties.push_back(new_prop);

				continue;
			}

			if (LADSPA_IS_HINT_BOUNDED_BELOW(hints[i].HintDescriptor)) {
				min= hints[i].LowerBound;
			} else {
				min= -9999.0f;
			}

			if (LADSPA_IS_HINT_BOUNDED_ABOVE(hints[i].HintDescriptor)) {
				max= hints[i].UpperBound;
			} else {
				max= 9999.0f;
			}



			/* infinity */

			if (LADSPA_IS_HINT_INTEGER(hints[i].HintDescriptor)) {
                        	is_int=true;
			}

			if (LADSPA_IS_HINT_DEFAULT_MINIMUM(hints[i].HintDescriptor)) {

				def=min;
			} else if (LADSPA_IS_HINT_DEFAULT_LOW(hints[i].HintDescriptor)) {

				def=min + (max-min) * 0.25;
			} else if (LADSPA_IS_HINT_DEFAULT_HIGH(hints[i].HintDescriptor)) {

				def=min + (max-min) * 0.75;
			} else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(hints[i].HintDescriptor)) {

				def=max;
			} else if (LADSPA_IS_HINT_DEFAULT_0(hints[i].HintDescriptor)) {

				def=0.0f;
			} else if (LADSPA_IS_HINT_DEFAULT_1(hints[i].HintDescriptor)) {

				def=1.0f;
			} else if (LADSPA_IS_HINT_DEFAULT_100(hints[i].HintDescriptor)) {

				def=100.0f;
			} else if (LADSPA_IS_HINT_DEFAULT_440(hints[i].HintDescriptor)) {

				def=440.0f;
			} else if (LADSPA_IS_HINT_INTEGER(hints[i].HintDescriptor)) {

				def=min;
			} else if (max >= 0.0f && min <= 0.0f) {

				def = 0.0f;
			} else { //middle by default...
				if (min<0.0f)
                                	def=0.0f;
				else
					def=min + (max-min) * 0.5;
			}

			if (gay_sample_rate) {

				def*=44100;
				min*=44100;
				max*=44100; //as gay as it can be
			}
			if (is_int) {

				new_prop->type=Property::TYPE_INT;
				new_prop->bridge = new Int_Property_Bridge(port_name,&new_prop->bridged.int_control,(int)min,(int)max,port_var_name);
				new_prop->bridged.int_control=(int)def;

			} else {

				new_prop->type=Property::TYPE_FLOAT;
				new_prop->bridge = new Float_Property_Bridge(port_name,&new_prop->bridged.float_control,min,max,port_var_name);
				new_prop->bridged.float_control=(int)def;
			}

			properties.push_back(new_prop);
		}
	}

	working=true;	
	dlclose(library);
	
}


list<Property_Bridge*> LADSPA_Effect::LADSPA_Effect_Parameters::get_property_list() {
	
	list<Property_Bridge*> plist;

	for (int i=0;i<(int)properties.size();i++) {

		plist.push_back(properties[i]->bridge);
	}

	return plist;
	
	
}

string LADSPA_Effect::LADSPA_Effect_Parameters::get_display_name() {
	
	return name;	
	
}
string LADSPA_Effect::LADSPA_Effect_Parameters::get_identifier_name() {
	
	
	return identifier;	
}


LADSPA_Effect::LADSPA_Effect_Parameters::~LADSPA_Effect_Parameters() {

	for (int i=0;i<(int)properties.size();i++) {

		delete properties[i]->bridge;
		delete properties[i];
	}
}
		
/***************** EFFECT *****************************/		
		
void LADSPA_Effect::reset() {

	printf("RESET called\n");

	if (!working)
		return;

	/* Clear exiting handles (instances) */
	if (handle) {
		if (descriptor->deactivate) {
			descriptor->deactivate(handle);
		}
		descriptor->cleanup(handle);
		handle = NULL;
	}

	if (handle2) {
		if (descriptor->deactivate) {
			descriptor->deactivate(handle2);
		}
		descriptor->cleanup(handle2);
		handle2 = NULL;
	}

	printf("Creating instances\n");

	/* Create new instances */
	handle = descriptor->instantiate(descriptor,(int)mixing_freq);

	/* If plugin is mono, create one for left and one for right */
	if (!stereo) {

		handle2 = descriptor->instantiate(descriptor,(int)mixing_freq);
	}

	int in_count=0,out_count=0,cont_count=0;
	printf("Creating links\n");
	/* If you dont connect all the input/outputs to something, the plugin will CRASH!!!" */

	for (int i=0;i < (int)descriptor->PortCount;i++) {

		if (LADSPA_IS_PORT_CONTROL(descriptor->PortDescriptors[i])) {
//			const LADSPA_PortRangeHint *hints = descriptor->PortRangeHints;
			if (LADSPA_IS_PORT_OUTPUT(descriptor->PortDescriptors[i])) {
				printf("Skpping output..\n");
				descriptor->connect_port(handle, i, buffer_thrash);
				if (handle2)
					descriptor->connect_port(handle2, i, buffer_thrash);
				continue; // I gots nothing to show
			} /*else if (LADSPA_IS_HINT_SAMPLE_RATE(hints[i].HintDescriptor)) {
				descriptor->connect_port(handle, i, &mixing_freq);
				if (handle2)
					descriptor->connect_port(handle2, i, &mixing_freq);
				printf("Skipping sample rate\n");
				continue; // I will not control the sample rate, sorry
			}       */

			descriptor->connect_port(handle, i, &properties[cont_count].ladspa_control);
			if (handle2)
				descriptor->connect_port(handle2, i, &properties[cont_count].ladspa_control);

			cont_count++;

		} else if (LADSPA_IS_PORT_AUDIO(descriptor->PortDescriptors[i])) {

			if (LADSPA_IS_PORT_INPUT(descriptor->PortDescriptors[i])) {
				if (in_count == 0) {

					descriptor->connect_port(handle, i, buffer_l_src);
					if (!stereo)
						descriptor->connect_port(handle2, i, buffer_r_src);

				} else if (in_count==1) {

					if (stereo) {
						descriptor->connect_port(handle, i, buffer_r_src);
					} else {
						descriptor->connect_port(handle, i, buffer_zero);
						descriptor->connect_port(handle2, i, buffer_zero);

					}

				} else {
					descriptor->connect_port(handle, i, buffer_zero);
					if (!stereo)
						descriptor->connect_port(handle2, i, buffer_zero);

                        	}
				in_count++;

			} else if (LADSPA_IS_PORT_OUTPUT(descriptor->PortDescriptors[i])) {

				if (out_count == 0) {

					descriptor->connect_port(handle, i, buffer_l_dst);
					if (!stereo)
						descriptor->connect_port(handle2, i, buffer_r_dst);

				} else if (out_count==1) {

					if (stereo) {
						descriptor->connect_port(handle, i, buffer_r_dst);
					} else {
						descriptor->connect_port(handle, i, buffer_thrash);
						descriptor->connect_port(handle2, i, buffer_thrash);
					}
				} else {
					descriptor->connect_port(handle, i, buffer_thrash);
					if (!stereo)
						descriptor->connect_port(handle2, i, buffer_thrash);

                        	}
				out_count++;

			}
		}
	}

	//clear buffers just in case!
        memset(buffer_l_src,0,sizeof(float)*MAX_SAMPLES);
        memset(buffer_l_dst,0,sizeof(float)*MAX_SAMPLES);
        memset(buffer_r_src,0,sizeof(float)*MAX_SAMPLES);
        memset(buffer_r_dst,0,sizeof(float)*MAX_SAMPLES);
        memset(buffer_r_src,0,sizeof(float)*MAX_SAMPLES);
        memset(buffer_thrash,0,sizeof(float)*MAX_SAMPLES);
	printf("Activating plugin\n");

	/* Finally, activate them */
	if (descriptor->activate) {
		descriptor->activate(handle);
		if (handle2)
			descriptor->activate(handle2);
	}
}

void LADSPA_Effect::set_sampling_rate(int p_rate) {
	
	if (((float)p_rate!=mixing_freq))  {
		mixing_freq=(float)p_rate;
		reset(); //@TODO take this out from here!!
	}
	
}

void LADSPA_Effect::process_buffer(sample_t* p_buffer,int p_length) {

	if (!working)
		return;

	while (p_length>MAX_SAMPLES) {

		process_buffer(p_buffer,MAX_SAMPLES);
		p_length-=MAX_SAMPLES;
		p_buffer+=MAX_SAMPLES*2;
	}

	if (p_length==0)
		return; //noting to do

	for (int i=0;i<(int)properties.size();i++) {

		switch(parameters->properties[i]->type) {
                        case LADSPA_Effect_Parameters::Property::TYPE_INT: {
                       		properties[i].ladspa_control=(float)parameters->properties[i]->bridged.int_control;
			} break;
                        case LADSPA_Effect_Parameters::Property::TYPE_BOOL: {
                       		properties[i].ladspa_control=parameters->properties[i]->bridged.bool_control?1.0f:0.0f;
			} break;
                        case LADSPA_Effect_Parameters::Property::TYPE_FLOAT: {
                       		properties[i].ladspa_control=parameters->properties[i]->bridged.float_control;
			} break;
		}
	}

	sample_t *in_ptr=p_buffer;

	for (int i=0;i<p_length;i++) {

		buffer_l_src[i]=*(in_ptr++);
		buffer_r_src[i]=*(in_ptr++);
	}

	descriptor->run(handle, p_length);
	
	if (handle2)
		descriptor->run(handle2, p_length);

	sample_t *out_ptr=p_buffer;

	for (int i=0;i<p_length;i++) {
		*out_ptr++ = buffer_l_dst[i];
		*out_ptr++ = buffer_r_dst[i];
	}

}




Effect::Parameters* LADSPA_Effect::create_parameters(string p_filename,int p_index) {
	
	return new LADSPA_Effect_Parameters(p_filename,p_index);
}



void LADSPA_Effect::create(string p_filename,int p_index,LADSPA_Effect_Parameters *p_parameters) {

	library=NULL;
	working=false;
	library = dlopen(p_filename.c_str(), RTLD_LAZY);
	if (library == NULL) {

		ERROR("Cant open LADSPA plugin at: " << p_filename);
		return;
	}

	LADSPA_Descriptor_Function descriptor_fn;
	descriptor_fn = (LADSPA_Descriptor_Function)dlsym(library, "ladspa_descriptor");
	if (descriptor_fn == NULL)
		descriptor_fn = (LADSPA_Descriptor_Function)dlsym(library, "_ladspa_descriptor"); //little hax0r

	if (descriptor_fn == NULL) {
		dlclose(library);
		ERROR("INVALID LADSPA Plugin at: " << p_filename);
		return;
	}

	descriptor= descriptor_fn(p_index);
	if (descriptor == NULL) {
		dlclose(library);
		ERROR("Wrong index of subplugin for LADSPA: " << p_index);
		return;
	}


	display_name=descriptor->Name;
	name="LADSPA ID: " + (string)itostr(descriptor->UniqueID);

	printf("opening: %s\n",name.c_str());
	int out_count=0,in_count=0;
	for (int i=0; i < (int)descriptor->PortCount; i++) {

		if (LADSPA_IS_PORT_AUDIO(descriptor->PortDescriptors[i])) {

			if (LADSPA_IS_PORT_INPUT(descriptor->PortDescriptors[i]))
				in_count++;
			if (LADSPA_IS_PORT_OUTPUT(descriptor->PortDescriptors[i]))
				out_count++;
		} 
	}
	printf("Plugin has %i inputs, %i outputs\n",in_count,out_count);

	if (in_count >= 2 && out_count >= 2)
		stereo= true;
	else
		stereo= false;

	handle=NULL;
	handle2=NULL;


	working=true;

	mixing_freq=44100;
	
	properties.resize(parameters->properties.size());
	
	reset();

}

LADSPA_Effect::LADSPA_Effect(string p_filename,int p_index,Effect::Parameters *p_parameters) : Effect(p_parameters) {
	
	parameters = dynamic_cast<LADSPA_Effect_Parameters*>( get_parameters() );
	if (!parameters) {
		ERROR("params is null?? is something very wrong?");
		return;
	}
	create(p_filename,p_index,parameters);
	
}


LADSPA_Effect::~LADSPA_Effect(){

	if (!working)
		return;

	
	if (handle) {
		if (descriptor->deactivate) {
			descriptor->deactivate(handle);
		}
		descriptor->cleanup(handle);
		handle = NULL;
	}

	if (handle2) {
		if (descriptor->deactivate) {
			descriptor->deactivate(handle2);
		}
		descriptor->cleanup(handle2);
		handle2 = NULL;
	}

	dlclose(library);


	properties.clear();
}

#endif
