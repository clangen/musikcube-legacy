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
#include "WmaDecoder.h"
#include "WmaMetadataEditor.h"
#include <objbase.h>
#include <map>
#include <windows.h>

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***

musikCore::String g_Name = _T("core_wma");
musikCore::String g_Description = _T("Windows Media Audio (wma)");
musikCore::String g_Formats = _T("wma");
bool g_CanConfigure = false;
bool g_CanAbout = false;

///////////////////////////////////////////////////

// global vars

std::map<HSTREAM, WmaDecoder*> g_DecoderMap;
HANDLE g_Mutex;

class CriticalSection
{
public:
    CriticalSection(HANDLE mutex)
    {
        this->mutex = mutex;
        ::WaitForSingleObject(mutex, INFINITE);
    }

    ~CriticalSection()
    {
        ::ReleaseMutex(this->mutex);
    }

private:
    HANDLE mutex;
};

///////////////////////////////////////////////////

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
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
    ::g_Mutex = CreateMutex(NULL, FALSE, NULL);
    ::g_DecoderMap.clear();
}

///////////////////////////////////////////////////

// optional

void OnPluginDetach()
{
    ::CloseHandle(::g_Mutex);
}

///////////////////////////////////////////////////

bool CanPlay(const musikCore::String& filename)
{
    musikCore::Filename mfn(filename);

    if (mfn.GetExtension() == _T("wma"))
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////

bool Play(HSTREAM stream, int offset)
{
    BASS_ChannelPlay(stream, true);

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
    CriticalSection cs(::g_Mutex);
    return ::g_DecoderMap[stream]->Seek(ms);
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
    CriticalSection cs(::g_Mutex);
    WmaDecoder* decoder = ::g_DecoderMap[stream];

    if (decoder)
    {
        BASS_ChannelStop(stream);
        BASS_StreamFree(stream);

        delete ::g_DecoderMap[stream];
        ::g_DecoderMap.erase(stream);
    }

    return true;
}

///////////////////////////////////////////////////

int GetDuration(HSTREAM stream)
{
    CriticalSection cs(::g_Mutex);
    return ::g_DecoderMap[stream]->GetDuration();
}

///////////////////////////////////////////////////

int GetTime(HSTREAM stream)
{
    CriticalSection cs(::g_Mutex);
    return ::g_DecoderMap[stream]->GetTime();
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

DWORD CALLBACK WMA_Decode_Proc(HSTREAM stream, void* buffer, DWORD length, DWORD user)
{
    try
    {
        CriticalSection cs(::g_Mutex);
        DWORD decodedBytes = ::g_DecoderMap[stream]->Decode(buffer, length);
        //
        if (decodedBytes == length)
        {
            return decodedBytes;
        }
    }
    catch(...)
    {
        std::wstring error = L"Caught unhandled exception while decoding WMA file";
        ::MessageBox(NULL, error.c_str(), L"musikCube - core_wma", MB_OK | MB_ICONERROR);
    }

    return BASS_STREAMPROC_END;
}

///////////////////////////////////////////////////

HSTREAM LoadFile(const musikCore::String& filename)
{
    WmaDecoder* decoder = new WmaDecoder(filename.c_str());

    HSTREAM stream = NULL;
    if (decoder->CanPlay())
    {
        stream = BASS_StreamCreate(
            decoder->GetSampleRate(),
            decoder->GetChannels(),
            BASS_STREAM_AUTOFREE,
            WMA_Decode_Proc,
            NULL);
    }

    if (stream == NULL)
    {
        delete decoder;
    }
    else
    {
        CriticalSection cs(::g_Mutex);
        ::g_DecoderMap[stream] = decoder;
    }

    return stream;
}

///////////////////////////////////////////////////

#define TRY_SET_STRING_ATTRIBUTE(attribute, filename) \
    if (editor.SetStrAttribute(attribute, filename) != S_OK) { return false; }

bool WriteTag(const musikCore::SongInfo& info)
{
    WmaMetadataEditor editor(info.GetFilename().c_str());

    if (editor.IsValid())
    {
        try
        {
            TRY_SET_STRING_ATTRIBUTE(g_wszWMAlbumArtist, info.GetArtist().c_str());
            TRY_SET_STRING_ATTRIBUTE(g_wszWMAuthor, info.GetArtist().c_str());
            TRY_SET_STRING_ATTRIBUTE(g_wszWMAlbumTitle, info.GetAlbum().c_str());
            TRY_SET_STRING_ATTRIBUTE(g_wszWMTitle, info.GetTitle().c_str());
            TRY_SET_STRING_ATTRIBUTE(g_wszWMGenre, info.GetGenre().c_str());
            TRY_SET_STRING_ATTRIBUTE(g_wszWMYear, info.GetYear().c_str());
            TRY_SET_STRING_ATTRIBUTE(g_wszWMTrackNumber, info.GetTrackNum().c_str());
            TRY_SET_STRING_ATTRIBUTE(g_wszWMDescription, info.GetNotes().c_str());

            return editor.Write();
        }
        catch(...)
        {
            std::wstring error = L"Caught unhandled exception while writing WMA file: " + info.GetFilename();
            ::OutputDebugString(error.c_str());
        }
    }

    return false;
}

///////////////////////////////////////////////////

bool ReadTag(const musikCore::String& fn, musikCore::SongInfo& target)
{
    WmaMetadataEditor editor(fn.c_str());

    if (editor.IsValid())
    {
        try
        {
            target.SetArtist(editor.GetStrAttribute(g_wszWMAlbumArtist));

            if (target.GetArtist() == L"")
            {
                target.SetArtist(editor.GetStrAttribute(g_wszWMAuthor));
            }

            target.SetAlbum(editor.GetStrAttribute(g_wszWMAlbumTitle));
            target.SetTitle(editor.GetStrAttribute(g_wszWMTitle));
            target.SetGenre(editor.GetStrAttribute(g_wszWMGenre));
            target.SetYear(editor.GetStrAttribute(g_wszWMYear));
            target.SetNotes(editor.GetStrAttribute(g_wszWMDescription));

            // track number can be stored as an int OR a string!
            //
            // also: it appears track numbers are stored 0 based if they
            // are stored as strings?
            //
            std::wstring strTrackNum = editor.GetStrAttribute(g_wszWMTrackNumber);
            int trackNum = musikCore::StringToInt(strTrackNum);
            //
            if (trackNum <= 0)
            {
                target.SetTrackNum(musikCore::IntToString(
                    editor.GetDWORDAttribute(g_wszWMTrackNumber) + 1));
            }
            else
            {
                target.SetTrackNum(musikCore::IntToString(trackNum));
            }

            target.SetBitrate(musikCore::IntToString(
                editor.GetDWORDAttribute(g_wszWMBitrate) / 1000));
            //
            target.SetDuration(musikCore::IntToString(
                editor.GetDWORDAttribute(g_wszWMDuration) / 10000));

            // if the title is empty, then use the
            // filename...
            if (target.GetTitle().IsEmpty())
            {
                musikCore::Filename MFN(fn);
                target.SetTitle(MFN.GetJustFilename());
            }

            target.SetFormat("Windows Media Audio");

            return true;
        }
        catch(...)
        {
            std::wstring error = L"Caught unhandled exception while reading WMA file: " + fn;
            ::OutputDebugString(error.c_str());
        }
    }

    return false;
}

///////////////////////////////////////////////////
