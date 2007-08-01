//
// C++ Implementation: midi_client_alsa
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "midi_client_alsa.h"


#ifdef ALSA_ENABLED


bool MIDI_Client_ALSA::is_enabled() {

	return active;

}

bool MIDI_Client_ALSA::init() {

	if (active)
		return true;
	snd_seq_port_info_t *pinfo;
	int port;

	/* open that bitch */
	if (snd_seq_open(&handle, "hw", SND_SEQ_OPEN_INPUT, 0)<0) return true;


	client = snd_seq_client_id(handle); //get client?
	//fd = get_file_descriptor();
	snd_seq_set_client_pool_input(handle, 1000); /* enough? */


	port = snd_seq_create_simple_port(handle, app_name.c_str(),
					  SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
					  SND_SEQ_PORT_TYPE_MIDI_GENERIC);
	if (port < 0) {
		ERROR("error in snd_seq_create_simple_port");
		return true;
	}


	active=true;
	port_index=port;

	return false;
}

void MIDI_Client_ALSA::finish() {

	if (!active)
		return;

	snd_seq_close(handle);
	active=false;

}

void MIDI_Client_ALSA::set_listener_device_count(int p_count) {

	listener_count=p_count;

}

bool MIDI_Client_ALSA::has_midi_events_pending() {

       if (!active) return false;

	int pending = snd_seq_event_input_pending(handle, 1);
	return (pending>0);
}

void MIDI_Client_ALSA::pop_midi_event(MIDI_Event *p_event) {

	if (!active)
		return;

	if (!snd_seq_event_input_pending(handle, 1)) {
		ERROR("DONT CALL ME IF NO INPUT EVENTS ARE PENDING!!!");
		return;
	}


	snd_seq_event_t *event;
	int er;

	er = snd_seq_event_input(handle, &event);

	if (er < 0 || event == NULL) {

		ERROR("Error retrieving event!!");
		return;
	}

	switch(event->type) {

		case SND_SEQ_EVENT_NOTEON: {

			p_event->type=MIDI_Event::NOTE_ON;
			p_event->channel=event->data.note.channel;
			p_event->data.note.note=event->data.note.note;
			p_event->data.note.velocity=event->data.note.velocity;

		//a	midi_event->note_on(event->data.note.channel,event->data.note.note,event->data.note.velocity);



			/*

			ev.channel = NOTE_CHAN(aevp);
			ev.a       = aevp->data.note.note;
			ev.b       = aevp->data.note.velocity;
			if (ev.b == 0)
				ev.type = ME_NOTEOFF;
			else
				ev.type = ME_NOTEON;
			seq_play_event(&ev);
			break; */
		} break;


		case SND_SEQ_EVENT_NOTEOFF: {

			p_event->type=MIDI_Event::NOTE_OFF;
			p_event->channel=event->data.note.channel;
			p_event->data.note.note=event->data.note.note;
			p_event->data.note.velocity=event->data.note.velocity;

		//a	midi_event->note_off(event->data.note.channel,event->data.note.note,event->data.note.velocity);

		/*
			ev.channel = NOTE_CHAN(aevp);
			ev.a       = aevp->data.note.note;
			ev.b       = aevp->data.note.velocity;
			ev.type = ME_NOTEOFF;
			seq_play_event(&ev); */

		} break;

		case SND_SEQ_EVENT_KEYPRESS: {

			p_event->type=MIDI_Event::NOTE_PRESSURE;
			p_event->channel=event->data.note.channel;
			p_event->data.note.note=event->data.note.note;
			p_event->data.note.velocity=event->data.note.velocity;

			/*
			ev.channel = NOTE_CHAN(aevp);
			ev.a       = aevp->data.note.note;
			ev.b       = aevp->data.note.velocity;
			ev.type = ME_KEYPRESSURE;
			seq_play_event(&ev);
			break; */
		} break;

		case SND_SEQ_EVENT_PGMCHANGE: {

			p_event->type=MIDI_Event::PATCH_SELECT;
			p_event->channel=event->data.control.channel;
			p_event->data.patch.index=event->data.control.value;



		//a	midi_event->program_change(event->data.control.channel,event->data.control.value);

			/*
			ev.channel = CTRL_CHAN(aevp);
			ev.a = aevp->data.control.value;
			ev.type = ME_PROGRAM;
			seq_play_event(&ev); */
		} break;

		case SND_SEQ_EVENT_CONTROLLER: {

			p_event->type=MIDI_Event::CONTROLLER;
			p_event->channel=event->data.control.channel;
			p_event->data.control.index=event->data.control.param;
			p_event->data.control.parameter=event->data.control.value;

		//a	midi_event->controller(event->data.control.channel,event->data.control.param,event->data.control.value);

			/*
			if(convert_midi_control_change(CTRL_CHAN(aevp),
						aevp->data.control.param,
						aevp->data.control.value,
						&ev))
				seq_play_event(&ev); */
		} break;


		case SND_SEQ_EVENT_CONTROL14: {

			/*

			if (aevp->data.control.param < 0 || aevp->data.control.param >= 32)
				break;
			if (! convert_midi_control_change(CTRL_CHAN(aevp),
							aevp->data.control.param,
							(aevp->data.control.value >> 7) & 0x7f,
							&ev))
				break;
			seq_play_event(&ev);
			if (! convert_midi_control_change(CTRL_CHAN(aevp),
							aevp->data.control.param + 32,
							aevp->data.control.value & 0x7f,
							&ev))
				break;
			seq_play_event(&ev); */
		} break;

		case SND_SEQ_EVENT_PITCHBEND: {

			p_event->type=MIDI_Event::PITCH_BEND;
			p_event->channel=event->data.control.channel;
			p_event->data.pitch_bend.bend=event->data.control.value+0x2000;


		//a	midi_event->pitch_bend(event->data.control.channel,event->data.control.value+0x2000);
			/*
			ev.type    = ME_PITCHWHEEL;
			ev.channel = CTRL_CHAN(aevp);
			aevp->data.control.value += 0x2000;
			ev.a       = (aevp->data.control.value) & 0x7f;
			ev.b       = (aevp->data.control.value>>7) & 0x7f;
			seq_play_event(&ev); */

		} break;

		case SND_SEQ_EVENT_CHANPRESS: {

			p_event->type=MIDI_Event::AFTERTOUCH;
			p_event->channel=event->data.control.channel;
			p_event->data.aftertouch.pressure=event->data.control.value;

			/*
			ev.type    = ME_CHANNEL_PRESSURE;
			ev.channel = CTRL_CHAN(aevp);
			ev.a       = aevp->data.control.value;
			seq_play_event(&ev); */
		} break;

		case SND_SEQ_EVENT_NONREGPARAM: {

			/*

			ev.type = ME_NRPN_MSB;
			ev.channel = CTRL_CHAN(aevp);
			ev.a = (aevp->data.control.param >> 7) & 0x7f;
			seq_play_event(&ev);
			ev.type = ME_NRPN_LSB;
			ev.channel = CTRL_CHAN(aevp);
			ev.a = aevp->data.control.param & 0x7f;
			seq_play_event(&ev);
			ev.type = ME_DATA_ENTRY_MSB;
			ev.channel = CTRL_CHAN(aevp);
			ev.a = (aevp->data.control.value >> 7) & 0x7f;
			seq_play_event(&ev);
			ev.type = ME_DATA_ENTRY_LSB;
			ev.channel = CTRL_CHAN(aevp);
			ev.a = aevp->data.control.value & 0x7f;
			seq_play_event(&ev);
			*/
		} break;


		case SND_SEQ_EVENT_REGPARAM: {



			/*

			ev.type = ME_RPN_MSB;
			ev.channel = CTRL_CHAN(aevp);
			ev.a = (aevp->data.control.param >> 7) & 0x7f;
			seq_play_event(&ev);
			ev.type = ME_RPN_LSB;
			ev.channel = CTRL_CHAN(aevp);
			ev.a = aevp->data.control.param & 0x7f;
			seq_play_event(&ev);
			ev.type = ME_DATA_ENTRY_MSB;
			ev.channel = CTRL_CHAN(aevp);
			ev.a = (aevp->data.control.value >> 7) & 0x7f;
			seq_play_event(&ev);
			ev.type = ME_DATA_ENTRY_LSB;
			ev.channel = CTRL_CHAN(aevp);
			ev.a = aevp->data.control.value & 0x7f;
			seq_play_event(&ev);
			*/



		} break;

		case SND_SEQ_EVENT_SYSEX: {

			/*
			if (parse_sysex_event(aevp->data.ext.ptr + 1, aevp->data.ext.len - 1, &ev))
			seq_play_event(&ev); */
		} break;

#define snd_seq_addr_equal(a,b)	((a)->client == (b)->client && (a)->port == (b)->port)

		case SND_SEQ_EVENT_PORT_SUBSCRIBED: {

			/*
			if (snd_seq_addr_equal(&aevp->data.connect.dest, &aevp->dest)) {
			if (! ctxp->active) {
			if (! start_sequencer(ctxp)) {
				snd_seq_free_event(aevp);
				return 0;
			}
			}
				ctxp->used++;
			} */
		} break;

		case SND_SEQ_EVENT_PORT_UNSUBSCRIBED: {

			/*
			if (snd_seq_addr_equal(&aevp->data.connect.dest, &aevp->dest)) {
				if (ctxp->active) {
					ctxp->used--;
					if (ctxp->used <= 0) {
						snd_seq_free_event(aevp);
						return 1;
					}
				}
			} */

		} break;

		default: {
			p_event->type=MIDI_Event::SYSEX;

			/*printf("Unsupported event %d\n", aevp->type);*/
		} break;
	}

	snd_seq_free_event(event);

}

string MIDI_Client_ALSA::get_name() {


	return "Alsa";
}

bool MIDI_Client_ALSA::is_internal() {

	return false;
}

void MIDI_Client_ALSA::set_app_name(string p_name) {

	app_name=p_name;

}

MIDI_Client_ALSA::MIDI_Client_ALSA() {

	listener_count=0;
	active=false;
	handle=NULL;
	client=0;
	port_index=0;
	fd=0;
	used=false;

}

MIDI_Client_ALSA::~MIDI_Client_ALSA() {



}

#endif
