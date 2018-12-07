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
/***************************************************************************
                          midi_client.h  -  description
                             -------------------
    begin                : Tue Jul 16 2002
    copyright            : (C) 2002 by red
    email                : red@server
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MIDI_CLIENT_H
#define MIDI_CLIENT_H

#include "components/midi/midi_event.h"
/**
  *@author red
  */

class MIDI_Client {
public:

	virtual bool is_enabled()=0; //return TRUE if driver is enabled
	virtual bool init()=0; //try to initialize driver, return true if failed
	virtual void finish()=0; // deinitialize driver

	virtual void set_listener_device_count(int p_count)=0; // give notice of the amount of input devices that we handle

	virtual bool has_midi_events_pending()=0; //if events are pending..
        virtual void pop_midi_event(MIDI_Event *p_event)=0; //get'em

	virtual string get_name()=0; //driver name
	virtual bool is_internal()=0; //means it works as an internal loopback or similar driver. May be configured, but not activated or deactivated

	virtual void set_app_name(string p_name)=0; //used to give a hint when registering a port name

	virtual ~MIDI_Client();
};

#endif
