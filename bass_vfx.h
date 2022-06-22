/*---------------------------------------------------------------------------------
    BASS_VFX 1.0 - Visual FX extension to the BASS Audio Library

    Visual Effects:
        Spectrum
        Specbands
        Spec3D
        Waveform
-----------------------------------------------------------------------------------
    filename   : bass_vfx.h
    description: main header file
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

#ifndef BASS_VFX_H
#define BASS_VFX_H

#ifdef __cplusplus
  extern "C" {
#endif

#define BASS_VFXVERSION		0x100	// API version
#define BASS_VFXVERSIONTEXT	"1.0"

#ifndef BASS_VFXDEF
  #define BASS_VFXDEF(f) WINAPI f
#endif

//---------------------------------------------------------------------------------
// vfx handle
//---------------------------------------------------------------------------------
typedef DWORD HVFX;

//---------------------------------------------------------------------------------
// bass_vfx version
//---------------------------------------------------------------------------------
DWORD BASS_VFXDEF(BASS_VFX_GetVersion)();
//	RETURN : For example, 0x01020304 (hex), would be version 1.2.3.4

//---------------------------------------------------------------------------------
//	V F X (Visual Effects)
//---------------------------------------------------------------------------------

enum {
	BASS_VFX_SPECTRUM,
	BASS_VFX_SPECBANDS,
	BASS_VFX_SPEC3D,
	BASS_VFX_WAVEFORM
};

//---------------------------------------------------------------------------------
// shared parameters for all visual effects
//---------------------------------------------------------------------------------
typedef struct {
	HWND hWnd;						// window handle
	int  lWidth;					// window width
	int  lHeight;					// window height
	UINT uTimerDelay;				// timer delay
	UINT uTimerResolution;			// timer resolution
	int  lBkGrndColor;				// display background color
} BASS_VFXSHARED;

//---------------------------------------------------------------------------------
// Spectrum
//---------------------------------------------------------------------------------
#define BASS_VFX_SPECTRUM_SCALE_SQRT	0
#define BASS_VFX_SPECTRUM_SCALE_LINEAR	1

typedef struct {
	BASS_VFXSHARED shared;
	int  lScale;					// BASS_VFX_SPECTRUM_SCALE_xxx
	int  lFFTsize;					// 128/256/512/1024/2048/4096
	BOOL lCombineBins;				// combine multiple bins
} BASS_VFXSPECTRUM;

//---------------------------------------------------------------------------------
// Specbands
//---------------------------------------------------------------------------------
typedef struct {
	BASS_VFXSHARED shared;
	int  lBands;					// number of bands
	int  lFFTsize;					// 128/256/512/1024/2048/4096
} BASS_VFXSPECBANDS;

//---------------------------------------------------------------------------------
// Spec3D
//---------------------------------------------------------------------------------
typedef struct {
	BASS_VFXSHARED shared;
	int  lMarkerPos;				// marker position
	int  lFFTsize;					// 128/256/512/1024/2048/4096
	BOOL lCombineBins;				// combine multiple bins
} BASS_VFXSPEC3D;

//---------------------------------------------------------------------------------
// Waveform
//---------------------------------------------------------------------------------
typedef struct {
	BASS_VFXSHARED shared;
} BASS_VFXWAVEFORM;

//---------------------------------------------------------------------------------
// functions
//---------------------------------------------------------------------------------
HVFX BASS_VFXDEF(BASS_VFX_Set)(DWORD handle, HWND hWnd, int vfx);
BOOL BASS_VFXDEF(BASS_VFX_Remove)(DWORD handle, HVFX vfx);
BOOL BASS_VFXDEF(BASS_VFX_SetParameters)(HVFX handle, const void *params);
BOOL BASS_VFXDEF(BASS_VFX_GetParameters)(HVFX handle, void *params);
BOOL BASS_VFXDEF(BASS_VFX_Reset)(HVFX handle);

#ifdef __cplusplus
}
#endif

#endif
