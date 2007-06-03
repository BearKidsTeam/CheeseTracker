//
// C++ Implementation: mixer__buffers
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "mixer.h"
#include "effect_source_manager.h"

static int new_buffer_name_idx=0;

void Mixer::add_buffer() {

	buffer.resize(buffer.size()+1);
	Buffer &new_buffer=buffer[buffer.size()-1];
	new_buffer.name="Buffer "+itostr(++new_buffer_name_idx);
}

void Mixer::remove_buffer(int p_idx) {

	if ((p_idx<0) || (p_idx>=buffer.size())) {
		ERROR("Attempting to remove unexisting buffer!!");
	}

	//this is the messier, since i first need to remove references to this buffer!

	for (int i=0;i<buffer.size();i++) {

		for (int j=0;j<buffer[i].output_list.size();j++) {

			int cidx=buffer[i].output_list[j].buffer_index;
			if (cidx==MIXDOWN_BUFFER_INDEX)
				continue;

			if (cidx==p_idx) { // if it's the same, erase it

				buffer[i].output_list.erase( buffer[i].output_list.begin() + j);
				--j; //since it's gone, lets dec-it
			} else if (cidx>p_idx) { //if greater, decrement it, so it still points to the proper one

				buffer[i].output_list[j].buffer_index--;
			}
		}
	}

	for (int i=0;i<channel.size();i++) {

		for (int j=0;j<channel[i].output_list.size();j++) {

			int cidx=channel[i].output_list[j].buffer_index;
			if (cidx==MIXDOWN_BUFFER_INDEX)
				continue;

			if (cidx==p_idx) { // if it's the same, erase it

				channel[i].output_list.erase( channel[i].output_list.begin() + j);
				--j; //since it's gone, lets dec-it
			} else if (cidx>p_idx) { //if greater, decrement it, so it still points to the proper one

				channel[i].output_list[j].buffer_index--;
			}
		}
	}

	buffer.delete_element( p_idx );
}

int Mixer::get_buffer_count() {

	return buffer.size();
}


Effect_Parameter_Chain * Mixer::get_buffer_effect_parameter_chain(int p_buffer) {

 	if ((p_buffer<0) || (p_buffer>=buffer.size()))
		return NULL;
	return buffer[p_buffer].effect_chain_parameters;

}


void Mixer::set_channel_name(int p_channel,string p_name) {
	if ((p_channel<0) || (p_channel>=channel.size()))
		return;
	channel[p_channel].name=p_name;
}
string Mixer::get_channel_name(int p_channel) {

	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return "";
	}
	return channel[p_channel].name;

}

void Mixer::set_buffer_name(int p_buffer,string p_name) {
	if ((p_buffer<0) || (p_buffer>=buffer.size()))
		return;
	buffer[p_buffer].name=p_name;
}
string Mixer::get_buffer_name(int p_buffer) {

	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return "";
	}
	return buffer[p_buffer].name;

}

int Mixer::get_channel_output_count(int p_channel) {

	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return -1;
	}
	return channel[p_channel].output_list.size();
}

int Mixer::get_channel_output_buffer_index(int p_channel,int p_output) {

	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return -1;
	}
	if ((p_output<0) || (p_output>=channel[p_channel].output_list.size())) {
		ERROR("Invalid channel output index");
		return -1;
	}
	return channel[p_channel].output_list[p_output].buffer_index;

}
int Mixer::get_channel_output_send_level(int p_channel,int p_output) {

	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return -1;
	}
	if ((p_output<0) || (p_output>=channel[p_channel].output_list.size())) {
		ERROR("Invalid channel output index");
		return -1;
	}
	return channel[p_channel].output_list[p_output].send_level;

}
int Mixer::get_channel_output_send_control(int p_channel,int p_output) {

	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return -1;
	}
	if ((p_output<0) || (p_output>=channel[p_channel].output_list.size())) {
		ERROR("Invalid channel output index");
		return -1;
	}
	return channel[p_channel].output_list[p_output].send_control;
}

void Mixer::remove_channel_output(int p_channel,int p_output) {
	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return ;
	}
	if ((p_output<0) || (p_output>=channel[p_channel].output_list.size())) {
		ERROR("Invalid channel output index");
		return ;
	}

	channel[p_channel].output_list.erase(
		channel[p_channel].output_list.begin() + p_output
	);
}

void Mixer::set_channel_output_buffer_index(int p_channel,int p_output,int p_index) {

	if ((p_index!=MIXDOWN_BUFFER_INDEX) && ((p_index<0) || (p_index>=buffer.size()))) {

		ERROR("Wrong buffer index!");
		return;
	}


	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return ;
	}
	if ((p_output<0) || (p_output>=channel[p_channel].output_list.size())) {
		ERROR("Invalid channel output index");
		return ;
	}
	channel[p_channel].output_list[p_output].buffer_index=p_index;

}
void Mixer::set_channel_output_send_level(int p_channel,int p_output,int p_level) {

	if ((p_level<0) || (p_level>127)) {
		ERROR("Wrong send level!!");
		return;
	}

	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return ;
	}
	if ((p_output<0) || (p_output>=channel[p_channel].output_list.size())) {
		ERROR("Invalid channel output index");
		return ;
	}
	channel[p_channel].output_list[p_output].send_level=p_level;

}
void Mixer::set_channel_output_send_control(int p_channel,int p_output,int p_control) {

	if ((p_control<-1) || (p_control>127)) {
		ERROR("Wrong send control!!");
		return;
	}

	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return ;
	}
	if ((p_output<0) || (p_output>=channel[p_channel].output_list.size())) {
		ERROR("Invalid channel output index");
		return ;
	}
	channel[p_channel].output_list[p_output].send_control=p_control;

}

void Mixer::add_channel_output(int p_channel) {

	if ((p_channel<0) || (p_channel>=channel.size())) {
		ERROR("Invalid channel index");
		return ;
	}
	Buffer::Output new_output;
	new_output.buffer_index=MIXDOWN_BUFFER_INDEX;
	new_output.send_level=127;
	new_output.send_control=-1;
	channel[p_channel].output_list.push_back(new_output);
}

int Mixer::get_buffer_output_count(int p_buffer) {

	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return -1;
	}

	return buffer[p_buffer].output_list.size();
}

int Mixer::get_buffer_output_buffer_index(int p_buffer,int p_output) {

	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return -1;
	}
	if ((p_output<0) || (p_output>=buffer[p_buffer].output_list.size())) {
		ERROR("Invalid buffer output index");
		return -1;
	}
	return buffer[p_buffer].output_list[p_output].buffer_index;

}
int Mixer::get_buffer_output_send_level(int p_buffer,int p_output) {

	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return -1;
	}
	if ((p_output<0) || (p_output>=buffer[p_buffer].output_list.size())) {
		ERROR("Invalid buffer output index");
		return -1;
	}
	return buffer[p_buffer].output_list[p_output].send_level;

}
int Mixer::get_buffer_output_send_control(int p_buffer,int p_output) {

	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return -1;
	}
	if ((p_output<0) || (p_output>=buffer[p_buffer].output_list.size())) {
		ERROR("Invalid buffer output index");
		return -1;
	}
	return buffer[p_buffer].output_list[p_output].send_control;

}

void Mixer::remove_buffer_output(int p_buffer,int p_output) {

	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return ;
	}
	if ((p_output<0) || (p_output>=buffer[p_buffer].output_list.size())) {
		ERROR("Invalid buffer output index");
		return ;
	}

	buffer[p_buffer].output_list.erase(
		buffer[p_buffer].output_list.begin() + p_output
	);

}

void Mixer::set_buffer_output_buffer_index(int p_buffer,int p_output,int p_index) {

	if ((p_index>=p_buffer) && p_index!=MIXDOWN_BUFFER_INDEX) {
		ERROR("A buffer cant mix to a higher order one, otherwise cyclic relationships may happen!");
		return;
	}

	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return ;
	}
	if ((p_output<0) || (p_output>=buffer[p_buffer].output_list.size())) {
		ERROR("Invalid buffer output index");
		return ;
	}
	buffer[p_buffer].output_list[p_output].buffer_index=p_index;

}
void Mixer::set_buffer_output_send_level(int p_buffer,int p_output,int p_level) {
	if ((p_level<0) || (p_level>127)) {
		ERROR("Wrong send level!!");
		return;
	}

	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return ;
	}
	if ((p_output<0) || (p_output>=buffer[p_buffer].output_list.size())) {
		ERROR("Invalid buffer output index");
		return ;
	}
	buffer[p_buffer].output_list[p_output].send_level=p_level;

}
void Mixer::set_buffer_output_send_control(int p_buffer,int p_output,int p_control) {

	if ((p_control<-1) || (p_control>127)) {
		ERROR("Wrong send control!!");
		return;
	}
	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return ;
	}
	if ((p_output<0) || (p_output>=buffer[p_buffer].output_list.size())) {
		ERROR("Invalid buffer output index");
		return ;
	}
	buffer[p_buffer].output_list[p_output].send_control=p_control;

}

void Mixer::add_buffer_output(int p_buffer) {

	if ((p_buffer<0) || (p_buffer>=buffer.size())) {
		ERROR("Invalid buffer index");
		return ;
	}
	Buffer::Output new_output;
	new_output.buffer_index=MIXDOWN_BUFFER_INDEX;
	new_output.send_level=127;
	new_output.send_control=-1;
	buffer[p_buffer].output_list.push_back(new_output);

}

void Mixer::set_chorus_and_reverb_routes_preset() {
	
	for (int i=0;i<channel.size();i++) {
		channel[i].effect_chain_parameters->reset();
		channel[i].output_list.clear();
	}		
		
	for (int i=0;i<buffer.size();i++) {
		buffer[i].effect_chain_parameters->reset();
		buffer[i].output_list.clear();
	}		
	buffer.clear();
	
			
	Effect_Source_Manager *esm = Effect_Source_Manager::get_singleton_instance();
	Effect::Parameters *aux_fx=NULL;
	
	add_buffer();
	add_buffer();
	add_buffer();
	
#define SET_FLOAT_PROPERTY(m_list,m_name,m_val) \
	{\
		list<Property_Bridge*> dalist=m_list;\
		for (list<Property_Bridge*>::iterator I=dalist.begin();I!=dalist.end();I++) { \
			Float_Property_Bridge *f=dynamic_cast<Float_Property_Bridge*>(*I); \
			if (f->get_short_name()==m_name) 	\
				f->set(m_val); \
		}\
	}
	
	
	set_buffer_name(0,"6-bands EQ");
	set_buffer_name(1,"Reverb");
	aux_fx=esm->instantiate_effect_parameters_by_name("INTERNAL: freeverb");

	if (aux_fx) {                                  
		SET_FLOAT_PROPERTY(aux_fx->get_property_list(),"dry",0);
		SET_FLOAT_PROPERTY(aux_fx->get_property_list(),"wet",1);
		get_buffer_effect_parameter_chain(1)->add_effect(aux_fx);
	}
	set_buffer_name(2,"Multi-Voice Chorus");
	aux_fx=esm->instantiate_effect_parameters_by_name("INTERNAL: chorus");
	if (aux_fx) {
		SET_FLOAT_PROPERTY(aux_fx->get_property_list(),"dry_level",0);
		SET_FLOAT_PROPERTY(aux_fx->get_property_list(),"wet_level",1);
		get_buffer_effect_parameter_chain(2)->add_effect(aux_fx);
	}
	
#define ADD_BUFFER_OUTPUT(m_buffer,m_output,m_to,m_send,m_control) \
	add_buffer_output(m_buffer); \
	set_buffer_output_buffer_index(m_buffer,m_output,m_to);	\
	set_buffer_output_send_control(m_buffer,m_output,m_control); \
	set_buffer_output_send_level(m_buffer,m_output,m_send);
	
#define ADD_CHANNEL_OUTPUT(m_channel,m_output,m_to,m_send,m_control) \
	add_channel_output(m_channel); \
	set_channel_output_buffer_index(m_channel,m_output,m_to);	\
	set_channel_output_send_control(m_channel,m_output,m_control); \
	set_channel_output_send_level(m_channel,m_output,m_send);
	
	
	
	
	ADD_BUFFER_OUTPUT(0,0,MIXDOWN_BUFFER_INDEX,127,-1);
	ADD_BUFFER_OUTPUT(1,0,0,127,-1);
	ADD_BUFFER_OUTPUT(2,0,0,127,-1);
			
	for (int i=0;i<channel.size();i++) {
		
		ADD_CHANNEL_OUTPUT(i,0,0,127,-1);
		ADD_CHANNEL_OUTPUT(i,1,1,32,91);
		ADD_CHANNEL_OUTPUT(i,2,2,16,93);
	}		
	
}