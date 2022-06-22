/*---------------------------------------------------------------------------------
    BASS_VFX 1.0 - Visual FX extension to the BASS Audio Library

    Visual Effects:
        Spectrum
        Specbands
        Spec3D
        Waveform
-----------------------------------------------------------------------------------
    filename   : bass_vfx_shared.cpp
    description: main shared functions
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
#include "../bass_vfx_std.h"

//---------------------------------------------------------------------------------
// get window width in pixels
//---------------------------------------------------------------------------------
long getWindowWidth(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return (rect.right - rect.left);
}
//---------------------------------------------------------------------------------
// get window height in pixels
//---------------------------------------------------------------------------------
long getWindowHeight(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return (rect.bottom - rect.top);
}
//---------------------------------------------------------------------------------
// create bitmap to draw in - 8 bit for easy updating :)
//---------------------------------------------------------------------------------
void setupBitmap(sharedVFX *svfx)
{
	BYTE data[2000] = {0};
	BITMAPINFOHEADER *bh = (BITMAPINFOHEADER*)data;
	RGBQUAD *pal = (RGBQUAD*)(data + sizeof(*bh));
	int a;
	bh->biSize = sizeof(*bh);
	bh->biWidth = svfx->svfxShared.lWidth;
	bh->biHeight = svfx->svfxShared.lHeight; // upside down (line 0=bottom)
	bh->biPlanes = 1;
	bh->biBitCount = 8;
	bh->biClrUsed = bh->biClrImportant = 256;
	// setup palette
	for (a = 1; a < 128; a++) {
		pal[a].rgbGreen = 255 - 2 * a;
		pal[a].rgbRed = 2 * a;
	}
	for (a = 0; a < 32; a++) {
		pal[128 + a].rgbBlue = 8 * a;
		pal[128 + 32 + a].rgbBlue = 255;
		pal[128 + 32 + a].rgbRed = 8 * a;
		pal[128 + 64 + a].rgbRed = 255;
		pal[128 + 64 + a].rgbBlue = 8 * (31 - a);
		pal[128 + 64 + a].rgbGreen = 8 * a;
		pal[128 + 96 + a].rgbRed = 255;
		pal[128 + 96 + a].rgbGreen = 255;
		pal[128 + 96 + a].rgbBlue = 8 * a;
	}
	// delete old bitmap first...
	if(svfx->svfxSpecBMP){
		DeleteDC(svfx->svfxSpecDC);
		DeleteObject(svfx->svfxSpecBMP);
	}
	// create the bitmap
	svfx->svfxSpecBMP = CreateDIBSection(0, (BITMAPINFO*)bh, DIB_RGB_COLORS, (void**)&svfx->svfxSpecBuf, NULL, 0);
	svfx->svfxSpecDC = CreateCompatibleDC(0);
	SelectObject(svfx->svfxSpecDC, svfx->svfxSpecBMP);
}
//---------------------------------------------------------------------------------
// shared remove visual fx
//---------------------------------------------------------------------------------
void sharedRemove(sharedVFX *svfx, void *inst)
{
	if(svfx->svfxHTimer) timeKillEvent(svfx->svfxHTimer);

	// in case the timer is still being called one more time after it has been killed
	Sleep(svfx->svfxShared.uTimerDelay * 2);

	// clear display
	sharedReset(svfx);

	// free stuff
	svfx->svfxSpecBuf = NULL;
	if(svfx->svfxDC) ReleaseDC(svfx->svfxShared.hWnd, svfx->svfxDC);
	if(svfx->svfxSpecDC) DeleteDC(svfx->svfxSpecDC);
	if(svfx->svfxSpecBMP) DeleteObject(svfx->svfxSpecBMP);

	free(inst);	// free struct
}
//---------------------------------------------------------------------------------
// shared reset visual fx
//---------------------------------------------------------------------------------
BOOL sharedReset(sharedVFX *svfx)
{
	// clear display
	memset(svfx->svfxSpecBuf, svfx->svfxShared.lBkGrndColor, svfx->svfxShared.lWidth * svfx->svfxShared.lHeight);
	BitBlt(svfx->svfxDC, 0, 0, svfx->svfxShared.lWidth, svfx->svfxShared.lHeight, svfx->svfxSpecDC, 0, 0, SRCCOPY);

	noerror();
}
//---------------------------------------------------------------------------------
// shared get visual fx parameters
//---------------------------------------------------------------------------------
BOOL sharedGetParams(BASS_VFXSHARED *shared, const sharedVFX *svfx)
{
	shared->hWnd = svfx->svfxShared.hWnd;
	shared->lWidth = svfx->svfxShared.lWidth;
	shared->lHeight = svfx->svfxShared.lHeight;
	shared->uTimerDelay = svfx->svfxShared.uTimerDelay;
	shared->uTimerResolution = svfx->svfxShared.uTimerResolution;
	shared->lBkGrndColor = svfx->svfxShared.lBkGrndColor;

	noerror();
}
//---------------------------------------------------------------------------------
// shared set visual fx parameters
//---------------------------------------------------------------------------------
BOOL sharedSetParams(sharedVFX *svfx, const BASS_VFXSHARED *shared, int fftSize)
{
	HDC dc;
	if(!(dc = GetDC(shared->hWnd))) {
		ReleaseDC(shared->hWnd, dc);	// in case
		error(BASS_ERROR_ILLPARAM);
	}

	svfx->svfxDC = dc;
	svfx->svfxShared.hWnd = shared->hWnd;
	svfx->svfxShared.lBkGrndColor = shared->lBkGrndColor;

	if(svfx->svfxSizeFFT != fftSize && fftSize) {
		int BASS_DATA_FFT;
		switch (fftSize){
			case 128: BASS_DATA_FFT = BASS_DATA_FFT256; break;
			case 256: BASS_DATA_FFT = BASS_DATA_FFT512; break;
			case 512: BASS_DATA_FFT = BASS_DATA_FFT1024; break;
			case 1024: BASS_DATA_FFT = BASS_DATA_FFT2048; break;
			case 2048: BASS_DATA_FFT = BASS_DATA_FFT4096; break;
			case 4096: BASS_DATA_FFT = BASS_DATA_FFT8192; break;
			default:
				error(BASS_ERROR_ILLPARAM);

		}
		svfx->svfxSizeFFT = fftSize;
		svfx->svfxBASS_DATA_FFT = BASS_DATA_FFT;
	}
	if(shared->lWidth != svfx->svfxShared.lWidth || shared->lHeight != svfx->svfxShared.lHeight){
		svfx->svfxShared.lHeight = shared->lHeight;
		svfx->svfxShared.lWidth = shared->lWidth;
		if (svfx->svfxShared.lWidth & 3) svfx->svfxShared.lWidth += 4 - (svfx->svfxShared.lWidth & 3); // bitmap width needs to be a multiple of 4
		setupBitmap(svfx);
	}
	if(shared->uTimerDelay != svfx->svfxShared.uTimerDelay || shared->uTimerResolution != svfx->svfxShared.uTimerResolution){
		if(svfx->svfxHTimer) timeKillEvent(svfx->svfxHTimer);
		svfx->svfxShared.uTimerDelay = shared->uTimerDelay;
		svfx->svfxShared.uTimerResolution = shared->uTimerResolution;
		svfx->svfxHTimer = timeSetEvent(shared->uTimerDelay, shared->uTimerResolution, svfx->svfxTimerLPTC, svfx->svfxTimerUser, TIME_PERIODIC|TIME_KILL_SYNCHRONOUS);
		if(!svfx->svfxHTimer) error(BASS_ERROR_ILLPARAM);
	}
	noerror();
}
//---------------------------------------------------------------------------------
// shared set visual fx
//---------------------------------------------------------------------------------
HVFX sharedSetVFX(sharedVFX *svfx, DWORD chan, HWND hWnd, LPTIMECALLBACK timerLPTC, void *inst, const ADDON_FUNCTIONS_FX *funcs)
{
	svfx->svfxHandle = chan;

	svfx->svfxTimerLPTC = timerLPTC;
	svfx->svfxTimerUser = (UINT)inst;

	BASS_VFXSHARED shared;

	shared.hWnd = hWnd;
	shared.lBkGrndColor = 0;
	shared.lHeight = getWindowHeight(hWnd);
	shared.lWidth = getWindowWidth(hWnd);
	shared.uTimerDelay = 25;					// timer at 40Hz
	shared.uTimerResolution = 25;

	if(!sharedSetParams(svfx, &shared, 1024)) {	// fft size 1024
		sharedRemove(svfx, inst);
		return 0;
	}

	return bassfunc->SetFX(chan, NULL, inst, 0, funcs); // set VFX on a channel
}
//-[EOF - bass_vfx_shared.cpp ]----------------------------------------------------
