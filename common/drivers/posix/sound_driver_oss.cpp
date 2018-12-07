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
                          sound_driver_oss.cpp  -  description
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

#ifdef OSS_ENABLED

#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include "Error.h"
#include "sound_driver_oss.h"
#include "math.h"
#include "components/audio/sample_conversion.h"



#define DEFAULT_AUDIO_DEVICE "/dev/dsp"


bool Sound_Driver_OSS::does_perform_disk_write() {

	return true;
}

string Sound_Driver_OSS::get_disk_writer_extension() {

	return "";

}
string Sound_Driver_OSS::get_disk_writer_file() {

	return device_file;

}
void Sound_Driver_OSS::set_disk_writer_file(string p_file) {

	device_file=p_file;
}


int Sound_Driver_OSS::init() {

	int fragment_size;
	int play_stereo,play_rate;
	int orig_precision,orig_stereo;
	long supported_formats;
	audio_buf_info buffinf;

	if (mix_frequency==-1) {

		Error E;
		E.set_error_pfx("Sound_Driver_OSS");
		E.set_error("Invalid Sampling Rate");
		throw E;

	}

	if ( (sound_fd=open(device_file.c_str(),O_WRONLY))<0 ) {
		throw Error(errno, device_file.c_str());
	}


	if (mix_buffersize<512) mix_buffersize=512;

	fragsize=10+(int)(log((float)(mix_buffersize >> 9))/log(2.0) );

//	fragsize = 10; //13?
	numfrags = 7;


	fragment_size=(numfrags<<16)|fragsize;

	if( ioctl(sound_fd,SNDCTL_DSP_SETFRAGMENT,&fragment_size)<0 ) {
		Error E(errno, "SNDCTL_DSP_SETFRAGMENT");
		finish();
		throw E;
	}


/* Ask device for supported formats */
	if(ioctl(sound_fd,SNDCTL_DSP_GETFMTS,&supported_formats)<0) {
		Error E(errno, "SNDCTL_DSP_GETFMTS");
		finish();
		throw E;
	}

	orig_precision=play_precision=(mix_16bits)?AFMT_S16_NE:AFMT_U8;


	if(!(supported_formats & play_precision)) {

		/* Device does not support the format we would prefer... */
	        /* We could try 8 bit sound if available */

		if(play_precision==AFMT_S16_NE &&(supported_formats&AFMT_U8)) {

			Error E;
			E.set_error_pfx("Sound_Driver_OSS::init");
			E.eprintf("%s supports neither 8 nor 16 bits", device_file.c_str());
			finish();
			throw E;
		}
	}

	if ( (ioctl(sound_fd,SNDCTL_DSP_SETFMT,&play_precision)<0 ) && ( orig_precision!=play_precision )) {

		Error E(errno, "SNDCTL_DSP_SETFMT");
		finish();
		throw E;
	}

	orig_stereo=play_stereo=(mix_stereo)?2:1;

	if ( ioctl(sound_fd,SNDCTL_DSP_CHANNELS,&play_stereo)<0 ) {

		Error E(errno, "SNDCTL_DSP_CHANNELS");
		finish();
		throw E;
	}

	play_rate=mix_frequency;

	if ( (ioctl(sound_fd,SNDCTL_DSP_SPEED,&play_rate)<0) ) {

		Error E(errno, "SNDCTL_DSP_SPEED");
		finish();
		throw E;
	}


	/* This call fails on Linux/PPC */
	if( (ioctl(sound_fd,SNDCTL_DSP_GETOSPACE,&buffinf)<0) ) {

		ioctl(sound_fd,SNDCTL_DSP_GETBLKSIZE,&buffinf.fragsize);
	}


	audiobuffer=(Sint8*)calloc(1,buffinf.fragsize);

	if ( audiobuffer==NULL ) {

		finish();
		throw std::bad_alloc();
	}


	buffersize = buffinf.fragsize;

//	mixer->set_mix_stereo(((play_stereo==2)?true:false));
//	mixer->set_mix_16bits((play_precision==AFMT_S16_NE));

	mix_frequency=play_rate;

	mix_stereo=((play_stereo==2)?true:false);
	mix_16bits=(play_precision==AFMT_S16_NE);
//	mix_buffersize=buffersize;

	active=true;

	return FUNCTION_SUCCESS;
}

bool Sound_Driver_OSS::finish() {

	if ( sound_fd>=0 ) {
		if(close(sound_fd) == -1) {
			throw Error(errno, "/dev/dsp: Cannot close");
		}
		// Sleep for a quarter of a second. This prevents the
		// "Device or resource busy" error if init() is called
		// immediately after finish().
		usleep(250000);
		sound_fd = -1;
	}

	if (audiobuffer!=NULL) free(audiobuffer);
	audiobuffer=NULL;

	active=false;

	return true;
}

void Sound_Driver_OSS::process_data(int p_samples) {

	memset(audiobuffer,0,p_samples);

	for (int i=0;i<get_mixer_list()->size();i++) {

	 	int samples;

		Mixer_Base *mixer=(*get_mixer_list())[i];
		mixer->set_mix_frequency(mix_frequency);

		samples=p_samples;

		samples/=mix_stereo?2:1;
		samples/=mix_16bits?2:1;


		int samples_todo=samples;
		Uint8 *audio_aux=(Uint8*)audiobuffer;

		while (samples_todo) {

			int samples_mixed=mixer->write_samples(samples_todo);
			samples_todo-=samples_mixed;

			if (mix_16bits) {

				if (mix_stereo) {

					convert_32f_stereo_to_16s_stereo_add((sample_16s_t*)audio_aux,mixer->get_mixdown_buffer_ptr(),samples_mixed);
				} else {

					convert_32f_stereo_to_16s_mono_add((sample_16s_t*)audio_aux,mixer->get_mixdown_buffer_ptr(),samples_mixed);
				}

			} else {

				if (mix_stereo) {

					convert_32f_stereo_to_8u_stereo_add((sample_8u_t*)audio_aux,mixer->get_mixdown_buffer_ptr(),samples_mixed);
				} else {

					convert_32f_stereo_to_8u_mono_add((sample_8u_t*)audio_aux,mixer->get_mixdown_buffer_ptr(),samples_mixed);
				}
			}

			samples_mixed*=mix_stereo?2:1;
			samples_mixed*=mix_16bits?2:1;
			audio_aux+=samples_mixed;
		}
	}

	current_buffer_size=p_samples;
}


Sound_Driver::Status Sound_Driver_OSS::process() {

	if (!active) return DISABLED;

	if ((!get_mixer_list() || !get_mixer_list()->size()))
		return IDLE;

	audio_buf_info buffinf;

        if (!blocking_mode) { //nonblocking mode (used for polling by hand) sems to work

		for (;;) {
			if ((ioctl(sound_fd,SNDCTL_DSP_GETOSPACE,&buffinf)<0)) {

				buffinf.fragments--;
				buffinf.fragsize = buffinf.bytes = buffersize;
			}

			if(!buffinf.fragments) break;


			process_data(buffinf.fragsize>buffinf.bytes?buffinf.bytes:buffinf.fragsize);
			write(sound_fd,audiobuffer,current_buffer_size);

		}
		return IDLE;


	} else { //blocking mode

		process_data(buffersize);
	}

	return ACTIVE;
}


void Sound_Driver_OSS::flush() {

	if (blocking_mode)
		write(sound_fd,audiobuffer,current_buffer_size);

}

int Sound_Driver_OSS::get_mix_frequency() {

 	return mix_frequency;
}
bool Sound_Driver_OSS::get_mix_stereo() {

 	return mix_stereo;
}

bool Sound_Driver_OSS::get_mix_16bits() {

	return mix_16bits;

}
int Sound_Driver_OSS::get_mix_buffer_size() {

	return mix_buffersize;
}

Sound_Driver_OSS::Sound_Driver_OSS(){

	fragsize = 14;
	numfrags = 16;

	audiobuffer = NULL;
	sound_fd=-1;

	active=false;
	blocking_mode=true;

	device_file=DEFAULT_AUDIO_DEVICE;
}
Sound_Driver_OSS::~Sound_Driver_OSS(){

}

#endif
