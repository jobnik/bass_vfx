/*---------------------------------------------------------------------------------
    BASS_VFX 1.0 - Visual FX extension to the BASS Audio Library

    Visual Effects:
        Spectrum
        Specbands
        Spec3D
        Waveform
-----------------------------------------------------------------------------------
    filename   : spec3d.cpp
    description: 3d spectrum visual effect
-----------------------------------------------------------------------------------
    website: https://www.jobnik.net/projects/bass_vfx

    forums : https://www.un4seen.com/forum/?board=1
             https://www.jobnik.net/forums

    e-mail : bass_vfx@jobnik.net
-----------------------------------------------------------------------------------
    Spec3D VFX Copyright (c) 1999-2008, Ian Luck

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
// Spec3D
typedef struct {
	int			s3dMarkerPos;	// marker pos
	BOOL		s3dCombineBins;
	sharedVFX	svfx;
} S3DVFX;

//---------------------------------------------------------------------------------
// update the spectrum (3D) display
//---------------------------------------------------------------------------------

#pragma optimize("t",on) // speed optimizations

static void CALLBACK __VFX_UpdateSpec3D(UINT uTimerID, UINT uMsg, S3DVFX *s3d, DWORD dw1, DWORD dw2)
{
	int x, y;
	int x2 = 0; // current bin marker
	float *fft;

	BASS_ChannelLock(s3d->svfx.svfxHandle, TRUE);

	// get the FFT data
	fft = (float*)alloca(s3d->svfx.svfxSizeFFT * sizeof(float)); // allocate buffer for data
	BASS_ChannelGetData(s3d->svfx.svfxHandle, fft, s3d->svfx.svfxBASS_DATA_FFT);

	for (x = 0; x < s3d->svfx.svfxShared.lHeight; x++) {
		if(s3d->s3dCombineBins) {
			float y2 = fft[1 + x2];
			while (x2 < (x + 1) * (s3d->svfx.svfxSizeFFT - 1) / s3d->svfx.svfxShared.lHeight) { // combine multiple bins if necessary...
				x2++;
				if (y2 < fft[1 + x2]) y2 = fft[1 + x2];
			}
			y = sqrt(y2) * 3 * 127; // scale it (sqrt to make low values more visible)
		} else
			y = sqrt(fft[x + 1]) * 3 * 127; // scale it (sqrt to make low values more visible)

		if (y > 127) y = 127; // cap it
		s3d->svfx.svfxSpecBuf[x * s3d->svfx.svfxShared.lWidth + s3d->s3dMarkerPos] = 128 + y; // plot it
	}
	// move marker onto next position
	s3d->s3dMarkerPos = (s3d->s3dMarkerPos + 1) % s3d->svfx.svfxShared.lWidth;
	for (x = 0; x < s3d->svfx.svfxShared.lHeight; x++) s3d->svfx.svfxSpecBuf[x * s3d->svfx.svfxShared.lWidth + s3d->s3dMarkerPos] = 255;

	// update the display
	BitBlt(s3d->svfx.svfxDC, 0, 0, s3d->svfx.svfxShared.lWidth, s3d->svfx.svfxShared.lHeight, s3d->svfx.svfxSpecDC, 0, 0, SRCCOPY);

	BASS_ChannelLock(s3d->svfx.svfxHandle, FALSE);
}

#pragma optimize("",on) // restore optimizations

//---------------------------------------------------------------------------------
// remove chosen visual fx
//---------------------------------------------------------------------------------
static void WINAPI VFX_Free(void *inst)
{
	S3DVFX *s3d=(S3DVFX*)inst;
	sharedRemove(&s3d->svfx, s3d);
}
//---------------------------------------------------------------------------------
// get parameters
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_GetParameters(void *inst, void *param)
{
	S3DVFX *s3dc=(S3DVFX*)inst;
	BASS_VFXSPEC3D *s3d = (BASS_VFXSPEC3D*)param;

	s3d->lMarkerPos = s3dc->s3dMarkerPos;
	s3d->lFFTsize = s3dc->svfx.svfxSizeFFT;
	s3d->lCombineBins = s3dc->s3dCombineBins;

	return sharedGetParams(&s3d->shared, &s3dc->svfx);
}
//---------------------------------------------------------------------------------
// set parameters
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_SetParameters(void *inst, const void *param)
{
	S3DVFX *s3dc=(S3DVFX*)inst;
	BASS_VFXSPEC3D *s3d = (BASS_VFXSPEC3D*)param;

	if(s3d->lFFTsize < 128 || s3d->lFFTsize > 4096 || s3d->lMarkerPos < 0)
		error(BASS_ERROR_ILLPARAM);

	s3dc->s3dMarkerPos = s3d->lMarkerPos;
	s3dc->s3dCombineBins = s3d->lCombineBins;

	return sharedSetParams(&s3dc->svfx, &s3d->shared, s3d->lFFTsize);
}
//---------------------------------------------------------------------------------
// reset VFX state/buffers
//---------------------------------------------------------------------------------
static BOOL WINAPI VFX_Reset(void *inst)
{
	S3DVFX *s3dc=(S3DVFX*)inst;
	return sharedReset(&s3dc->svfx);
}
//---------------------------------------------------------------------------------
// set chosen visual fx
//---------------------------------------------------------------------------------

// FX function table
static ADDON_FUNCTIONS_FX VFXfuncs={VFX_Free, VFX_SetParameters, VFX_GetParameters, VFX_Reset};

HVFX SetVFX_Spec3D(DWORD chan, HWND hWnd, BASS_CHANNELINFO *info)
{
	S3DVFX *s3d=(S3DVFX*) malloc(sizeof(S3DVFX));
	memset(s3d, 0, sizeof(S3DVFX));

	//s3d->s3dMarkerPos = 0;
	return sharedSetVFX(&s3d->svfx, chan, hWnd, (LPTIMECALLBACK)&__VFX_UpdateSpec3D, s3d, &VFXfuncs);
}
//-[ EOF - spec3d.cpp ]------------------------------------------------------------
