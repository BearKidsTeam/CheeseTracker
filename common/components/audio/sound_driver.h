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
                          sound_driver.h  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Juan Linietsky
    email                : reduz@anime.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SOUND_DRIVER_H
#define SOUND_DRIVER_H


#include "components/audio/mixer_base.h"
#include <vector>
	using std::vector;

/**sound driver base class
  *@author Juan Linietsky
  */

/******************************
 sound_driver.h
 --------

I think this is in theory how a sound
driver should look like
but I think it needs something to know
when the song begins/ends
********************************/

class Sound_Driver : public SigC::Object {

	vector<Mixer_Base*> *mixer_list;
public:
	enum ProcessMode {

		POLL,
		CALLBACK
	};

	enum Status {
		DISABLED,
		IDLE,
		ACTIVE
	};

protected:

       	int mix_frequency;
	bool mix_16bits;
	bool mix_stereo;
	int mix_buffersize;
	ProcessMode process_mode;
	vector<Mixer_Base*> *get_mixer_list();

public:

	SigC::Signal0<void> callback;


	ProcessMode get_process_mode() { return process_mode; };
	virtual bool accepts_blocking();
	enum {

		SOUND_DRIVER_ERROR_OPENING_DEVICE=-2,
		SOUND_DRIVER_ERROR_CONFIGURING_DEVICE=-3,
	};

      	virtual void request_mix_frequency(int p_mix_frequency) {mix_frequency=p_mix_frequency ;};
        virtual void request_mix_stereo(bool p_mix_stereo) {mix_stereo=p_mix_stereo; };
        virtual void request_mix_16bits(bool p_mix_16bits) {mix_16bits=p_mix_16bits; };
        virtual void request_buffer_size(int p_buffsize) {mix_buffersize=p_buffsize ;};

      	virtual int get_mix_frequency() =0;
        virtual bool get_mix_stereo() =0;
        virtual bool get_mix_16bits() =0;
        virtual int get_mix_buffer_size() =0;

	virtual string get_name() =0;

	virtual int init() =0;
	virtual bool finish() =0;
	virtual bool is_active() =0;
        virtual Status process() =0;
        virtual void flush() =0;
	virtual void set_mixer_list(vector<Mixer_Base*>* p_mixer_list);

	virtual bool does_perform_disk_write();
	virtual string get_disk_writer_extension();
	virtual string get_disk_writer_file();
	virtual void set_disk_writer_file(string p_file);

	Sound_Driver();
	virtual ~Sound_Driver();

};

#endif
