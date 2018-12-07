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

// Sound_Driver_Win32.cpp: implementation of the Sound_Driver_Win32 class.
//
//////////////////////////////////////////////////////////////////////
#include "sound_driver_win32.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#ifdef WIN32_ENABLED

Sound_Driver_Win32::Sound_Driver_Win32()
{

	// АБВллллллллллллл DESCRIPTION лллллллллллллВБА

	// This initializes Direct Sound if it isn't already initialized.  If
	// this routine fails it returns TRUE, or FALSE if succeeded.

	// АБВллллллллллллл LOCAL VARIABLES лллллллллллллВБА

	HRESULT DSReturnValue;
	HWND hWnd;

	// АБВллллллллллллл CODE лллллллллллллВБА

	if ( g_lpDS ) { return FALSE; }

	DSReturnValue = DirectSoundCreate (
		NULL,
		&g_lpDS,
		NULL
		);

	if ( DSReturnValue != DS_OK ) {

	    g_lpDS = NULL;
	    return TRUE;
	}

	hWnd = GetForegroundWindow();

	if ( hWnd == NULL ) {

	    hWnd = GetDesktopWindow();
	}

	DSReturnValue = g_lpDS->lpVtbl->SetCooperativeLevel (
		g_lpDS,
		hWnd,
		DSSCL_PRIORITY
		);

	if ( DSReturnValue != DS_OK ) {

	    g_lpDS->lpVtbl->Release ( g_lpDS );
	    g_lpDS = NULL;
	}




	g_PrimaryBuffer=NULL;
	g_SecondaryBuffer=NULL;
	g_SoundNotify=NULL;
        g_SoundThread=FALSE;
	driver_ready = false;
	
	return FALSE;	
}

Sound_Driver_Win32::~Sound_Driver_Win32()
{

}

int Sound_Driver_Win32::init() {

// DESCRIPTION


	// LOCAL VARIABLES

/* typedef struct waveformat_extended_tag {

    WORD  wFormatTag;
    WORD  nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD  nBlockAlign;
    WORD  wBitsPerSample;
    WORD  cbSize;
} WAVEFORMATEX;
*/
	HRESULT DSReturnValue;
	
	static WAVEFORMATEX DesiredWaveFormat = {
		WAVE_FORMAT_PCM,
		(mix_stereo?2:1),
		mix_frequency,
		mix_frequency * (mix_stereo?4:2),
		(mix_stereo?4:2),
		(mix_16bits?16:8),
		0
	};
	
	static WAVEFORMATEX WaveFormat;
	DSBUFFERDESC BufferDesc;


	Uint8 * Buffer1;	// First part of circular buffer when locking
	Uint32 Buffer1Size;
	Uint8 * Buffer2;	// Second part of circular buffer when locking
	Uint32 Buffer2Size;

	DSBPOSITIONNOTIFY NotifyPositions[2];

	// АБВллллллллллллл CODE лллллллллллллВБА

	g_Context = Context;

	g_StreamFunction = StreamFunction;

	if ( InitDirectSound () ) {

	    ERROR ("Failed to initialize DirectSound.");
	    finish();
	    goto Failure;
	}

	// ллл Create primary sound buffer and set its desired format. ллл

	memset(BufferDesc,0, sizeof ( BufferDesc ));
	
	BufferDesc.dwSize = sizeof ( BufferDesc );
	BufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	DSReturnValue = g_lpDS->lpVtbl->CreateSoundBuffer (
		g_lpDS,
		&BufferDesc,
		&g_PrimaryBuffer,
		NULL
		);

	if ( DSReturnValue != DS_OK ) {

	    ERROR( "Cannot create primary sound buffer.");
	    finish();
	    goto Failure;
	}

	g_PrimaryBuffer->lpVtbl->SetFormat (
		g_PrimaryBuffer,
		&DesiredWaveFormat
		);

	// ллл Get the actual format of the primary buffer. ллл

	g_PrimaryBuffer->lpVtbl->GetFormat (
		g_PrimaryBuffer,
		&WaveFormat,
		sizeof ( WaveFormat ),
		NULL
		);

	mix_frequency = WaveFormat.nSamplesPerSec;
	mix_16bits = (WaveFormat.wBitsPerSample==16);
	mix_stereo = (WaveFormat.nChannels==2);


	// ллл Determine size of secondary buffer and create it. ллл

	// Get amount of samples in SOUNDBLOCKSIZE milliseconds

	g_SecondaryBufferSize = mix_frequency;
	g_SecondaryBufferSize *= SOUNDBLOCKSIZE;
	g_SecondaryBufferSize /= 1000;

	// Round the samples up to be divisible by 2

	g_SecondaryBufferSize += ( g_SecondaryBufferSize & 1 );

	// Convert samples to bytes

	g_SecondaryBufferSize *= WaveFormat.nChannels;
	g_SecondaryBufferSize *= mix_16its?2:1;

	// Create secondary buffer


	memset(BufferDesc,0, sizeof ( BufferDesc ));
		
	BufferDesc.dwSize = sizeof ( BufferDesc );
	BufferDesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY;
	BufferDesc.dwBufferBytes = g_SecondaryBufferSize;
	BufferDesc.lpwfxFormat = &WaveFormat;

	DSReturnValue = g_lpDS->lpVtbl->CreateSoundBuffer (
		g_lpDS,
		&BufferDesc,
		&g_SecondaryBuffer,
		NULL
		);

	if ( DSReturnValue != DS_OK ) {

	    ERROR( "Cannot create secondary sound buffer.");
	    finish();
	    goto Failure;
	}


	// ллл Determine minimum size to mix. ллл

	// Get amount of samples in MINMIXSIZE milliseconds

	g_MinimumMixBytes = mix_frequency;
	g_MinimumMixBytes *= MINMIXSIZE;
	g_MinimumMixBytes /= 1000;

	// Convert samples to bytes

	g_MinimumMixBytes *= WaveFormat.nChannels;
	g_MinimumMixBytes *= (mix_16bits?2:1);


	// ллл Create sound thread and notification event. ллл

	g_SoundNotify = CreateEvent (
		NULL,
		FALSE,
		FALSE,
		NULL
		);

	if ( g_SoundNotify == NULL ) {

	    ERROR ( "Cannot create sound event.");
	    finish();
	    goto Failure;
	}

	g_SoundThreadExit = FALSE;
	g_WritePosition = 0;

	if ( !_beginthread (
		(void *)SoundThreadEntryPoint,
		4096,
		NULL
		)
	    ) {

	    ERROR ( "Cannot create sound thread.");
	    finish();
	    goto Failure;
	}

	g_SoundThread = TRUE;

	// ллл Set notification positions in the buffer at 0% and 50%. ллл

	DSReturnValue = g_SecondaryBuffer->lpVtbl->QueryInterface (
		g_SecondaryBuffer,
		&IID_IDirectSoundNotify,
		&g_SecondaryBufferNotify
		);

	if ( DSReturnValue != DS_OK ) {

	    ERROR ( "Cannot achieve SoundNotify interface.");
	    finish();	
	    goto Failure;
	}

	NotifyPositions[0].dwOffset = g_SecondaryBufferSize / 2;
	NotifyPositions[0].hEventNotify = g_SoundNotify;
	NotifyPositions[1].dwOffset = 0;
	NotifyPositions[1].hEventNotify = g_SoundNotify;

	DSReturnValue = g_SecondaryBufferNotify->lpVtbl->SetNotificationPositions (
		g_SecondaryBufferNotify,
		2,
		NotifyPositions
		);

	if ( DSReturnValue != DS_OK ) {

	    finish();
	    ERROR ( "Could not set notification positions.");
	    goto Failure;
	}


	// ллл Fill the secondary buffer with zeros and play it. ллл

	DSReturnValue = g_SecondaryBuffer->lpVtbl->Lock (
		g_SecondaryBuffer,
		0,
		g_SecondaryBufferSize,
		&Buffer1,
		&Buffer1Size,
		&Buffer2,
		&Buffer2Size,
		DSBLOCK_ENTIREBUFFER
		);

	if ( DSReturnValue == DSERR_BUFFERLOST ) {

	    DSReturnValue = g_SecondaryBuffer->lpVtbl->Restore (
		    g_SecondaryBuffer
		    );

	    if ( DSReturnValue != DS_OK ) {

		finish();
		ERROR ( "Can't lock secondary buffer.");
		goto Failure;
	    }

	    DSReturnValue = g_SecondaryBuffer->lpVtbl->Lock (
		    g_SecondaryBuffer,
		    0,
		    g_SecondaryBufferSize,
		    &Buffer1,
		    &Buffer1Size,
		    &Buffer2,
		    &Buffer2Size,
		    DSBLOCK_ENTIREBUFFER
		    );

	}

	if ( DSReturnValue != DS_OK ) {

	    finish();
	    ERROR("Can't lock secondary buffer.");
	    goto Failure;
	}

	if ( Buffer1 ) {

	    memset (
		    Buffer1,
		    0,
		    Buffer1Size
		    );
	}

	if ( Buffer2 ) {

	    memset (
		    Buffer1,
		    0,
		    Buffer1Size
		    );
	}

	g_SecondaryBuffer->lpVtbl->Unlock (
		g_SecondaryBuffer,
		Buffer1,
		Buffer1Size,
		Buffer2,
		Buffer2Size
		);

	g_WritePosition = 0;

	DSReturnValue = g_SecondaryBuffer->lpVtbl->Play (
		g_SecondaryBuffer,
		0,
		0,
		DSBPLAY_LOOPING
		);

	if ( DSReturnValue == DSERR_BUFFERLOST ) {

	    DSReturnValue = g_SecondaryBuffer->lpVtbl->Restore (
		    g_SecondaryBuffer
		    );

	    if ( DSReturnValue != DS_OK ) {

		finish();
		ERROR( "Can't play secondary buffer.");
		goto Failure;
	    }

	    DSReturnValue = g_SecondaryBuffer->lpVtbl->Play (
		    g_SecondaryBuffer,
		    0,
		    0,
		    DSBPLAY_LOOPING
		    );
	}

	if ( DSReturnValue != DS_OK ) {

	    finish();
	    ERROR( "Can't play secondary buffer.");
	    goto Failure;
	}

	driver_ready=true;

	return FALSE;

	Failure:


	g_SilentOutput = TRUE;

	return FALSE;
}



bool Sound_Driver_Win32::is_active() {

	return driver_ready;
}

bool Sound_Driver_Win32::finish() {

	driver_ready=false;

	if ( g_SecondaryBuffer ) {

	    g_SecondaryBuffer->lpVtbl->Stop (
		    g_SecondaryBuffer
		    );
	}

	if ( g_SecondaryBufferNotify ) {

	    g_SecondaryBufferNotify->lpVtbl->Release (
		    g_SecondaryBufferNotify
		    );
	    g_SecondaryBufferNotify = NULL;
	}

	if ( g_SecondaryBuffer ) {

	    g_SecondaryBuffer->lpVtbl->Release (
		    g_SecondaryBuffer
		    );
	    g_SecondaryBuffer = NULL;
	}

	if ( g_PrimaryBuffer ) {

	    g_PrimaryBuffer->lpVtbl->Release ( g_PrimaryBuffer );
	    g_PrimaryBuffer = NULL;
	}

	if ( g_SoundThread ) {

	    g_SoundThreadExit = TRUE;
	    SetEvent ( g_SoundNotify );

	} else {

	    if ( g_SoundNotify ) {

		CloseHandle ( g_SoundNotify );
		g_SoundNotify = NULL;
	    }
	}



}

bool Sound_Driver_Win32::update() {

	HRESULT DSReturnValue;
	Uint32 PlayCursor;
	Uint32 WriteBytes;
	Uint32 Count;

	Uint8 * Buffer1;	// First part of circular buffer when locking
	Uint32 Buffer1Size;
	Uint8 * Buffer2;	// Second part of circular buffer when locking
	Uint32 Buffer2Size;

	static Uint8 Reentrant;       // Nonzero if in progress

	// АБВллллллллллллл CODE лллллллллллллВБА

	Reentrant++;

	if ( Reentrant > 1 ) { Reentrant--; return; }

	// Find out how many more bytes to mix

	if ( g_SecondaryBuffer == NULL ) {

	    Reentrant--;
	    return;
	}

	DSReturnValue = g_SecondaryBuffer->lpVtbl->GetCurrentPosition (
		g_SecondaryBuffer,
		&PlayCursor,
		NULL
		);

	if ( DSReturnValue == DSERR_BUFFERLOST ) {

	    DSReturnValue = g_SecondaryBuffer->lpVtbl->Restore (
		    g_SecondaryBuffer
		    );

	    if ( DSReturnValue != DS_OK ) {

		ERROR ( "Can't get position of secondary buffer in UpdateSoundBuffer.");
		Reentrant--;
		return;
	    }

	    DSReturnValue = g_SecondaryBuffer->lpVtbl->GetCurrentPosition (
		    g_SecondaryBuffer,
		    &PlayCursor,
		    NULL
		    );

	}

	if ( DSReturnValue != DS_OK ) {

	    ERROR ( "Can't get position of secondary buffer in UpdateSoundBuffer.");
	    Reentrant--;
	    return;
	}


	if ( PlayCursor < g_WritePosition ) {

	    PlayCursor += g_SecondaryBufferSize;
	}

	WriteBytes = PlayCursor - g_WritePosition;

	for ( Count = 2 ; Count ; Count-- ) {

	    if ( WriteBytes < g_MinimumMixBytes ) {

		Reentrant--;
		return;
	    }

	    DSReturnValue = g_SecondaryBuffer->lpVtbl->Lock (
		    g_SecondaryBuffer,
		    g_WritePosition,
		    WriteBytes,
		    &Buffer1,
		    &Buffer1Size,
		    &Buffer2,
		    &Buffer2Size,
		    0
		    );

	    if ( DSReturnValue == DSERR_BUFFERLOST ) {

		DSReturnValue = g_SecondaryBuffer->lpVtbl->Restore (
			g_SecondaryBuffer
			);

		if ( DSReturnValue != DS_OK ) {

		    ERROR ( "Can't lock secondary buffer in UpdateSoundBuffer.");
		    Reentrant--;
		    return;
		}

		DSReturnValue = g_SecondaryBuffer->lpVtbl->Lock (
			g_SecondaryBuffer,
			g_WritePosition,
			WriteBytes,
			&Buffer1,
			&Buffer1Size,
			&Buffer2,
			&Buffer2Size,
			0
			);
	    }

	    if ( DSReturnValue != DS_OK ) {

		WriteBytes *= 181;	// Try again with the buffer .5^.5
		WriteBytes >>= 8;	// times the size.
		WriteBytes &= ~3;	// Make sure it is aligned on sample
					// boundary
		ERROR ( "Can't lock, trying again");
		continue;
	    }

	    if ( Buffer1 ) {

	
	    	mixer->writebytes(Buffer1,Buffer1Size); 	

	    }

	    if ( Buffer2 ) {

	    	mixer->writebytes(Buffer2,Buffer1Size); 	
	

	    }

	    g_WritePosition += WriteBytes;
	    g_WritePosition %= g_SecondaryBufferSize;

	    g_SecondaryBuffer->lpVtbl->Unlock (
		    g_SecondaryBuffer,
		    Buffer1,
		    Buffer1Size,
		    Buffer2,
		    Buffer2Size
		    );

	    Reentrant--;
	    return;

	}

	ERROR ("Couldn't lock.");

	Reentrant--;

}

void Sound_Driver_Win32::request_mix_frequency(int p_mix_frequency)

{
	mix_frequency=p_mix_frequency;
}


void Sound_Driver_Win32::request_mix_stereo(bool p_mix_stereo)
{
	mix_stereo=p_mix_stereo;
}


void Sound_Driver_Win32::request_mix_16bits(bool p_mix_16bits)
{
	mix_16bits=p_mix_16bits;
}

void Sound_Driver_Win32::request_buffer_size(int p_buffsize)
{
	mix_buffersize=p_buffsize;
}

int Sound_Driver_Win32::get_mix_frequency()
{
	return mix_frequency;
}

bool Sound_Driver_Win32::get_mix_stereo()
{
	return mix_stereo;
}

bool Sound_Driver_Win32::get_mix_16bits()
{
	return mix_16bits;
}

int Sound_Driver_Win32::get_mix_buffer_size()
{
	return mix_buffersize;
}

BOOL Sound_Driver_Win32::WIN_IsThere()
{
	return waveOutGetNumDevs()>0?1:0;
}

void Sound_Driver_Win32::WIN_CallBack(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	if (uMsg==WOM_DONE) --buffersout;
}


WORD Sound_Driver_Win32::buffersout;


#endif