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
//
//
// C++ Implementation: cpp
//
// Description:
//
//
// Author: Juan Linietsky <coding@reduz.com.ar>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "interface__QT/interface.h"
#include "interface__QT/mdi_main_window.h"
#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
#include "components/audio/sound_driver_manager.h"
#include "components/audio/sound_driver_dummy.h"
#include "os/path_data.h"
#include "drivers/posix/sound_driver_oss.h"
#include "drivers/posix/sound_driver_jack.h"
#include "drivers/posix/mutex_lock_pthreads.h"
#include "drivers/posix/midi_client_alsa.h"
#include "drivers/rtaudio/sound_driver_rtaudio.h"
#include "drivers/win32/sound_driver_winmm.h"
#include "components/data/keyboard_input.h"
#include "components/audio/effect_source_manager.h"
#include "components/audio/effect_source_internal.h"
#include "components/midi/midi_client_manager.h"
                                                      
						      
#include "plugins/effects/custom/effect_amp.h"
#include "plugins/effects/custom/freeverb_effect.h"
#include "plugins/effects/custom/chorus_effect.h"
#include "plugins/effects/custom/effect_echo.h"
#include "plugins/effects/custom/effect_equalizer.h"
#include "plugins/effects/custom/effect_stereo_fx.h"
#include "plugins/effects/custom/effect_distortion.h"
#include "plugins/effects/custom/effect_pshift.h"

						      /*
//#include "plugins/effects/custom/dummy_effect.h"

#include "plugins/effects/custom/chorus_effect.h"

#include "plugins/effects/custom/effect_stereo_expander.h"
#include "plugins/effects/custom/effect_stereo_enhancer.h"                                                    
#include "plugins/effects/custom/effect_distort.h"

*/

#include "plugins/effects/ladspa/ladspa_effect_source.h"
#include "plugins/resamplers/resampler_dummy.h"
#include "plugins/resamplers/resampler_cubic.h"
#include "plugins/resamplers/resampler_cosine.h"

#include <iostream>
#include <exception>
#include <unistd.h>

#include "Error.h"

using std::cerr;

Sound_Driver_Manager driver_manager; //this is global yo :)

#ifdef POSIX_ENABLED

	#define SLEEP_MSECS(m_arg) usleep(m_arg*1000)
#endif

static bool quit_request=false;
static bool thread_finished=false;

void* audio_thread_cbk(void *data) {

	while(!quit_request)
		if (driver_manager.external_poll_active_driver())
			SLEEP_MSECS(1);

	thread_finished=true;
	
	return NULL;
}


#ifdef WIN32_ENABLED

static const char* DIR_SEPARATOR= "\\";
static const char* CONFIG_FILE= "cheese_qt.cfg";

#endif
#ifdef POSIX_ENABLED

static const char* CONFIG_FILE= ".cheesetracker_QT";
static const char* DIR_SEPARATOR= "/";

#endif

void display_help() {

	printf("Usage: cheesetracker_qt [flags] [songs]\n");
	printf("Flags:\n");
	printf("\t--help shows this help\n");
	printf("\t--schedfifo runs at schedfifo priority (suid root needed)\n");
}

// #define UNIT_TEST
#ifdef UNIT_TEST
#include <cstring>
#include <cassert>
#include <algorithm>
#include "common/components/audio/sample_data.h"
#include "Error.h"
using std::max;
using std::min;

#define APPROX(x,y) (max(x,y)-min(x,y) < 0.0001)

void test_sample_data()
{
	const sample_int_t in_buffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
	                                   14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                                           26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
                                           38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48 };
	sample_int_t out_buffer[96];
	Sample_Data test;
	size_t channels=0;

	Error E;
	size_t ix;
	size_t status;
	bool exception_thrown=false;

	// Try it out with 1 to 4 channels

	for(channels=1; channels<=4; channels++) {
		// Initialize the local buffer.
		printf("Testing with %d channels...\n", channels);


		// Go to the beginning for each test except the first.
		// We expect Sample_Data to initialize the position pointer
		// to zero.

		if(channels > 1) {
			test.seek(0);
			test.truncate();
		}

		memset(out_buffer, 0xFF, sizeof(out_buffer));
		
		// Set the sample size to the length of out_buffer.
		// We expect that the sample will be zero-initialized.
		test.set_size(sizeof(out_buffer)/sizeof(sample_int_t)/channels);

		status = test.get_size();

		test.set_num_channels(channels);
		if(test.num_channels() != channels) {
			E.set_error_pfx("set_num_channels");
			E.eprintf("%s: %i: Tried to set up %i channels, but got %i channels instead",
			          __FILE__, __LINE__, channels, test.num_channels());
			throw E;
		}

		if(status != test.get_size()) {
			E.set_error_pfx("set_num_channels");
			E.eprintf("%s: %i: Changing number of channels resized sample from %i to %i!",
			          status, test.get_size());
			throw E;
		}

		// Read the contents of the sample into the local out_buffer.
		//
		status = test.get_sample_array(out_buffer,
				    		test.get_size());

		// Ensure that get_sample_array reports the correct
		// number of samples.
		if(status != sizeof(out_buffer)/sizeof(sample_int_t)/channels) {
			E.set_error_pfx("get_sample_array");
			E.eprintf("Set size to %i but read %i samples",
				 sizeof(out_buffer)/sizeof(sample_int_t)/channels,
				 status);
			throw E;
		}

		// Make sure that out_buffer was filled with zeros by
		// get_sample_array.
		for(ix=0; ix<sizeof(out_buffer)/sizeof(sample_int_t)/channels; ix++) {
			if(out_buffer[ix]) {
				E.set_error_pfx("get_sample_array");
				E.eprintf("Output buffer uninitialized at index %i: Contains %i",
					  ix, out_buffer[ix]);
				throw E;
			}
		}

		test.set_size(10);
		if((status = test.get_current_pos()) != 10) {
			E.set_error_pfx("set_size/get_current_pos");
			E.eprintf("Set size to 10 after initial size. "
				  "Sample position: %i Size: %i",
				  status, test.get_size());
			throw E;
		}

		if(!test.eof_reached()) {
			E.set_error_pfx("eof_reached");
			E.eprintf("%s: %i: eof_reached() is false but shouldn't be.", __FILE__, __LINE__);
			throw E;
		}

		// Make sure that seek() works.

		test.seek(0);
		status = test.get_current_pos();
		if(status) {
			E.set_error_pfx("seek");
			E.eprintf("Sought 0 but ended up at %i", status);
			throw E;
		}

		test.seek(10);
		status=test.get_current_pos();
		if(status != 10) {
			E.set_error_pfx("seek");
			E.eprintf("%s: %i: Sought 10 but ended up at %i",
				  __FILE__, __LINE__, status);
			throw E;
		}
		test.seek(0);

		// Ensure that an attempt to fill out_buffer only results
		// in 10 samples, since that's the size we're using.
		status = test.get_sample_array(out_buffer, sizeof(out_buffer)/sizeof(sample_int_t)/channels);
		if(status != 10) {
			E.set_error_pfx("get_sample_array");
			E.eprintf("Set size to 10 but read %i samples", status);
			throw E;
		}

		// Test if truncate() can create zero-length samples.

		test.seek(0);
		test.truncate();

		status=test.get_size();
		if(status) {
			E.set_error_pfx("truncate");
			E.eprintf("%s: %i: Truncate at position 0 resulted in "
				  "size of %i", __FILE__, __LINE__, status);
			throw E;
		}

		if(!test.eof_reached()) {
			E.set_error_pfx("eof_reached");
			E.eprintf("%s: %i: No EOF in zero-length buffer",
				  __FILE__, __LINE__);
			throw E;
		}

		// Test if get_int_sample throws an exception.

		exception_thrown=false;
		try {
			test.get_int_sample();
		} catch(Sample_EOF_Error S) {
			exception_thrown=true;
		}

		if(!exception_thrown) {
			E.set_error_pfx("get_int_sample");
			E.eprintf("%s:%i: No exception while reading zero-length sample", __FILE__, __LINE__);
			throw E;
		}

		// Make sure get_sample_array() returns zero

		status = test.get_sample_array(out_buffer, 1);

		if(status) {
			E.set_error_pfx("get_sample_array");
			E.eprintf("%s: %i: Got %i samples from an empty buffer", __FILE__, __LINE__, status);
			throw E;
		}

		// Test if put_sample_array extends a zero-length sample.

		test.set_size(0);
		test.put_sample_array(in_buffer, 10);
		status = test.get_size();
		if(status != 10) {
			E.set_error_pfx("put_sample_array");
			E.eprintf("%s: %i: put_sample_array failed to resize sample "
				  "to 10 samples (size=%i)", __FILE__, __LINE__,
				  status);
			throw E;
		}

		// Make sure that the contents of the sample are an exact copy
		// of the data that we put in.

		test.seek(0);
		test.get_sample_array(out_buffer, 10);
		for(ix=0; ix<10*channels; ix++) {
			if(out_buffer[ix] != in_buffer[ix]) {
				E.set_error_pfx("put_sample_array/get_sample_array");
				E.eprintf("%s: %i: Expected value %i but "
					  "got %i instead", __FILE__, __LINE__,
					  ix, out_buffer[ix]);
				throw E;
			}
		}

		// Make sure that get_int_sample() agrees with
		// get_sample_array.

		test.seek(0);
		for(ix=0; ix<10; ix++) {
			const sample_int_t *tmp;
			tmp = test.get_int_sample();
			for(size_t jx=0; jx<channels; jx++) {
				if(tmp[jx] != in_buffer[ix*channels+jx]) {
					E.set_error_pfx("put_sample_array/get_int_sample");
					E.eprintf("%s: %i: Expected %i but got %i",
						  __FILE__, __LINE__, in_buffer[ix*channels+jx], tmp[jx]);
					
					throw E;
				}
			}
		}

		// Make sure that the extended part of an extended sample
		// is zero-initialized.
		test.set_size(20) ;
		assert(channels < 9);
		sample_int_t zeroes[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		for(ix=0; ix<10; ix++) {
			const sample_int_t *tmp;
			tmp = test.get_int_sample();
			if(memcmp(tmp, zeroes, sizeof(sample_int_t)*channels)) {
				E.set_error_pfx("set_size");
				E.eprintf("%s: %i: Buffer uninitialized at index %i: "
					  "Contains %i", __FILE__, __LINE__, ix+10, tmp);
				throw E;
			}
		}

		// Test put_sample and get_int_sample.

		test.seek(0);
		sample_int_t *temp_buffer = new sample_int_t[channels];
		for(ix=0; ix<60; ix++) {
			for(size_t jx=0; jx<channels; jx++) {
				temp_buffer[jx] = ix+jx;
			}
			test.put_sample(temp_buffer);
		}
		delete[] temp_buffer;
		test.seek(0);

		// Make sure we didn't have to tell Sample_Data to resize
		// the sample.

		status = test.get_size();

		if(status != 60) {
			E.set_error_pfx("put_sample");
			E.eprintf("%s: %i: put_sample failed to resize sample "
				  "to 60 samples (size=%i)", __FILE__, __LINE__,
				  status);
			throw E;
		}

		// Make sure get_int_sample gets the expected results.

		for(ix=0; ix<60; ix++) {
			const sample_int_t *tmp;
			tmp=test.get_int_sample();
			for(size_t jx=0; jx<channels; jx++) {
				if(tmp[jx] != (sample_int_t)(ix+jx)) {
					E.set_error_pfx("get_sample");
					E.eprintf("%s: %i: Expected %i got %i",
						  __FILE__, __LINE__, ix+jx, tmp[jx]);
					throw E;
				}
			}
		}

		// Ensure that truncate can resize a sample.

		test.seek(30);
		test.truncate(); 
		status = test.get_size();
		if(status != 30) {
			E.set_error_pfx("truncate");
			E.eprintf("%s: %i: Tried to truncate to 30 samples "
				  "but ended up with %i samples", __FILE__,
				  __LINE__, status);
			throw E;
		}

		// Ensure that truncating a sample doesn't corrupt it.

		test.seek(0);
		for(ix=0; ix<30; ix++) {
			const sample_int_t *tmp;
			tmp=test.get_int_sample();
			for(size_t jx=0; jx<channels; jx++) {
				if(tmp[jx] != (sample_int_t)(ix+jx)) {
					E.set_error_pfx("get_sample");
					E.eprintf("%s: %i: Expected %i got %i",
						  __FILE__, __LINE__, ix+jx, tmp);
					throw E;
				}
			}


		}

		// Check to see if we can write to the middle of a sample...

		test.seek(15);
		test.put_sample_array(in_buffer, 10);

		// Read back the results. We start at position 0 to
		// make sure that the write took place where it was expected.

		test.seek(0);
		test.get_sample_array(out_buffer, 20);

		for(ix=0; ix<15; ix++) {

			for(size_t jx=0; jx<channels; jx++) {
				if(out_buffer[ix*channels+jx] != (sample_int_t)(ix+jx)) {
					E.set_error_pfx("get_sample_array");
					E.eprintf("%s: %i: Expected %i got %i",
						  __FILE__, __LINE__, ix+jx, out_buffer[ix*channels+jx]);
					throw E;
				}
			}
		}

		for(ix=15*channels; ix<15*channels+5; ix++) {
			if(out_buffer[ix] != (sample_int_t)(ix-15*channels)) {
				E.set_error_pfx("get_sample_array");
				E.eprintf("%i: Expected %i got %i",
					  __LINE__, ix-15,
					 out_buffer[ix]);
				throw E;
			}
		}

		// Test that Linietsky's get_data_value functions work too.

		for(ix=0; ix<15; ix++) {

			const sample_int_t *buffer = test.get_data_value(ix);
			for(size_t jx=0; jx<channels; jx++) {
				if(buffer[jx] != (sample_int_t)(ix+jx)) {
					E.set_error_pfx("get_data_value");
					E.eprintf("%s: %i: Expected %i got %i",
						  __FILE__, __LINE__, ix+jx, buffer[jx]);
					throw E;
				}
			}
		}
		

		// Test that the data is preserved when set_num_channels() is used.
		//

		test.set_num_channels(channels+1);
		test.seek(0);
		for(ix=0; ix<15; ix++) {
			const sample_int_t *temp = test.get_int_sample();
			for(size_t jx=0; jx<channels; jx++) {
				if(temp[jx] != (sample_int_t)(ix+jx)) {
					E.set_error_pfx("set_num_channels");
					E.eprintf("%s:%i: Expected %i got %i",
						  __FILE__, __LINE__, ix+jx, temp[jx]);
					throw E;
				}
			}
			if(temp[channels] != 0) {
				E.set_error_pfx("set_num_channels");
				E.eprintf("%s:%i: Expected 0 got %i", __FILE__, __LINE__, temp[channels]);
				throw E;
			}
		}

		test.set_num_channels(channels);
		test.seek(0);
		for(ix=0; ix<15; ix++) {
			const sample_int_t *temp = test.get_int_sample();
			for(size_t jx=0; jx<channels; jx++) {
				if(temp[jx] != (sample_int_t)(ix+jx)) {
					E.set_error_pfx("set_num_channels");
					E.eprintf("%s:%i: Expected %i got %i",
						  __FILE__, __LINE__, ix+jx, temp[jx]);
					throw E;
				}
			}
		}

		// Check if the get/put_f_sample functions work.
		//

		test.seek(0);
		for(float fx=0.0; fx < 0.5; fx += 0.1)
		{
			float *buffer = new float[channels];
			for(size_t jx=0; jx<channels; jx++) {
				float fjx = jx;

				buffer[jx] = fx + 1.0/fjx;
			}
			test.put_f_sample(buffer);
			delete[] buffer;
		}

		// Read-back results.
		//

		test.seek(0);
		for(float fx=0.0; fx < 0.5; fx += 0.1)
		{
			float *buffer = new float[channels];
			test.get_f_sample(buffer);
			for(size_t jx=0; jx<channels; jx++) {
				float fjx = jx;
				float expected = BOUND((fx + 1.0/fjx), -1.0, 1.0);

				if(!APPROX(buffer[jx], expected)) {
					E.set_error_pfx("get_f_sample");
					E.eprintf("%s:%i: Expected %f got %f", __FILE__, __LINE__, expected, buffer[jx]);
					throw E;
				}
			}
			delete[] buffer;
		}

		// Verify that results are identical using Linietsky's functions.
		//

		for(float fx=0.0, ix=0; fx < 0.5; fx += 0.1, ix++)
		{
			float *buffer = new float[channels];
			test.get_sample(ix, buffer);
			for(size_t jx=0; jx<channels; jx++) {
				float fjx = jx;
				float expected = BOUND((fx + 1.0/fjx), -1.0, 1.0);

				if(!APPROX(buffer[jx], expected)) {
					E.set_error_pfx("get_sample");
					E.eprintf("%s:%i: Expected %f got %f", __FILE__, __LINE__, expected, buffer[jx]);
					throw E;
				}
			}
			delete[] buffer;
		}

	}

	// Check if samples can be copied and deleted without causing memory errors.
	//

	Sample_Data *samples = new Sample_Data[4];

	for(ix=0; ix<4; ix++) {
		samples[ix] = test;
	}

	delete[] samples;

	{
		Sample_Data values[4];
		for(ix=0; ix<4; ix++)
		{
			values[ix].set_size(300);
		}
	}
}

int main( int argc, char **argv )
{

	try {
		test_sample_data();
	} catch(Error E) {
		printf("%s\n", E.what());
		return 1;
	}
	printf("All tests passed\n");
	return 0;
}
#else

int main( int argc, char **argv )
{

	printf("Entering main() at %s:%d\n", __FILE__, __LINE__);

	try {
	#ifdef POSIX_ENABLED

		Mutex_Lock_Pthreads variables_lock;
		Mutex_Lock_Pthreads pattern_lock;
		Mutex_Lock_Pthreads driver_lock;
		Mutex_Lock_Pthreads info_lock;

		pthread_t player_thread;
		pthread_attr_t thread_attr;
	#endif



		driver_manager.set_variables_lock(&variables_lock);
		driver_manager.set_driver_lock(&driver_lock);
		Pattern::data_lock=&pattern_lock;

	//	driver_oss.link_to_mixer(&mixer);
	#ifdef OSS_ENABLED
		Sound_Driver_OSS driver_oss;
		driver_manager.register_driver(&driver_oss);
	#endif

	#ifdef RTAUDIO_ENABLED
		Sound_Driver_RtAudio driver_rtaudio;
		driver_manager.register_driver(&driver_rtaudio);
	#endif

	#ifdef JACK_ENABLED
		Sound_Driver_JACK driver_jack;
		driver_manager.register_driver(&driver_jack);
	#endif

		MIDI_Client_Manager midi_client_manager;
		midi_client_manager.set_app_name("CheeseTracker");

	#ifdef ALSA_ENABLED

		MIDI_Client_ALSA midi_client_alsa;
		midi_client_manager.add_midi_client(&midi_client_alsa);
	#endif

	#if ((CYGWIN_ENABLED) || (WIN32_ENABLED))

		Sound_Driver_WinMM driver_winmm;
		driver_manager.register_driver(&driver_winmm);
	#endif
		Sound_Driver_Dummy driver_dummy;

		driver_manager.register_driver(&driver_dummy);

		Resampler_Manager resampler_manager;
		Resampler_Raw resampler_raw;
		Resampler_Linear resampler_linear;
		Resampler_Cosine resampler_cosine(Resampler_Cosine::COSINE);
		Resampler_Cosine resampler_fm(Resampler_Cosine::FM);
		Resampler_Cubic resampler_cubic;
		Resampler_Dummy resampler_dummy;
		//resampler_manager.register_resampler(&resampler_dummy);
		resampler_manager.register_resampler(&resampler_raw);
		resampler_manager.register_resampler(&resampler_fm);
		resampler_manager.register_resampler(&resampler_cosine);
		resampler_manager.register_resampler(&resampler_linear);
		resampler_manager.register_resampler(&resampler_cubic);

		Effect_Source_Manager effect_source_manager_singleton;
			
		Effect_Source_Internal effect_source_internal;

		effect_source_manager_singleton.register_effect_source(&effect_source_internal);
		effect_source_internal.register_effect_constructor("Amplifier","INTERNAL: amplifier",&Effect_Amp::create_this,&Effect_Amp::create_parameters );	
		effect_source_internal.register_effect_constructor("Reverb","INTERNAL: freeverb",&Freeverb_Effect::create_this,&Freeverb_Effect::create_parameters );
		effect_source_internal.register_effect_constructor("Stereo Chorus","INTERNAL: stereo_chorus",&Chorus_Effect::create_this,&Chorus_Effect::create_parameters );
		effect_source_internal.register_effect_constructor("Echo","INTERNAL: echo",&Effect_Echo::create_this,&Effect_Echo::create_parameters );
		effect_source_internal.register_effect_constructor("Stereo FX","INTERNAL: stereo_fx",&Effect_Stereo_FX::create_this,&Effect_Stereo_FX::create_parameters );
		effect_source_internal.register_effect_constructor("6 Bands EQ","INTERNAL: eq_6_bands",&Effect_Equalizer::create_this,&Effect_Equalizer::create_parameters_6bands);
		effect_source_internal.register_effect_constructor("10 Bands EQ","INTERNAL: eq_10_bands",&Effect_Equalizer::create_this,&Effect_Equalizer::create_parameters_10bands);
		effect_source_internal.register_effect_constructor("21 Bands EQ","INTERNAL: eq_21_bands",&Effect_Equalizer::create_this,&Effect_Equalizer::create_parameters_21bands);
		effect_source_internal.register_effect_constructor("31 Bands EQ","INTERNAL: eq_31_bands",&Effect_Equalizer::create_this,&Effect_Equalizer::create_parameters_31bands);
		effect_source_internal.register_effect_constructor("Distortion","INTERNAL: distortion",&Effect_Distortion::create_this,&Effect_Distortion::create_parameters );
		effect_source_internal.register_effect_constructor("Pitch Shift","INTERNAL: pshift",&Effect_PShift::create_this,&Effect_PShift::create_parameters );
		
	#ifdef LADSPA_ENABLED
		LADSPA_Effect_Source effect_source_ladspa;
		effect_source_manager_singleton.register_effect_source(&effect_source_ladspa);

	#endif

		Keyboard_Input keyboard_input; //singleton instance here!

		QApplication a( argc, argv );

		MDI_Main_Window main_window;
		main_window.set_sound_driver_manager(&driver_manager);
		main_window.set_info_lock(&info_lock);

		a.setMainWidget( &main_window);

		string settings_file=Path_Data::get_home_dir()+DIR_SEPARATOR+(string)CONFIG_FILE;
		if (Path_Data::file_exists(settings_file)) {

			ConfigHandler config;
			config.set_header_check("CHEESETRACKER_CONFIG");
			config.load_from_text(settings_file);

			main_window.get_settings_window()->load_settings(&config);
		} else {

			printf("Warning, config file not found at %s. One will be created on exit!\n",settings_file.c_str());
			//should add something like "first time or stuff :)
			//initializing default driver since no config found!
			driver_manager.set_active_driver(0);
			driver_manager.reset_active_driver();
		}

		midi_client_manager.init_all_clients();
		//midi_client_alsa.init();

		main_window.show();

		//start the thread

		int priority=sched_get_priority_max(SCHED_RR);

		for (int i=1;i<argc;i++) {


			if ((string)argv[i]=="--help") {
				display_help();
				exit(255);
			} else if ((string)argv[i]=="--schedfifo") {
				printf("Warning, running in SCHEDFIFO mode!\n");
				priority=sched_get_priority_max(SCHED_FIFO);
			} else {

				main_window.open_song(argv[i]);
			}
		}

	#ifdef POSIX_ENABLED

		pthread_attr_init(&thread_attr);
		sched_param thread_sched_param;
		memset(&thread_sched_param,0,sizeof(thread_sched_param));
		thread_sched_param.sched_priority=priority;
		pthread_attr_setschedparam (&thread_attr,&thread_sched_param);
		pthread_create(&player_thread,&thread_attr,&audio_thread_cbk,NULL);
	#endif

		int result=a.exec();


		{
			ConfigHandler config;
			config.set_header_check("CHEESETRACKER_CONFIG");
			main_window.get_settings_window()->save_settings(&config);

			config.save_to_text(settings_file);
			printf("Saving settings to %s\n",settings_file.c_str());

		}

		//quit requested, cleaning up!

		quit_request=true;
		while(!thread_finished) {
			SLEEP_MSECS(1);
		}
		return result;
	} catch (Error E) {
		cerr << E.what() << std::endl;
	} catch (exception E) {
		cerr << E.what() << std::endl;
	}
	return -1;

}
#endif

