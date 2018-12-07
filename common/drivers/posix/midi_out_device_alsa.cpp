/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://reduz.com.ar/cheesetronic
    copyright            : (C) 2003 by Juan Linietsky
    email                : coding@reduz.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "midi_out_device_alsa.h"

#ifdef ALSA_ENABLED

string MidiOutDevice_ALSA::get_subdevice_name(int subdevicenum) {

	return alsa_devices[subdevicenum].name;
}

bool MidiOutDevice_ALSA::enable_subdevice(int subdevicenum) { // true if error enabling
	
 	//      rwFlag = rw;
	snd_seq_port_subscribe_t* subs;

	snd_seq_port_subscribe_alloca(&subs);

	snd_seq_port_subscribe_set_sender(subs, &ctxp.address);
	snd_seq_port_subscribe_set_dest(subs, &alsa_devices[subdevicenum].address);

	int error = snd_seq_subscribe_port(ctxp.handle, subs);

	return true;
}


bool MidiOutDevice_ALSA::disable_subdevice(int subdevicenum) { // true if error disabling

	return true;

}

bool MidiOutDevice_ALSA::is_subdevice_enabled(int subdevicenum) { // true if error disabling

	return true;

}
//midi

void MidiOutDevice_ALSA::setup_event(snd_seq_event_t *p_event,int p_devindex) {

 	memset(p_event, 0, sizeof(snd_seq_event_t));
        p_event->queue   = SND_SEQ_QUEUE_DIRECT;
        p_event->source  = ctxp.address;
        p_event->dest    = alsa_devices[p_devindex].address;
}

void MidiOutDevice_ALSA::send_MIDI_noteon(int subdevice,char channel, char note, char velocity) {

	snd_seq_event_t event;
 	setup_event(&event,subdevice);

	snd_seq_ev_set_noteon(&event, channel, note, velocity);
  	/**/
	snd_seq_event_output_direct(ctxp.handle, &event);  	
}

void MidiOutDevice_ALSA::send_MIDI_noteoff(int subdevice,char channel, char note, char velocity) {

	snd_seq_event_t event;
 	setup_event(&event,subdevice);


	snd_seq_ev_set_noteoff(&event, channel, note, velocity);
  	/**/
	snd_seq_event_output_direct(ctxp.handle, &event);  	
}

void MidiOutDevice_ALSA::send_MIDI_program(int subdevice,char channel, char patch) {

	snd_seq_event_t event;
 	setup_event(&event,subdevice);

	snd_seq_ev_set_pgmchange(&event, channel, patch);
  	/**/
	snd_seq_event_output_direct(ctxp.handle, &event);  	
}
void MidiOutDevice_ALSA::send_MIDI_control(int subdevice,char channel, char controller, char value) {

	snd_seq_event_t event;
 	setup_event(&event,subdevice);

	snd_seq_ev_set_controller(&event, channel, controller, value);		
  	/**/
	snd_seq_event_output_direct(ctxp.handle, &event);  	
}

void MidiOutDevice_ALSA::send_MIDI_pitch_bender(int subdevice,char channel, int value) {

	snd_seq_event_t event;
 	setup_event(&event,subdevice);

	snd_seq_ev_set_pitchbend(&event, channel, value-0x2000);
  	/**/
	snd_seq_event_output_direct(ctxp.handle, &event);  	
}

void MidiOutDevice_ALSA::send_MIDI_note_aftertouch(int subdevice,char channel, char note, char pressure) {

	snd_seq_event_t event;
 	setup_event(&event,subdevice);

   	/**/
	snd_seq_event_output_direct(ctxp.handle, &event);  	
};
void MidiOutDevice_ALSA::send_MIDI_channel_aftertouch(int subdevice,char channel, char pressure) {

	snd_seq_event_t event;
 	setup_event(&event,subdevice);

	snd_seq_ev_set_chanpress(&event, channel, pressure);
  	/**/
	snd_seq_event_output_direct(ctxp.handle, &event);  	
}

void MidiOutDevice_ALSA::send_MIDI_RPN(int subdevice, char channel, char lsb, char msb, char datamsb, char datalsb) {


	send_MIDI_control(subdevice,channel,0x65,msb);
	send_MIDI_control(subdevice,channel,0x64,lsb);
	send_MIDI_control(subdevice,channel,0x06,datamsb);
	send_MIDI_control(subdevice,channel,0x26,datalsb);


}
void MidiOutDevice_ALSA::send_MIDI_reset(int subdevice) {
}
void MidiOutDevice_ALSA::send_PANIC() {


}

void MidiOutDevice_ALSA::play_notify() {


}; //notify device that playing started

void MidiOutDevice_ALSA::stop_notify() {

}; //notify device that playing stopped

void MidiOutDevice_ALSA::device_wait(int wait_amount) { // in msecs


}

void MidiOutDevice_ALSA::device_set_tick_offset(int p_offset) { //current+offset in ms, relative to current

	tick.offset=p_offset;	
}
void MidiOutDevice_ALSA::device_set_tick_current(int p_offset) { //absolute

	tick.offset-=p_offset-tick.current;
	tick.current=p_offset;
}


void MidiOutDevice_ALSA::send_MIDI_tempo(int p_bpm) {

	snd_seq_queue_tempo_t *queue_tempo;
	
	snd_seq_queue_tempo_malloc(&queue_tempo);
	int tempo = (int)(6e7 / ((double)p_bpm * (double)TICKS_PER_QUARTER) * (double)TICKS_PER_QUARTER);
	snd_seq_queue_tempo_set_tempo(queue_tempo, tempo);
	snd_seq_queue_tempo_set_ppq(queue_tempo, TICKS_PER_QUARTER);
	snd_seq_set_queue_tempo(ctxp.handle, queue, queue_tempo);
	snd_seq_queue_tempo_free(queue_tempo);

}


MidiOutDevice_ALSA::MidiOutDevice_ALSA() {


	if (snd_seq_open(&ctxp.handle, "hw", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		ERROR("****Could not open sequencer:" << snd_strerror(errno));
        snd_seq_client_info_t *cinfo;

	snd_seq_client_info_alloca(&cinfo);
	snd_seq_client_info_set_client(cinfo, -1);
	subdevices_count=0;

	while (snd_seq_query_next_client(ctxp.handle, cinfo) >= 0) { //iterate through clients

		snd_seq_port_info_t *pinfo;
		snd_seq_port_info_alloca(&pinfo);
		snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
		snd_seq_port_info_set_port(pinfo, -1);
  		

		while (snd_seq_query_next_port(ctxp.handle, pinfo) >= 0) { //and now through ports

			unsigned int capability = snd_seq_port_info_get_capability(pinfo);
   			AlsaDevice new_device;
      			char name[100]; //crap
			name[99]=0;

			if ((capability & SND_SEQ_PORT_CAP_SUBS_WRITE) == 0)
                        	continue;

			new_device.address= *snd_seq_port_info_get_addr(pinfo);
                        new_device.name=snd_seq_port_info_get_name(pinfo);
                        snprintf(name,sizeof(name)-1,"%s (ALSA %i:%i)",new_device.name.c_str(),new_device.address.client,new_device.address.port);
                        new_device.name=name;
                        alsa_devices.push_back(new_device);
                        subdevice_enabled[alsa_devices.size()-1]=true;
                        subdevices_count++;
		}

  		//free(pinfo);
	}


	snd_seq_set_client_name(ctxp.handle, "Shake Tracker");

	ctxp.address.port=snd_seq_create_simple_port(ctxp.handle, "STracker Port 0",SND_SEQ_PORT_CAP_SUBS_WRITE,SND_SEQ_PORT_TYPE_APPLICATION);
 	ctxp.address.client=snd_seq_client_id(ctxp.handle);

	queue=snd_seq_alloc_queue(ctxp.handle);
	snd_seq_set_client_pool_output(ctxp.handle,1024); //is 1024 enough?
  

  	for (int i=0;i<subdevices_count;i++) {

		enable_subdevice(i);
   	}

  	

/*
      int ci = snd_seq_poll_descriptors_count(alsaSeq, POLLIN);
      int co = snd_seq_poll_descriptors_count(alsaSeq, POLLOUT);

      if (ci > 1 || co > 1) {
            //printf("ALSA midi: cannot handle more than one poll fd\n");
            abort();
            }

      struct pollfd pfdi[ci];
      struct pollfd pfdo[co];
      snd_seq_poll_descriptors(alsaSeq, pfdi, ci, POLLIN);
      snd_seq_poll_descriptors(alsaSeq, pfdo, co, POLLOUT);
      alsaSeqFdo = pfdo[0].fd;
      alsaSeqFdi = pfdi[0].fd;

      int port  = snd_seq_create_simple_port(alsaSeq, "MusE Port 0",
         inCap | outCap | SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_WRITE,
         SND_SEQ_PORT_TYPE_APPLICATION);
      if (port < 0) {
            perror("create port");
            exit(1);
            }
      musePort.port   = port;
      musePort.client = snd_seq_client_id(alsaSeq);

	ctxp.queue = snd_seq_alloc_queue(ctxp->handle);

	ctxp->client = snd_seq_client_id(ctxp->handle);

	seq_new_port(ctxp);
        snd_seq_connect_to(ctxp->handle, ctxp->source.port, client, port);
	return ctxp;
  */
}



MidiOutDevice_ALSA::~MidiOutDevice_ALSA() {


}


#endif


