/*---------------------------------------------------------------------------------
    BASS_VFX 1.0 - Visual FX extension to the BASS Audio Library

    Visual Effects:
        Spectrum
        Specbands
        Spec3D
        Waveform
-----------------------------------------------------------------------------------
    filename   : bass_vfx_std.h
-----------------------------------------------------------------------------------
    website: https://www.jobnik.net/projects/bass_vfx

    forums : https://www.un4seen.com/forum/?board=1
             https://www.jobnik.net/forums

    e-mail : bass_vfx@jobnik.net
-----------------------------------------------------------------------------------
    Copyright (c) 2008, (: JOBnik! :) [Arthur Aminov, ISRAEL]
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------------*/

//---------------------------------------------------------------------------------
// I N C L U D E
//---------------------------------------------------------------------------------
#if _MSC_VER > 1000
	#pragma once
#endif

#define WIN32_LEAN_AND_MEAN				// Exclude rarely-used stuff from Windows headers
#define TIME_KILL_SYNCHRONOUS 0x0100	// Passing this flag should prevent an event from occurring after the timeKillEvent() function is called.

#include <windows.h>
#include <Mmsystem.h>
#include <malloc.h>
#include <math.h>

#ifdef _DEBUG 
	#include <crtdbg.h> 
#endif 

#include "bass.h"
#include "bass-addon.h"
#include "bass_vfx.h"
//---------------------------------------------------------------------------------
// SetError macros
//---------------------------------------------------------------------------------
#define noerror() return (bassfunc->SetError(BASS_OK),TRUE)
#define noerrorn(n) return (bassfunc->SetError(BASS_OK),n)
#define error(n) return (bassfunc->SetError(n),FALSE) // error = 0/NULL
#define errorn(n) return (bassfunc->SetError(n),-1) // error = -1
//---------------------------------------------------------------------------------
// shared struct
//---------------------------------------------------------------------------------
typedef struct {
	BASS_VFXSHARED svfxShared;		// shared: hWnd, height, width...

	// display
	HDC     svfxSpecDC;
	HBITMAP svfxSpecBMP;
	BYTE    *svfxSpecBuf;
	HDC		svfxDC;					// DC display

	// channel
	DWORD				svfxHandle;	// channel handle
	BASS_CHANNELINFO	svfxInfo;	// channel info

	// fft
	int		svfxSizeFFT;			// 128/256/512/1024/2048/4096
	DWORD	svfxBASS_DATA_FFT;		// BASS_DATA_FFT256..BASS_DATA_FFT8192

	// timer
	MMRESULT		svfxHTimer;		// timer handle
	LPTIMECALLBACK	svfxTimerLPTC;	// callback function
	DWORD			svfxTimerUser;
} sharedVFX;
//---------------------------------------------------------------------------------
// shared functions
//---------------------------------------------------------------------------------
void sharedRemove(sharedVFX *svfx, void *inst);
BOOL sharedReset(sharedVFX *svfx);
BOOL sharedGetParams(BASS_VFXSHARED *shared, const sharedVFX *svfx);
BOOL sharedSetParams(sharedVFX *svfx, const BASS_VFXSHARED *shared, int fftSize);
HVFX sharedSetVFX(sharedVFX *svfx, DWORD chan, HWND hWnd, LPTIMECALLBACK timerLPTC, void *inst, const ADDON_FUNCTIONS_FX *funcs);

void setupBitmap(sharedVFX *svfx);
long getWindowWidth(HWND hWnd);
long getWindowHeight(HWND hWnd);
//-[EOF - bass_vfx_std.h]----------------------------------------------------------
