/*---------------------------------------------------------------------------------
    BASS_VFX 1.0 - Visual FX extension to the BASS Audio Library

    Visual Effects:
        Spectrum
        Specbands
        Spec3D
        Waveform
-----------------------------------------------------------------------------------
    filename   : bass_vfx_set.cpp
    description: main functions
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
#include "spectrum.h"
#include "specbands.h"
#include "spec3d.h"
#include "waveform.h"

//---------------------------------------------------------------------------------
// set chosen visual fx
// VFX plugin function. return VFX handle, 0=unrecognised VFX or error
//---------------------------------------------------------------------------------
HVFX WINAPI BASS_VFX_Set(DWORD handle, HWND hWnd, int vfx)
{
	BASS_CHANNELINFO info;
	if(!BASS_ChannelGetInfo(handle, &info)) return 0;	// invalid handle

	switch (vfx) {
		case BASS_VFX_SPECTRUM: return SetVFX_Spectrum(handle, hWnd, &info);
		case BASS_VFX_SPECBANDS: return SetVFX_Specbands(handle, hWnd, &info);
		case BASS_VFX_SPEC3D: return SetVFX_Spec3D(handle, hWnd, &info);
		case BASS_VFX_WAVEFORM: return SetVFX_Waveform(handle, hWnd, &info);
	}
	error(BASS_ERROR_ILLPARAM);
}
//---------------------------------------------------------------------------------
// remove chosen visual fx
//---------------------------------------------------------------------------------
BOOL WINAPI BASS_VFX_Remove(DWORD handle, HVFX vfx)
{
	return BASS_ChannelRemoveFX(handle, vfx);
}
//---------------------------------------------------------------------------------
// get parameters
//---------------------------------------------------------------------------------
BOOL WINAPI BASS_VFX_GetParameters(HVFX handle, void *params)
{
	return BASS_FXGetParameters(handle, params);
}
//---------------------------------------------------------------------------------
// set parameters
//---------------------------------------------------------------------------------
BOOL WINAPI BASS_VFX_SetParameters(HVFX handle, const void *params)
{
	return BASS_FXSetParameters(handle, params);
}
//---------------------------------------------------------------------------------
// reset parameters
//---------------------------------------------------------------------------------
BOOL WINAPI BASS_VFX_Reset(HVFX handle)
{
	return BASS_FXReset(handle);
}
//-[EOF - bass_vfx_set.cpp ]-------------------------------------------------------
