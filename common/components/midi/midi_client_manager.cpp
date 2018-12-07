//
// C++ Implementation: midi_client_manager
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "midi_client_manager.h"

MIDI_Client_Manager * MIDI_Client_Manager::singleton_instance=NULL;

MIDI_Client_Manager *MIDI_Client_Manager::get_singleton_instance() {


	return singleton_instance;
}


void MIDI_Client_Manager::set_app_name(string p_name) {

	app_name=p_name;

}


void MIDI_Client_Manager::add_midi_client(MIDI_Client* p_client) {

	midi_clients.push_back(p_client);
	p_client->set_app_name(app_name);
}

int MIDI_Client_Manager::get_midi_client_count() {

	return midi_clients.size();
}
MIDI_Client* MIDI_Client_Manager::get_midi_client(int p_idx) {

	if ((p_idx<0) || (p_idx>=midi_clients.size()))
		return NULL;

	return midi_clients[p_idx];

}


bool MIDI_Client_Manager::pop_midi_event(MIDI_Event *p_event) {

	for(int i=0;i<midi_clients.size();i++) {

		if (!midi_clients[i]->is_enabled())
			continue;
		if (midi_clients[i]->has_midi_events_pending()) {
			midi_clients[i]->pop_midi_event(p_event);
			return true;
		}
	}

	return false;
}

int MIDI_Client_Manager::pop_midi_events(MIDI_Event *p_event_array,int p_array_max) {

	int count=0;

	for(int i=0;i<midi_clients.size();i++) {

		if (!midi_clients[i]->is_enabled())
			continue;

		while (midi_clients[i]->has_midi_events_pending()) {
			midi_clients[i]->pop_midi_event(p_event_array);
			p_event_array++;
			count++;
			if (count==p_array_max)
				return count;
		}
	}

	return count;
}

void MIDI_Client_Manager::init_all_clients() {

	for(int i=0;i<midi_clients.size();i++) {

		midi_clients[i]->init();
	}

}


MIDI_Client_Manager::MIDI_Client_Manager() {

	singleton_instance=this;
}

