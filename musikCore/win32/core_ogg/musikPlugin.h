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

#ifndef __C_MUSIKPLUGIN_H__
#define __C_MUSIKPLUGIN_H__

///////////////////////////////////////////////////

#include <bass.h>
#include <musikCore.h>
#include <iostream>
#include <vector>

#ifdef MUSIKPLUGIN_EXPORTS
#define MUSIKPLUGIN_API __declspec(dllexport)
#else
#define MUSIKPLUGIN_API __declspec(dllimport)
#endif

///////////////////////////////////////////////////

extern "C" {

///////////////////////////////////////////////////

// these can be implemented and/or initialized 

extern bool g_CanConfigure;
extern bool g_CanAbout;
extern musikCore::String g_Name;
extern musikCore::String g_Description;    
extern musikCore::String g_Formats;
MUSIKPLUGIN_API void Configure();
MUSIKPLUGIN_API void About();
MUSIKPLUGIN_API void OnSources();
void OnPluginAttach();
void OnPluginDetach();

///////////////////////////////////////////////////

// stream prototype

///////////////////////////////////////////////////

// *** YOU DO NOT WANT TO MODIFY ANY OF THESE ***
// musikCube uses them for communication between the
// plugin and the main UI.

extern HMODULE g_Instance;
extern musikCore::Library* g_Library;
extern musikCore::Player* g_Player;
extern HWND g_MainWnd;

MUSIKPLUGIN_API    const musikCore::String& GetPluginFormats();
MUSIKPLUGIN_API    const musikCore::String& GetPluginName();
MUSIKPLUGIN_API    const musikCore::String& GetPluginDescription();
MUSIKPLUGIN_API void Unload();

///////////////////////////////////////////////////

// all of these must be implemented

MUSIKPLUGIN_API bool CanPlay(const musikCore::String& filename);
MUSIKPLUGIN_API bool Play(HSTREAM stream, int offset = 0);
MUSIKPLUGIN_API bool CanSeek();
MUSIKPLUGIN_API bool Seek(HSTREAM stream, int ms);
MUSIKPLUGIN_API bool Pause(HSTREAM stream);
MUSIKPLUGIN_API bool Resume(HSTREAM stream);
MUSIKPLUGIN_API bool Stop(HSTREAM stream); 
MUSIKPLUGIN_API int  GetDuration(HSTREAM stream);
MUSIKPLUGIN_API int  GetTime(HSTREAM stream);
MUSIKPLUGIN_API bool IsActive(HSTREAM stream);
MUSIKPLUGIN_API HSTREAM LoadFile(const musikCore::String& file);
MUSIKPLUGIN_API bool WriteTag(const musikCore::SongInfo& info);
MUSIKPLUGIN_API bool ReadTag(const musikCore::String& fn, musikCore::SongInfo& target);

///////////////////////////////////////////////////

}    // extern "C"

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

