//
// C++ Implementation: dds_helpers
//
// Description: A class that has some mysterious relationship with
//              DDS.h. Its apparent purpose is to prevent us from
//              being able to figure out how sample data is being
//              interpreted in the saver_ct and loader_ct modules,
//              to make it more difficult to make those modules support
//              wide, multi-channel samples. 
//
//              Fortunately, those two modules are the only ones
//              that use this class.
//
// BTW:		EMACS sucks. Use vi instead.
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dds_helpers.h"
#include "effect_source_manager.h"
#include "ns_autoptr.h"


void DDS_Helpers::set_property_bridge_list_data(vector_dds& p_dds_array,list<Property_Bridge*> *p_list)
{


	for (int i=0;i<p_dds_array.size();i++) {

		DDS &prop=p_dds_array[i];
         	string name=prop.get_str_var("name");
         	string type=prop.get_str_var("type");

		list<Property_Bridge*>::iterator I=p_list->begin();
		list<Property_Bridge*>::iterator E=p_list->end();
		for (;I!=E;I++) {

			if ( ((*I)->get_short_name()==name) && ((*I)->get_type()==type) )
				break;
		}

		if (I==E)
			continue; //wasnt found

		Property_Bridge *pr=(*I);

		if ( pr->get_type()=="int")  {

			Int_Property_Bridge *bridge_int = dynamic_cast<Int_Property_Bridge*>(pr);
			if (bridge_int==NULL) {
				printf("Warning: int bridge failed dynamic cast!\n");
				continue;
			}

			bridge_int->set( prop.get_int_var("value") );

		} else if ( pr->get_type()=="float")  {

			Float_Property_Bridge *bridge_float = dynamic_cast<Float_Property_Bridge*>(pr);
			if (bridge_float==NULL) {
				ERROR("Bridge is not float?");
				continue;
			}
			bridge_float->set( prop.get_float_var("value") );


		} else if ( pr->get_type()=="bool")  {

			Bool_Property_Bridge *bridge_bool = dynamic_cast<Bool_Property_Bridge*>(pr);
			if (bridge_bool==NULL)
				continue;

			bridge_bool->set( prop.get_int_var("value") );

		} else if ( pr->get_type()=="string")  {

			String_Property_Bridge *bridge_string = dynamic_cast<String_Property_Bridge*>(pr);
			if (bridge_string==NULL)
				continue;
			bridge_string->set( prop.get_str_var("value") );
		} else if ( pr->get_type()=="options")  {

			Options_Property_Bridge *bridge_options = dynamic_cast<Options_Property_Bridge*>(pr);
			if (bridge_options==NULL)
				continue;
			bridge_options->set( prop.get_int_var("value") );
		} else {
			ERROR("Cant load property, unknown type: " << pr->get_type());
			continue;
		}
	}
}

void DDS_Helpers::set_effect_chain_data(DDS* p_dds,Effect_Parameter_Chain *p_chain)
{



	vector_dds &effects=p_dds->get_dds_array_var("effects");

	for (int i=0;i<effects.size();i++) {

		DDS &effect=effects[i];
		string name = effect.get_str_var("name");
                Effect::Parameters *new_fx=Effect_Source_Manager::get_singleton_instance()->instantiate_effect_parameters_by_name(name);
		if (!new_fx) {
			ERROR("Effect '" << name << "' not found!!");
			continue;
		}
		p_chain->add_effect(new_fx);

		vector_dds &controls=effect.get_dds_array_var("controls");
		set_property_bridge_list_data(controls,&new_fx->get_property_list());
	}
}

void DDS_Helpers::set_envelope(DDS* p_dds,Envelope *p_envelope)
{

	p_envelope->reset();

	int nodes=p_dds->get_int_var("node_count");

	const vector_int &node_offset=p_dds->get_int_array_var("node_offsets");
	const vector_int &node_height=p_dds->get_int_array_var("node_heights");

	if (node_offset.size()!=nodes) {

		ERROR("Wrong node offset count in envelope");
		return;
	}
	if (node_height.size()!=nodes) {

		ERROR("Wrong node height count in envelope");
		return;
	}

	for (int i=0;i<nodes;i++) {

		p_envelope->add_node_at_offset(node_offset[i],node_height[i]);

	}

	p_envelope->set_enabled( p_dds->get_int_var("enabled") );

	p_envelope->set_loop_enabled( p_dds->get_int_var("has_loop") );

	p_envelope->set_loop_begin( p_dds->get_int_var("loop_begin") );
	p_envelope->set_loop_end( p_dds->get_int_var("loop_end") );

	p_envelope->set_sustain_loop_enabled( p_dds->get_int_var("has_sustain_loop") );

	p_envelope->set_sustain_loop_begin( p_dds->get_int_var("sustain_loop_begin") );
	p_envelope->set_sustain_loop_end( p_dds->get_int_var("sustain_loop_end") );

}

void DDS_Helpers::set_sample_data(DDS* p_dds,Sample_Data *p_sample)
{


	p_sample->set_c5_freq( p_dds->get_int_var("c5_freq") );
	p_sample->set_loop_enabled( p_dds->get_int_var("has_loop") );
	p_sample->set_loop_ping_pong( p_dds->get_int_var("has_pingpong_loop") );

	p_sample->set_loop_begin( p_dds->get_int_var("loop_begin") );
	p_sample->set_loop_end( p_dds->get_int_var("loop_end") );

	bool is16=p_dds->get_int_var("is_16bits");

	string compression=p_dds->get_str_var("compression_type");
	if (compression!="none") {

		ERROR("Unsupported compression");
		return;
	}

	Uint8 *data=(Uint8*)p_dds->get_data_var("data");

	int datasize=p_dds->get_data_var_size("data");
	Uint8 *new_data= (Uint8*)malloc(datasize);
	if (is16)
		datasize/=2;

	p_sample->set_num_channels(1);

	if (is16) {

		// This must be rewritten when sample_int_t is widened 
		// to 32 bits.

		COMPILER_ASSERT(sizeof(Uint16) == sizeof(sample_int_t));

		sample_int_t *new_data16=(sample_int_t*)new_data;
		for (size_t i=0;i<datasize;i++) {

			new_data16[i]=data[i*2];
			new_data16[i]<<=8;
			new_data16[i]|=data[i*2+1];
		}
		p_sample->put_sample_array(new_data16, datasize);
		free(new_data);

	} else {

		for(size_t ix=0; ix<datasize; ix++) {
			sample_int_t buffer = CONVERT_FROM_TYPE(Uint8, data[ix]);
			p_sample->put_sample(&buffer);
		}
	}

}

void DDS_Helpers::set_mixer_data(DDS* p_dds,Mixer *p_mixer)
{

	vector_dds& buffers=p_dds->get_dds_array_var("buffers");

	for (int i=0;i<buffers.size();i++) {

		DDS &buffer=buffers[i];
		p_mixer->add_buffer();
		p_mixer->set_buffer_name(i,buffer.get_str_var("name"));

		DDS &chain=buffer.get_dds_var("effect_chain");
		set_effect_chain_data(&chain,p_mixer->get_buffer_effect_parameter_chain(i));

		vector_dds& outputs=buffer.get_dds_array_var("outputs");

		while (p_mixer->get_buffer_output_count(i))
			p_mixer->remove_buffer_output(i,0);

		for (int j=0;j<outputs.size();j++) {
			DDS &output=outputs[j];

			p_mixer->add_buffer_output(i);

			p_mixer->set_buffer_output_buffer_index(i,j,output.get_int_var("index"));
			p_mixer->set_buffer_output_send_level(i,j,output.get_int_var("initial_send_level"));
			p_mixer->set_buffer_output_send_control(i,j,output.get_int_var("send_controller"));
           	}

	}

	vector_dds& channels=p_dds->get_dds_array_var("channels");

	for (int i=0;i<channels.size();i++) {

		DDS &channel=channels[i];
		//p_mixer->add_buffer();
		//p_mixer->set_buffer_name(i,buffer.get_str_var("name"));
		p_mixer->set_channel_name(i,channel.get_str_var("name"));

		DDS &chain=channel.get_dds_var("effect_chain");
		set_effect_chain_data(&chain,p_mixer->get_channel_effect_parameter_chain(i));

		vector_dds& outputs=channel.get_dds_array_var("outputs");

		while (p_mixer->get_channel_output_count(i))
			p_mixer->remove_channel_output(i,0);

		for (int j=0;j<outputs.size();j++) {
			DDS &output=outputs[j];

			p_mixer->add_channel_output(i);

			p_mixer->set_channel_output_buffer_index(i,j,output.get_int_var("index"));
			p_mixer->set_channel_output_send_level(i,j,output.get_int_var("initial_send_level"));
			p_mixer->set_channel_output_send_control(i,j,output.get_int_var("send_controller"));
           	}
	}
}


void DDS_Helpers::get_property_bridge_list_data(list<Property_Bridge*> *p_list,vector_dds& p_dds_array)
{

	list<Property_Bridge*>::iterator I=p_list->begin();
	list<Property_Bridge*>::iterator E=p_list->end();

	for (;I!=E;I++) {

		DDS prop;
		Property_Bridge *pr=(*I);

		prop.set_str_var("name",pr->get_short_name());
		prop.set_str_var("type",pr->get_type());

		if ( pr->get_type()=="int")  {

			Int_Property_Bridge *bridge_int = dynamic_cast<Int_Property_Bridge*>(pr);
			if (bridge_int==NULL) {
				printf("Warning: int bridge failed dynamic cast!\n");
				continue;
			}

			prop.set_int_var("value",bridge_int->get());

		} else if ( pr->get_type()=="float")  {

			Float_Property_Bridge *bridge_float = dynamic_cast<Float_Property_Bridge*>(pr);
			if (bridge_float==NULL) {
				ERROR("Bridge is not float?");
				continue;
			}
			prop.set_float_var("value",bridge_float->get());


		} else if ( pr->get_type()=="bool")  {

			Bool_Property_Bridge *bridge_bool = dynamic_cast<Bool_Property_Bridge*>(pr);
			if (bridge_bool==NULL)
				continue;

			prop.set_int_var("value",bridge_bool->get());

		} else if ( pr->get_type()=="string")  {

			String_Property_Bridge *bridge_string = dynamic_cast<String_Property_Bridge*>(pr);
			if (bridge_string==NULL)
				continue;
			prop.set_str_var("value",bridge_string->get());
		} else if ( pr->get_type()=="options")  {

			Options_Property_Bridge *bridge_options = dynamic_cast<Options_Property_Bridge*>(pr);
			if (bridge_options==NULL)
				continue;
			prop.set_int_var("value",bridge_options->get());
		} else {
			ERROR("Cant save property, unknown type: " << pr->get_type());
			continue;
		}

		p_dds_array.push_back(prop);
	}

}

void DDS_Helpers::get_effect_chain_data(Effect_Parameter_Chain *p_chain,DDS* p_dds)
{

	vector_dds aux_vector_dds;

	p_dds->set_dds_array_var("effects",aux_vector_dds);
	vector_dds &effects=p_dds->get_dds_array_var("effects");

	for (int i=0;i<p_chain->get_effect_count();i++) {

		DDS effect;
		list<Property_Bridge*> property_list = p_chain->get_effect(i)->get_property_list();
		

		effect.set_str_var("name",p_chain->get_effect(i)->get_identifier_name());
		effect.set_dds_array_var("controls",aux_vector_dds);
		get_property_bridge_list_data(&property_list,effect.get_dds_array_var("controls"));

                effects.push_back(effect);
	}
}



void DDS_Helpers::get_mixer_data(Mixer *p_mixer,DDS* p_dds)
{

	p_dds->set_float_var("preamp",1.0);

	vector_dds aux_vector_dds;

	p_dds->set_dds_array_var("channels",aux_vector_dds);
	vector_dds& channels=p_dds->get_dds_array_var("channels");

	for (int i=0;i<p_mixer->get_channel_count();i++) {

		DDS channel;
		channel.set_str_var("name",p_mixer->get_channel_name(i));

		DDS chain;
		get_effect_chain_data(p_mixer->get_channel_effect_parameter_chain(i),&chain);
		channel.set_dds_var("effect_chain",chain);


		channel.set_dds_array_var("outputs",aux_vector_dds);
		vector_dds& outputs=channel.get_dds_array_var("outputs");

		for (int j=0;j<p_mixer->get_channel_output_count(i);j++) {
			DDS output;
			output.set_int_var("index",p_mixer->get_channel_output_buffer_index(i,j));
			output.set_int_var("initial_send_level",p_mixer->get_channel_output_send_level(i,j));
			output.set_int_var("send_controller",p_mixer->get_channel_output_send_control(i,j));
			outputs.push_back(output);
           	}

		channels.push_back(channel);
	}

	p_dds->set_dds_array_var("buffers",aux_vector_dds);
	vector_dds& buffers=p_dds->get_dds_array_var("buffers");

	for (int i=0;i<p_mixer->get_buffer_count();i++) {

		DDS buffer;
		buffer.set_str_var("name",p_mixer->get_buffer_name(i));

		DDS chain;
		get_effect_chain_data(p_mixer->get_buffer_effect_parameter_chain(i),&chain);
		buffer.set_dds_var("effect_chain",chain);


		buffer.set_dds_array_var("outputs",aux_vector_dds);
		vector_dds& outputs=buffer.get_dds_array_var("outputs");

		for (int j=0;j<p_mixer->get_buffer_output_count(i);j++) {
			DDS output;
			output.set_int_var("index",p_mixer->get_buffer_output_buffer_index(i,j));
			output.set_int_var("initial_send_level",p_mixer->get_buffer_output_send_level(i,j));
			output.set_int_var("send_controller",p_mixer->get_buffer_output_send_control(i,j));
			outputs.push_back(output);
           	}

		buffers.push_back(buffer);
	}

}



void DDS_Helpers::get_sample_data(Sample_Data *p_sample,DDS* p_dds)
{


	p_dds->set_int_var("c5_freq",p_sample->get_c5_freq());
	p_dds->set_int_var("has_loop",p_sample->is_loop_enabled());
	p_dds->set_int_var("has_pingpong_loop",p_sample->is_loop_ping_pong());
	p_dds->set_int_var("loop_begin",p_sample->get_loop_begin());
	p_dds->set_int_var("loop_end",p_sample->get_loop_end());
	p_dds->set_int_var("is_16bits",p_sample->is_16bit());
	p_dds->set_str_var("compression_type","none");

	vector<Uint8> data;
	data.resize(p_sample->is_16bit() ? (p_sample->get_size() * 2) : p_sample->get_size() );

	Mutex_Lock_Container *lock = p_sample->lock();
	ns_autoptr<Mutex_Lock_Container> ns_lock;
	ns_lock.ptr_new(lock);
	p_sample->seek(0);

	if (p_sample->is_16bit()) {

		// Convert a 16-bit sample into little-endian
		// format.
		//
		// Since the only purpose of this function is
		// to save CheeseTracker samples, and CheeseTracker
		// 0.9.13 format (and below) does not support stereo,
		// we can drop all channels except channel 0.
		//

		for (size_t i=0;i<p_sample->get_size();i++) {
			Uint16 raw_twoscomp_data = CONVERT_TO_TYPE(Uint16, *p_sample->get_int_sample());

			data[i*2]=raw_twoscomp_data >> BITS_PER_BYTE;
			data[i*2+1]=raw_twoscomp_data & 0xFF;
		}

	} else {

		for(size_t ix=0; ix < p_sample->get_size(); ix++) {
			data[ix]=CONVERT_TO_TYPE(Uint8, *p_sample->get_int_sample());
		}
	}

	p_dds->set_data_var("data",&data[0],data.size());
}



void DDS_Helpers::get_envelope(Envelope *p_envelope,DDS* p_dds)
{

	p_dds->set_int_var("max_nodes",p_envelope->get_max_nodes());
	p_dds->set_int_var("min_nodes",p_envelope->get_min_nodes());
	p_dds->set_int_var("node_count",p_envelope->get_node_count());
	p_dds->set_int_var("enabled",p_envelope->is_enabled());
	p_dds->set_int_var("has_loop",p_envelope->is_loop_enabled());
	p_dds->set_int_var("loop_begin",p_envelope->get_loop_begin());
	p_dds->set_int_var("loop_end",p_envelope->get_loop_end());
	p_dds->set_int_var("has_sustain_loop",p_envelope->is_sustain_loop_enabled());
	p_dds->set_int_var("sustain_loop_begin",p_envelope->get_sustain_loop_begin());
	p_dds->set_int_var("sustain_loop_end",p_envelope->get_sustain_loop_end());

	vector_int node_offset;
	vector_int node_height;
	for (int i=0;i<p_envelope->get_node_count();i++) {

		node_offset.push_back( p_envelope->get_node_offset(i) );
		node_height.push_back( p_envelope->get_node_height(i) );

	}

	p_dds->set_int_array_var("node_offsets",node_offset);
	p_dds->set_int_array_var("node_heights",node_height);

}




