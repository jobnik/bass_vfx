/*---------------------------------------------------------------------------------
    BASS_VFX 1.0 - Visual FX extension to the BASS Audio Library

    Visual Effects:
        Spectrum
        Specbands
        Spec3D
        Waveform
-----------------------------------------------------------------------------------
    filename   : specbands.cpp
    description: spectrum bands visual effect
-----------------------------------------------------------------------------------
    website: https://www.jobnik.net/projects/bass_vfx

    forums : https://www.un4seen.com/forum/?board=1
             https://www.jobnik.net/forums

    e-mail : bass_vfx@jobnik.net
-----------------------------------------------------------------------------------
    Specbands VFX Copyright (c) 1999-2008, Ian Luck

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
// Specbands
typedef struct {
	int			bndBands;
	sharedVFX	svfx;
} BNDVFX;

//---------------------------------------------------------------------------------
// update the spectrum (logarithmic, acumulate & average bins) display
//---------------------------------------------------------------------------------

#pragma optimize("t",on) // speed optimizations

static void CALLBACK __VFX_UpdateSpecbands(UINT uTimerID, UINT uMsg, BNDVFX *bnd, DWORD dw1, DWORD dw2)
{
	int x, y;
	int b0 = 0;
	float *fft;

	BASS_ChannelLock(bnd->svfx.svfxHandle, TRUE);

	// get the FFT data
	fft = (float*)alloca(bnd->svfx.svfxSizeFFT * sizeof(float)); // allocate buffer for data
	BASS_ChannelGetData(bnd->svfx.svfxHandle, fft, bnd->svfx.svfxBASS_DATA_FFT);

	// clear display
	memset(bnd->svfx.svfxSpecBuf, bnd->svfx.svfxShared.lBkGrndColor, bnd->svfx.svfxShared.lWidth * bnd->svfx.svfxShared.lHeight);

	for (x = 0; x < bnd->bndBands; x++) {
		float sum = 0;
		int sc, b1 = pow(2, x * 10.0 / (bnd->bndBands - 1));
		if (b1 > bnd->svfx.svfxSizeFFT - 1) b1 = bnd->svfx.svfxSizeFFT - 1;
		if (b1 <= b0) b1 = b0 + 1; // make sure it uses at least 1 FFT bin
		sc = 10 + b1 - b0;
		for (;b0 < b1; b0++) sum += fft[1 + b0];
		y = (sqrt(sum / log10(sc)) * 1.7 * bnd->svfx.svfxShared.lHeight) - 4; // scale it
		if (y > bnd->svfx.svfxShared.lHeight) y = bnd->svfx.svfxShared.lHeight; // cap it
		while (--y >= 0)
			memset(bnd->svfx.svfxSpecBuf + y * bnd->svfx.svfxShared.lWidth + x * (bnd->svfx.svfxShared.lWidth / bnd->bndBands), 1 + (y * 127 / bnd->svfx.svfxShared.lHeight), bnd->svfx.svfxShared.lWidth / bnd->bndBands - 2); // draw bar
	}

	// update the display
	BitBlt(bnd->svfx.svfxDC, 0, 0, bnd->svfx.svfxShared.lWidth, bnd->svfx.svfxShared.lHeight, bnd->svfx.svfxSpecDC, 0, 0, SRCCOPY);

	BASS_ChannelLock(bnd->svfx.svfxHandle, FALSE);
}

#pragma optimize("",on) // restore optimizations

//---------------------------------------------------------------------------------
// remove chosen visual fx
//---------------------------------------------------------------------------------
static void WINAPI VFX_Free(void *inst)
{
	BNDVFX *bnd=(BNDVFX*)inst;
	sharedRemove(&bnd->svfx, bnd);
}
//---------------------------------------------------------------------------------
// get parameters
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_GetParameters(void *inst, void *param)
{
	BNDVFX *bndc=(BNDVFX*)inst;
	BASS_VFXSPECBANDS *bnd = (BASS_VFXSPECBANDS*)param;

	bnd->lBands = bndc->bndBands;
	bnd->lFFTsize = bndc->svfx.svfxSizeFFT;

	return sharedGetParams(&bnd->shared, &bndc->svfx);
}
//---------------------------------------------------------------------------------
// set parameters
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_SetParameters(void *inst, const void *param)
{
	BNDVFX *bndc=(BNDVFX*)inst;
	BASS_VFXSPECBANDS *bnd = (BASS_VFXSPECBANDS*)param;

	if (bnd->lBands < 1 || bnd->lFFTsize<128 || bnd->lFFTsize>4096)
		error(BASS_ERROR_ILLPARAM);

	bndc->bndBands = bnd->lBands;
	return sharedSetParams(&bndc->svfx, &bnd->shared, bnd->lFFTsize);
}
//---------------------------------------------------------------------------------
// reset VFX state/buffers
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_Reset(void *inst)
{
	BNDVFX *bndc=(BNDVFX*)inst;
	return sharedReset(&bndc->svfx);
}
//---------------------------------------------------------------------------------
// set chosen visual fx
//---------------------------------------------------------------------------------

// FX function table
static ADDON_FUNCTIONS_FX VFXfuncs={VFX_Free, VFX_SetParameters, VFX_GetParameters, VFX_Reset};

HVFX SetVFX_Specbands(DWORD chan, HWND hWnd, BASS_CHANNELINFO *info)
{
	BNDVFX *bnd=(BNDVFX*) malloc(sizeof(BNDVFX));
	memset(bnd, 0, sizeof(BNDVFX));

	bnd->bndBands = 28;
	return sharedSetVFX(&bnd->svfx, chan, hWnd, (LPTIMECALLBACK)&__VFX_UpdateSpecbands, bnd, &VFXfuncs);
}
//-[EOF - specbands.cpp]-----------------------------------------------------------
