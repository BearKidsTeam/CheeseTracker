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
                          sound_driver.cpp  -  description
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

#include "sound_driver_rtaudio.h"
#ifdef RTAUDIO_ENABLED



int Sound_Driver_RtAudio::get_mix_frequency() {

    return mix_frequency;
}
bool Sound_Driver_RtAudio::get_mix_stereo() {

    return mix_stereo;
}
bool Sound_Driver_RtAudio::get_mix_16bits() {

    return mix_16bits;
}
int Sound_Driver_RtAudio::get_mix_buffer_size() {

    return mix_buffersize;
}

string Sound_Driver_RtAudio::get_name() {

    return "RtAudio Sound Driver";
}

int Sound_Driver_RtAudio::init() {

	if (active)
		return FUNCTION_SUCCESS;

	active=false;
	buffer_size = mix_buffersize;
	int n_buffers = 4;      // number of internal buffers used by device
	int device = 0;         // 0 indicates the default or first available device
        audio=NULL;


	try {
                // int -> float conversion seems to be extremely fast in macs :)
		audio = new RtAudio(&stream, device, mix_stereo?2:1 , 0, 0, RtAudio::RTAUDIO_SINT16, mix_frequency, &buffer_size, n_buffers);
	} catch (RtError &error) {
		error.printMessage();
		return SOUND_DRIVER_ERROR_OPENING_DEVICE;
	}

	if (!audio)
		return SOUND_DRIVER_ERROR_OPENING_DEVICE;
	try {
	// Get a pointer to the stream buffer
		buffer = (Sint16 *) audio->getStreamBuffer(stream);
		audio->startStream(stream);
	} catch (RtError &error) {
		error.printMessage();
		return SOUND_DRIVER_ERROR_OPENING_DEVICE;
	}

	active=true;
	return FUNCTION_SUCCESS;
}
bool Sound_Driver_RtAudio::finish() {

	if (!active)
		return false;
	try {
		// Stop and close the stream
		audio->stopStream(stream);
		audio->closeStream(stream);
	} catch (RtError &error) {
		error.printMessage();
	}
	delete audio;
	active=false;
	return true;
}
bool Sound_Driver_RtAudio::is_active() {

    return active;
}


void Sound_Driver_RtAudio::process_data() {

//	buffer = (float *) audio->getStreamBuffer(stream);

	memset(buffer,0,sizeof(Sint16)*buffer_size*(mix_stereo?2:1));

	for (int i=0;i<get_mixer_list()->size();i++) {

	 	int samples;

		Mixer_Base *mixer=(*get_mixer_list())[i];
		mixer->set_mix_frequency(mix_frequency);

		samples=buffer_size;

		//samples/=mix_stereo?2:1;

		int samples_todo=samples;
                Sint16 *dest=buffer;

		while (samples_todo) {

			int samples_mixed=mixer->write_samples(samples_todo);
			samples_todo-=samples_mixed;

                        sample_t *src=&mixer->get_mixdown_buffer_ptr()[0];

			if (mix_stereo) {
				for (int i=0;i<(samples_mixed*2);i++) {

					Sint32 sample=lrintf(*(src++) * 32767.0);
					Sint16 ssample=sample;
					*dest++ +=ssample;

				}
			} else {

				for (int i=0;i<samples_mixed;i++) {

					Sint32 sample=lrintf(*(src++) * 16383.0);
					sample+=lrintf(*(src++) * 16383.0);
					*dest++ +=(Sint16)sample;
				}
			}
		}
	}
}

Sound_Driver::Status Sound_Driver_RtAudio::process() {
        if (!active)
		return IDLE;

	process_data(); //process the data

	return ACTIVE;
}
void Sound_Driver_RtAudio::flush() {

	try { //and now mix it!
		audio->tickStream(stream);
	} catch (RtError &error) {
		error.printMessage();
//		return DISABLED; //error, disable it!
	}

}

Sound_Driver_RtAudio::Sound_Driver_RtAudio(){


	mix_buffersize=4096;
	active=false;
	stream=-1;
	audio=NULL;
}
Sound_Driver_RtAudio::~Sound_Driver_RtAudio(){


}

#endif
