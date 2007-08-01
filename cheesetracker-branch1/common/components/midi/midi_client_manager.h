//
// C++ Interface: midi_client_manager
//
// Description: 
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MIDI_CLIENT_MANAGER_H
#define MIDI_CLIENT_MANAGER_H

#include "components/midi/midi_client.h"
#include <vector>
	using std::vector;
/**
@author Juan Linietsky
*/
class MIDI_Client_Manager {

	static MIDI_Client_Manager *singleton_instance;

	vector<MIDI_Client*> midi_clients;
	string app_name;
public:

	static MIDI_Client_Manager *get_singleton_instance();
	void add_midi_client(MIDI_Client* p_client);
	int get_midi_client_count();
	MIDI_Client* get_midi_client(int p_idx);

	void set_app_name(string p_name);

	bool pop_midi_event(MIDI_Event *p_event); //try to pop an event, return true if succeded
	int pop_midi_events(MIDI_Event *p_event_array,int p_array_max); //optimized_fucntion, fills an array and returns how much did it get

	void init_all_clients();

	MIDI_Client_Manager();

};

#endif
