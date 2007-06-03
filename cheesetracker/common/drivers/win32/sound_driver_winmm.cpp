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

#include "sound_driver_winmm.h"

#if ((CYGWIN_ENABLED) || (WIN32_ENABLED))
#include "components/audio/sample_conversion.h"
#include <windows.h>

//portions taken from the libmikmod drivers
#define NUMBUFFERS	3				/* number of buffers */
#define BUFFERSIZE	120				/* buffer size in milliseconds */

class WinMMPrivate {
public:
	HWAVEOUT	handle;
	WAVEHDR		header[NUMBUFFERS];
	LPSTR		buffer[NUMBUFFERS];		/* pointers to buffers */
	ULONG		buffer_size;				/* buffer size in bytes */


};

int Sound_Driver_WinMM::get_mix_frequency() {

    return mix_frequency;
}
bool Sound_Driver_WinMM::get_mix_stereo() {

    return mix_stereo;
}
bool Sound_Driver_WinMM::get_mix_16bits() {

    return mix_16bits;
}
int Sound_Driver_WinMM::get_mix_buffer_size() {

    return mix_buffersize;
}

string Sound_Driver_WinMM::get_name() {

    return "Windows Media Sound Driver";
}
static Sound_Driver_WinMM *instance;

static void CALLBACK nasty_callback(HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2) {
	if (uMsg==WOM_DONE) {
		instance->notify_buffer_done();
	}
	////printf("Callback\n");
}



void Sound_Driver_WinMM::notify_buffer_done() {

	block_count--;
}

int Sound_Driver_WinMM::init() {

	active=false;
	if (waveOutGetNumDevs()==0)
		return FUNCTION_FAILURE;


	WAVEFORMATEX wave_format;
	WORD sample_size;
	MMRESULT res;
	int n;

	int samplesize=1;
	if (mix_stereo) samplesize*=2;
	if (mix_16bits) samplesize*=2;

	wave_format.wFormatTag=WAVE_FORMAT_PCM;
	wave_format.nChannels=mix_stereo?2:1;
	wave_format.nSamplesPerSec=mix_frequency;
	wave_format.nAvgBytesPerSec=mix_frequency*samplesize;
	wave_format.nBlockAlign=samplesize;
	wave_format.wBitsPerSample=mix_frequency?16:8;
	wave_format.cbSize=sizeof(wave_format);

	res=waveOutOpen(&p->handle,WAVE_MAPPER,&wave_format,(DWORD)nasty_callback,0,CALLBACK_FUNCTION);

	if (res!=MMSYSERR_NOERROR) {
		//printf("Cant open waveout\n");
		return FUNCTION_FAILURE;
	}

	//p->buffer_size=mix_frequency*samplesize*BUFFERSIZE/1000;
	p->buffer_size=mix_buffersize;
	p->buffer_size<<=mix_stereo?1:0;
	p->buffer_size<<=mix_16bits?1:0;

	//printf("Preparing audio..");
	for (n=0;n<NUMBUFFERS;n++) {

		p->buffer[n]=(LPSTR)malloc(p->buffer_size);
		p->header[n].lpData=p->buffer[n];
		p->header[n].dwBufferLength=p->buffer_size;
		res=waveOutPrepareHeader(p->handle,&p->header[n],sizeof(WAVEHDR));
		if (!p->buffer[n]||res!=MMSYSERR_NOERROR) {

			//printf("Could not allocate some buffer\n");
			return FUNCTION_FAILURE;
		}
	}

	block_count=0;
	block_to_process=0;

	//printf("Initialized (in theory)\n");
	active=true;
	return FUNCTION_SUCCESS;
}
bool Sound_Driver_WinMM::finish() {
	if (p->handle) {

		for (int n=0;n<NUMBUFFERS;n++) {
			if (p->header[n].dwFlags&WHDR_PREPARED)
			waveOutUnprepareHeader(p->handle,&p->header[n],sizeof(WAVEHDR));
			free(p->buffer[n]);
		}
		while (waveOutClose(p->handle)==WAVERR_STILLPLAYING)
			Sleep(10);
		p->handle=NULL;
	}
	//if (p->handle) waveOutReset(p->handle);
	//p->handle=NULL;

	active=false;
	return true;
}
bool Sound_Driver_WinMM::is_active() {

    return active;
}


void Sound_Driver_WinMM::process_data(int p_samples) {

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

					convert_32s_stereo_to_16s_stereo_add((sample_16s_t*)audio_aux,mixer->get_mixdown_buffer_ptr(),samples_mixed);
				} else {


					convert_32s_stereo_to_16s_mono_add((sample_16s_t*)audio_aux,mixer->get_mixdown_buffer_ptr(),samples_mixed);
				}

			} else {

				if (mix_stereo) {


					convert_32s_stereo_to_8u_stereo_add((sample_8u_t*)audio_aux,mixer->get_mixdown_buffer_ptr(),samples_mixed);
				} else {


					convert_32s_stereo_to_8u_mono_add((sample_8u_t*)audio_aux,mixer->get_mixdown_buffer_ptr(),samples_mixed);
				}
			}

			samples_mixed*=mix_stereo?2:1;
			samples_mixed*=mix_16bits?2:1;
			audio_aux+=samples_mixed;
		}
	}

//	current_buffer_size=p_samples;
}

Sound_Driver::Status Sound_Driver_WinMM::process() {
        if (!active)
		return IDLE;

	ULONG done;

	////printf("Processing %i blocks..\n",block_count);
	while (block_count<NUMBUFFERS) {
		
		audiobuffer=(Uint8*)p->buffer[block_to_process];
		process_data(p->buffer_size);
		
		p->header[block_to_process].dwBufferLength=p->buffer_size;
		waveOutWrite(p->handle,&p->header[block_to_process],sizeof(WAVEHDR));
		if (++block_to_process>=NUMBUFFERS) block_to_process%=NUMBUFFERS;
		++block_count;
	}


	return IDLE;
}
void Sound_Driver_WinMM::flush() {

}

Sound_Driver_WinMM::Sound_Driver_WinMM(){

	p= new WinMMPrivate;
	p->handle=NULL;
	mix_frequency=-1;
	process_mode=POLL;
	instance=this;
}
Sound_Driver_WinMM::~Sound_Driver_WinMM(){

	delete p;
}

#endif
