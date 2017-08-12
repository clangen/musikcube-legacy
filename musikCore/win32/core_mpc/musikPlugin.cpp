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
#include "MpcDecoder.h"
#include "MpcMetadataEditor.h"
#include <objbase.h>
#include <map>
#include <windows.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***

musikCore::String g_Name = _T("core_mpc");
musikCore::String g_Description = _T("Musepack");
musikCore::String g_Formats = _T("mpc");
bool g_CanConfigure = false;
bool g_CanAbout = false;

///////////////////////////////////////////////////

std::map<HSTREAM, MpcDecoder*> g_DecoderMap;
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
    ::g_Mutex = ::CreateMutex(NULL, FALSE, NULL);
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

    if (mfn.GetExtension() == _T("mpc"))
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
    MpcDecoder* decoder = ::g_DecoderMap[stream];

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

DWORD CALLBACK MPC_Decode_Proc(HSTREAM stream, void* buffer, DWORD length, DWORD user)
{
    CriticalSection cs(::g_Mutex);
    DWORD decodedBytes = ::g_DecoderMap[stream]->Decode(buffer, length);
    //
    if (decodedBytes != length)
    {
        return BASS_STREAMPROC_END;
    }

    return decodedBytes;
}

///////////////////////////////////////////////////

HSTREAM LoadFile(const musikCore::String& filename)
{
    MpcDecoder* decoder = new MpcDecoder(filename.c_str());

    HSTREAM stream = NULL;
    if (decoder->CanPlay())
    {
        stream = BASS_StreamCreate(
            decoder->GetSampleRate(),
            decoder->GetNumChannels(),
            BASS_STREAM_AUTOFREE,
            (STREAMPROC*) MPC_Decode_Proc,
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


bool WriteTag(const musikCore::SongInfo& info)
{
    bool ret = true;

    try
    {
#if defined (WIN32)
        TagLib::FileRef tag_file(info.GetFilename().c_str());
#else
        TagLib::FileRef tag_file(utf16to8(info.GetFilename(), true).c_str());
#endif
        if (! tag_file.isNull())
        {
            TagLib::Tag *tag = tag_file.tag();

            tag->setArtist(info.GetArtist().c_str());
            tag->setAlbum(info.GetAlbum().c_str());
            tag->setTitle(info.GetTitle().c_str());
            tag->setGenre(info.GetGenre().c_str());
            tag->setYear(musikCore::StringToInt(info.GetYear()));
            tag->setTrack(musikCore::StringToInt(info.GetTrackNum()));
            tag->setComment(info.GetNotes().c_str());

            tag_file.save();
        }
    }
    catch (...)
    {
        ret = false;
        cout << "taglib crashed trying to write: " << info.GetFilename().c_str() << endl;
    }

    return ret;
}

///////////////////////////////////////////////////

bool ReadTag(const musikCore::String& fn, musikCore::SongInfo& target)
{
    bool ret = true;

    musikCore::Filename mfn(fn);

    target.SetFilename(fn);
    target.SetFormat("Musepack");

    try
    {
#if defined (WIN32)
        TagLib::FileRef tag_file(fn.c_str());
#else
        TagLib::FileRef tag_file(utf16to8(fn, true).c_str());
#endif
        if (! tag_file.isNull())
        {
            if (tag_file.tag())
            {
                TagLib::Tag *tag = tag_file.tag();

                target.SetArtist(musikCore::utf8to16(tag->artist().to8Bit(true)));
                target.SetAlbum(musikCore::utf8to16(tag->album().to8Bit(true)));
                target.SetTitle(musikCore::utf8to16(tag->title().to8Bit(true)));
                target.SetGenre(musikCore::utf8to16(tag->genre().to8Bit(true)));
                target.SetNotes(musikCore::utf8to16(tag->comment().to8Bit(true)));

                target.SetYear(musikCore::IntToString(tag->year()));
                target.SetTrackNum(musikCore::IntToString(tag->track()));
            }

            if (tag_file.audioProperties())
            {
                TagLib::AudioProperties *properties = tag_file.audioProperties();
                int duration = properties->length() * 1000;
                target.SetBitrate(musikCore::IntToString(properties->bitrate()));
                target.SetDuration(musikCore::IntToString(duration));
            }
        }

        // if the title is empty, then use the
        // filename...
        if (target.GetTitle().IsEmpty())
        {
            musikCore::Filename MFN(fn);
            target.SetTitle(MFN.GetJustFilename());
        }
    }
    catch (...)
    {
        ret = false;
        cout << "taglib crashed reading: " << fn.c_str() << endl;
    }

    return ret;
}

///////////////////////////////////////////////////