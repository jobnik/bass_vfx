/*---------------------------------------------------------------------------------
    BASS_VFX 1.0 - Visual FX extension to the BASS Audio Library

    Visual Effects:
        Spectrum
        Specbands
        Spec3D
        Waveform
-----------------------------------------------------------------------------------
    filename   : spectrum.cpp
    description: spectrum visual effect
-----------------------------------------------------------------------------------
    website: https://www.jobnik.net/projects/bass_vfx

    forums : https://www.un4seen.com/forum/?board=1
             https://www.jobnik.net/forums

    e-mail : bass_vfx@jobnik.net
-----------------------------------------------------------------------------------
    Spectrum VFX Copyright (c) 1999-2008, Ian Luck

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
// Spectrum
typedef struct {
	int			spcScale;
	BOOL		spcCombineBins;
	sharedVFX	svfx;
} SPCVFX;

//---------------------------------------------------------------------------------
// update the spectrum ("normal" FFT) display - the interesting bit :)
//---------------------------------------------------------------------------------

#pragma optimize("t",on) // speed optimizations

void CALLBACK __VFX_UpdateSpectrum(UINT uTimerID, UINT uMsg, SPCVFX *spc, DWORD dw1, DWORD dw2)
{
	int x, y, y1;
	int x2 = 0; // current bin marker
	float *fft;

	BASS_ChannelLock(spc->svfx.svfxHandle, TRUE);

	// get the FFT data
	fft = (float*)alloca(spc->svfx.svfxSizeFFT * sizeof(float)); // allocate buffer for data
	BASS_ChannelGetData(spc->svfx.svfxHandle, fft, spc->svfx.svfxBASS_DATA_FFT);

	// clear display
	memset(spc->svfx.svfxSpecBuf, spc->svfx.svfxShared.lBkGrndColor, spc->svfx.svfxShared.lWidth * spc->svfx.svfxShared.lHeight);

	for (x = 0; x < spc->svfx.svfxShared.lWidth / 2; x++) {
		float y2;
		if(spc->spcCombineBins){
			y2 = fft[1 + x2];
			while (x2 < (x + 1) * (spc->svfx.svfxSizeFFT - 1) / spc->svfx.svfxShared.lWidth) { // combine multiple bins if necessary...
				x2++;
				if (y2 < fft[1 + x2]) y2 = fft[1 + x2];
			}
		} else
			y2 = fft[1 + x];

		if (spc->spcScale == BASS_VFX_SPECTRUM_SCALE_SQRT)
			y = sqrt(y2) * 3 * spc->svfx.svfxShared.lHeight - 4; // scale it (sqrt to make low values more visible)
		else
			y = y2 * 10 * spc->svfx.svfxShared.lHeight; // scale it (linearly)

		if (y > spc->svfx.svfxShared.lHeight) y = spc->svfx.svfxShared.lHeight; // cap it
		if (x && (y1 = (y + y1) / 2)) // interpolate from previous to make the display smoother
			while (--y1 >= 0) spc->svfx.svfxSpecBuf[y1 * spc->svfx.svfxShared.lWidth + x * 2 - 1] = 1 + (y1 * 127 / spc->svfx.svfxShared.lHeight);
		y1 = y;
		while (--y >= 0) spc->svfx.svfxSpecBuf[y * spc->svfx.svfxShared.lWidth + x * 2] = 1 + (y * 127 / spc->svfx.svfxShared.lHeight); // draw level
	}

	// update the display
	BitBlt(spc->svfx.svfxDC, 0, 0, spc->svfx.svfxShared.lWidth, spc->svfx.svfxShared.lHeight, spc->svfx.svfxSpecDC, 0, 0, SRCCOPY);

	BASS_ChannelLock(spc->svfx.svfxHandle, FALSE);
}

#pragma optimize("",on) // restore optimizations

//---------------------------------------------------------------------------------
// remove chosen visual fx
//---------------------------------------------------------------------------------
static void WINAPI VFX_Free(void *inst)
{
	SPCVFX *spc=(SPCVFX*)inst;
	sharedRemove(&spc->svfx, spc);
}
//---------------------------------------------------------------------------------
// get parameters
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_GetParameters(void *inst, void *param)
{
	SPCVFX *spcc=(SPCVFX*)inst;
	BASS_VFXSPECTRUM *spc = (BASS_VFXSPECTRUM*)param;

	spc->lScale = spcc->spcScale;
	spc->lFFTsize = spcc->svfx.svfxSizeFFT;
	spc->lCombineBins = spcc->spcCombineBins;
	
	return sharedGetParams(&spc->shared, &spcc->svfx);
}
//---------------------------------------------------------------------------------
// set parameters
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_SetParameters(void *inst, const void *param)
{
	SPCVFX *spcc=(SPCVFX*)inst;
	BASS_VFXSPECTRUM *spc = (BASS_VFXSPECTRUM*)param;

	if(spc->lFFTsize < 128 || spc->lFFTsize > 4096)
		error(BASS_ERROR_ILLPARAM);

	spcc->spcScale = spc->lScale;
	spcc->spcCombineBins = spc->lCombineBins;

	return sharedSetParams(&spcc->svfx, &spc->shared, spc->lFFTsize);
}
//---------------------------------------------------------------------------------
// reset VFX state/buffers
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_Reset(void *inst)
{
	SPCVFX *spcc=(SPCVFX*)inst;
	return sharedReset(&spcc->svfx);
}
//---------------------------------------------------------------------------------
// set chosen visual fx
//---------------------------------------------------------------------------------

// FX function table
static ADDON_FUNCTIONS_FX VFXfuncs={VFX_Free, VFX_SetParameters, VFX_GetParameters, VFX_Reset};

HVFX SetVFX_Spectrum(DWORD chan, HWND hWnd, BASS_CHANNELINFO *info)
{
	SPCVFX *spc=spc = (SPCVFX*) malloc(sizeof(SPCVFX));
	memset(spc, 0, sizeof(SPCVFX));

	spc->spcScale = BASS_VFX_SPECTRUM_SCALE_SQRT;
	spc->spcCombineBins = TRUE;

	return sharedSetVFX(&spc->svfx, chan, hWnd, (LPTIMECALLBACK)&__VFX_UpdateSpectrum, spc, &VFXfuncs);
}
//-[EOF - spectrum.cpp ]-----------------------------------------------------------
