///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include <string>

///////////////////////////////////////////////////

class GlobalHotkeys
{
public:

    enum Hotkey
    {
        PlayPauseResume = 200,
        Stop,
        NextTrack,
        PrevTrack,
        VolumeUp,
        VolumeDown,
        NextArtist,
        PrevArtist,
        NextAlbum,
        PrevAlbum,
        MediaKeyPlayPauseResume,
        MediaKeyStop,
        MediaKeyNextTrack,
        MediaKeyPrevTrack,
        MediaKeyVolumeUp,
        MediaKeyVolumeDown
    };

    static void DeserializeHotkey(DWORD hotkey, WORD& virtualKey, WORD& modifiers);

    static void ReloadHotkeys();
    static void UnloadHotkeys();
    static bool ProcessHotkey(enum Hotkey hotkey, std::wstring& statusString);

	HWND getMainCubeHwnd();
	
protected:

    static void SetHotkey(HWND hwnd, enum Hotkey hotkey, DWORD serializedHotkey);

/*	UINT m_nNumMixers;
	HMIXER m_hMixer;
	MIXERCAPS m_mxcaps;

	CString m_strDstLineName, m_strVolumeControlName;
	DWORD m_dwMinimum, m_dwMaximum;
	DWORD m_dwVolumeControlID;

	bool WinVolInitialize();
	bool WinVolUninitialize();
	bool WinGetMasterVolumeControl();
	bool WinGetMasterVolumeValue(DWORD &dwVal) const;
	bool WinSetMasterVolumeValue(DWORD dwVal) const;*/
};

///////////////////////////////////////////////////
