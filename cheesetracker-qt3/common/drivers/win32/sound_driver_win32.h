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
                             -------------------
    begin                : Sun Mar 18 2001
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
// Sound_Driver_Win32.h: interface for the Sound_Driver_Win32 class.
//
// by James Martin, adapted by Juan Linietsky for Saturno, used with permission.
//////////////////////////////////////////////////////////////////////

#ifndef SOUND_DRIVER_WIN32_H
#define SOUND_DRIVER_WIN32_H

#ifdef WIN32_ENABLED

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <dsound.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <process.h>


class Sound_Driver_Win32 : public Sound_Driver   {



         struct {
        	Uint16	HZ;	    // HZ of playback
        	Uint8	Bits;	    // Bits per sample, 8 or 16
        	Uint8	Stereo;     // TRUE if stereo, FALSE if not
        	} g_SoundFormat;    // Format of streaming sound

         LPDIRECTSOUNDBUFFER g_PrimaryBuffer;
        					// Primary sound buffer (not used
        					// directly)
         LPDIRECTSOUNDBUFFER g_SecondaryBuffer;
        					// Secondary sound buffer for mixing
        					// sound into
         Uint32 g_SecondaryBufferSize;	// Size of secondary buffer in bytes
         Uint32 g_MinimumMixBytes;	// MINMIXSIZE readjusted from ms to
        					// bytes
         Uint32 g_WritePosition;		// First byte not yet written to
         LPDIRECTSOUNDNOTIFY g_SecondaryBufferNotify;
        					// Notification object for secondary
        					// buffer

         HANDLE g_SoundNotify;	// Event to notify the sound thread
        					// that more mixing is required
         Uint8 g_SoundThreadExit; 	// FALSE = No exit requested
        					// TRUE = Exit requested
         Uint8 g_SoundThread;	// FALSE = Sound thread doesn't exist
        					// TRUE = Sound thread exists

         LPDIRECTSOUND g_lpDS = NULL;		// DirectSound object

         void * g_Context;




public:

	int get_mix_buffer_size();
	bool get_mix_16bits();
	bool get_mix_stereo();
	int get_mix_frequency();
	void request_buffer_size(int p_buffsize);
	void request_mix_16bits(bool p_mix_16bits);
	void request_mix_stereo(bool p_mix_stereo);
	void request_mix_frequency(int p_mix_frequency);
	bool update();
	bool finish();
	bool is_active();
	int init();
	Sound_Driver_Win32();
	virtual ~Sound_Driver_Win32();

	std::string get_name() { return "Win32 Sound Driver"; };

};

#endif // !defined(AFX_SOUND_DRIVER_WIN32_H__F680845D_6BDF_4937_971A_08E67695C5E2__INCLUDED_)
#endif