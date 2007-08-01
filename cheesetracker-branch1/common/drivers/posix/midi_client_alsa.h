//
// C++ Interface: midi_client_alsa
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MIDI_CLIENT_ALSA_H
#define MIDI_CLIENT_ALSA_H



#ifdef ALSA_ENABLED

#include "components/midi/midi_client.h"
#include <alsa/asoundlib.h>
/**
@author Juan Linietsky
*/
class MIDI_Client_ALSA : public MIDI_Client {


	string app_name;

	snd_seq_t *handle;	/* The snd_seq handle to /dev/snd/seq */
	int client;		/* The client associated with this context */
	int port_index;	/* created sequencer ports */
	int fd;			/* The file descriptor */
	int used;		/* number of current connection */
	bool active;		/* */

	int listener_count;

//	int get_file_descriptor();
public:

	bool is_enabled();
	bool init();
	void finish();

	void set_listener_device_count(int p_count);

	bool has_midi_events_pending();
        void pop_midi_event(MIDI_Event *p_event);

	string get_name();
	bool is_internal();

	void set_app_name(string p_name);

	MIDI_Client_ALSA();
	~MIDI_Client_ALSA();

};

#endif /* alsa enabled */

#endif
