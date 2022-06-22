/*---------------------------------------------------------------------------------
    BASS_VFX 1.0 - Visual FX extension to the BASS Audio Library

    Visual Effects:
        Spectrum
        Specbands
        Spec3D
        Waveform
-----------------------------------------------------------------------------------
    filename   : waveform.cpp
    description: waveform visual effect
-----------------------------------------------------------------------------------
    website: https://www.jobnik.net/projects/bass_vfx

    forums : https://www.un4seen.com/forum/?board=1
             https://www.jobnik.net/forums

    e-mail : bass_vfx@jobnik.net
-----------------------------------------------------------------------------------
    Waveform VFX Copyright (c) 1999-2008, Ian Luck

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
#include "../bass_vfx_std.h"

//---------------------------------------------------------------------------------
// GLOBAL Variables & Macros
//---------------------------------------------------------------------------------
// Waveform
typedef struct {
	sharedVFX	svfx;
} WAVVFX;

//---------------------------------------------------------------------------------
// update the waveform display
//---------------------------------------------------------------------------------

#pragma optimize("t",on) // speed optimizations

static void CALLBACK __VFX_UpdateWaveform(UINT uTimerID, UINT uMsg, WAVVFX *wav, DWORD dw1, DWORD dw2)
{
	int x, y;
	int c;
	float *buf;

	BASS_ChannelLock(wav->svfx.svfxHandle, TRUE);

	buf = (float*)alloca(wav->svfx.svfxInfo.chans * wav->svfx.svfxShared.lWidth * sizeof(float)); // allocate buffer for data
	BASS_ChannelGetData(wav->svfx.svfxHandle, buf, (wav->svfx.svfxInfo.chans * wav->svfx.svfxShared.lWidth * sizeof(float)) | BASS_DATA_FLOAT); // get the sample data (floating-point to avoid 8 & 16 bit processing)

	// clear display
	memset(wav->svfx.svfxSpecBuf, wav->svfx.svfxShared.lBkGrndColor, wav->svfx.svfxShared.lWidth * wav->svfx.svfxShared.lHeight);

	for (c = 0; c < wav->svfx.svfxInfo.chans; c++) {
		for (x = 0; x < wav->svfx.svfxShared.lWidth; x++) {
			int v = (1 - buf[x * wav->svfx.svfxInfo.chans + c]) * wav->svfx.svfxShared.lHeight / 2; // invert and scale to fit display
			if (v < 0) v = 0;
			else if (v >= wav->svfx.svfxShared.lHeight) v = wav->svfx.svfxShared.lHeight - 1;
			if (!x) y = v;
			do { // draw line from previous sample...
				if (y < v) y++;
				else if (y > v) y--;
				wav->svfx.svfxSpecBuf[y * wav->svfx.svfxShared.lWidth + x] = 255 - (c * 50); // chans palette colors
			} while (y != v);
		}
	}

	// update the display
	BitBlt(wav->svfx.svfxDC, 0, 0, wav->svfx.svfxShared.lWidth, wav->svfx.svfxShared.lHeight, wav->svfx.svfxSpecDC, 0, 0, SRCCOPY);

	BASS_ChannelLock(wav->svfx.svfxHandle, FALSE);
}

#pragma optimize("",on) // restore optimizations

//---------------------------------------------------------------------------------
// remove chosen visual fx
//---------------------------------------------------------------------------------
static void WINAPI VFX_Free(void *inst)
{
	WAVVFX *wav=(WAVVFX*)inst;
	sharedRemove(&wav->svfx, wav);
}
//---------------------------------------------------------------------------------
// get parameters
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_GetParameters(void *inst, void *param)
{
	WAVVFX *wavc=(WAVVFX*)inst;
	BASS_VFXWAVEFORM *wav = (BASS_VFXWAVEFORM*)param;

	return sharedGetParams(&wav->shared, &wavc->svfx);
}
//---------------------------------------------------------------------------------
// set parameters
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_SetParameters(void *inst, const void *param)
{
	WAVVFX *wavc=(WAVVFX*)inst;
	BASS_VFXWAVEFORM *wav = (BASS_VFXWAVEFORM*)param;

	return sharedSetParams(&wavc->svfx, &wav->shared, 0);
}
//---------------------------------------------------------------------------------
// reset VFX state/buffers
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_Reset(void *inst)
{
	WAVVFX *wavc=(WAVVFX*)inst;
	return sharedReset(&wavc->svfx);
}
//---------------------------------------------------------------------------------
// set chosen visual fx
//---------------------------------------------------------------------------------

// FX function table
static ADDON_FUNCTIONS_FX VFXfuncs={VFX_Free, VFX_SetParameters, VFX_GetParameters, VFX_Reset};

HVFX SetVFX_Waveform(DWORD chan, HWND hWnd, BASS_CHANNELINFO *info)
{
	WAVVFX *wav=(WAVVFX*) malloc(sizeof(WAVVFX));
	memset(wav, 0, sizeof(WAVVFX));

	wav->svfx.svfxInfo = *info;
	return sharedSetVFX(&wav->svfx, chan, hWnd, (LPTIMECALLBACK)&__VFX_UpdateWaveform, wav, &VFXfuncs);
}
//-[EOF - waveform.cpp ]-----------------------------------------------------------
