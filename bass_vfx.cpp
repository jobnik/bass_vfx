/*---------------------------------------------------------------------------------
    BASS_VFX 1.0 - Visual FX extension to the BASS Audio Library

    Visual Effects:
        Spectrum
        Specbands
        Spec3D
        Waveform
-----------------------------------------------------------------------------------
    filename   : bass_vfx.cpp
    description: main dll functions
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
#include "bass_vfx_std.h"

//---------------------------------------------------------------------------------
// get BASS_VFX version
//---------------------------------------------------------------------------------
DWORD WINAPI BASS_VFX_GetVersion(void)
{
	return 0x01000000;	// 1.0.0.0 (01 = 1 . 00 = 0 . 00 = 0 . 00 = 0)
}
//---------------------------------------------------------------------------------
// main dll function
//---------------------------------------------------------------------------------
BOOL WINAPI DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	#ifdef _DEBUG 
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG ); 
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG ); 
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF 
		|_CRTDBG_CHECK_ALWAYS_DF 
		|_CRTDBG_LEAK_CHECK_DF); 
	#endif

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls((HINSTANCE)hModule);
			if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
					MessageBox(0,"Incorrect BASS.DLL version (2.4 is required)","BASS_VFX",MB_ICONERROR);
					return FALSE;
			}
    }
    return TRUE;
}
//-[EOF - bass_vfx.cpp]------------------------------------------------------------
