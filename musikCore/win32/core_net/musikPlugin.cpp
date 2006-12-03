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

#include "stdafx.h"
#include "musikPlugin.h"

#include <windows.h>
#include <bass.h>

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***

musikCore::String g_Name = _T("core_NET");
musikCore::String g_Description = _T("Net Streams");
musikCore::String g_Formats = _T("");
bool g_CanConfigure = true;
bool g_CanAbout = true;

///////////////////////////////////////////////////

// these will get initialized automatically by musikCube

HMODULE g_Instance = NULL;
HSYNC g_Sync = NULL;

///////////////////////////////////////////////////

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        OnPluginAttach();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

///////////////////////////////////////////////////

// do not modify these unless you know what you're doing

const musikCore::String& GetPluginDescription()
{ return g_Description; }

const musikCore::String& GetPluginName()
{ return g_Name; }

const musikCore::String& GetPluginFormats()
{ return g_Formats; }

void Unload()
{ OnPluginDetach(); }

///////////////////////////////////////////////////

// implement me

void Configure()
{
}

///////////////////////////////////////////////////

// implement me

void About()
{
}

///////////////////////////////////////////////////

// optional

void OnPluginAttach()
{
}

///////////////////////////////////////////////////

// optional

void OnPluginDetach()
{

}

///////////////////////////////////////////////////

bool CanPlay(const musikCore::String& filename)
{
    // we handle netstreams and cda...
    if (filename.Left(4).ToLower() == _T("http"))
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////

bool Play(HSTREAM stream, int offset)
{
    BASS_ChannelPlay(
        stream,
        true
       );

    if (offset)
    {
        Seek(stream, offset);
    }

    return true;
}

///////////////////////////////////////////////////

bool CanSeek()
{ 
    return true; 
}

///////////////////////////////////////////////////

bool Seek(HSTREAM stream, int ms)
{
    float secs = (float)ms / 1000.0f;
    QWORD pos = BASS_ChannelSeconds2Bytes(stream, secs);
    BASS_ChannelSetPosition(stream, pos);

    return true;
}

///////////////////////////////////////////////////

bool Pause(HSTREAM stream)
{
    BASS_ChannelPause(stream);
    return true;
}

///////////////////////////////////////////////////

bool Resume(HSTREAM stream)
{
    BASS_ChannelPlay(stream, false);
    return true;
}

///////////////////////////////////////////////////

bool Stop(HSTREAM stream)
{    
    BASS_ChannelStop(stream);
    BASS_StreamFree(stream);
    return true;
}

///////////////////////////////////////////////////

int GetDuration(HSTREAM stream)
{
    return 0;
}

///////////////////////////////////////////////////

int GetTime(HSTREAM stream)
{
    return 0;
}

///////////////////////////////////////////////////

bool IsActive(HSTREAM stream)
{
    DWORD state = BASS_ChannelIsActive(stream);
    if (state == BASS_ACTIVE_PLAYING || state == BASS_ACTIVE_PAUSED)
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////

HSTREAM LoadFile(const musikCore::String& filename)
{
    HSTREAM load = NULL;

    musikCore::String fn = filename;
    fn = fn.Trim();
    std::string url8bit = musikCore::utf16to8(fn, false);

    load = BASS_StreamCreateURL(
        url8bit.c_str(),
        0, 
        BASS_STREAM_BLOCK | BASS_STREAM_AUTOFREE,
        NULL,
        NULL);

    if (load == 0)
    {
        int error = BASS_ErrorGetCode();
        error += 0;
    }

    return load;
}

///////////////////////////////////////////////////

bool WriteTag(const musikCore::SongInfo& info)
{
    return false;
}

///////////////////////////////////////////////////

bool ReadTag(const musikCore::String& fn, musikCore::SongInfo& target)
{
    return false;
}

///////////////////////////////////////////////////