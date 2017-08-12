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
#include "ape/include/APETag.h"
#include "ape/include/MACLib.h"

using namespace APE;

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***

musikCore::String g_Name = _T("core_ape");
musikCore::String g_Description = _T("Monkey Audio Format (APE)");
musikCore::String g_Formats = _T("ape");
bool g_CanConfigure = true;
bool g_CanAbout = true;
OpenThreads::Mutex g_StreamMutex;

///////////////////////////////////////////////////

const float g_BufferSizeSec = 0.5;

struct APE_StreamInfo
{
    HSTREAM m_StreamID;
    IAPEDecompress* m_pAPEDecompress;        // decoder
    int m_Bitrate;
    int m_BitsPerSample;
    int m_BytesPerBlock;
    int m_DecoderBufferSize;
    int m_SampleRate;
    int m_LengthMS;
    int m_Channels;
};

std::vector<APE_StreamInfo> g_ActiveStreams;

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
}

///////////////////////////////////////////////////

// optional

void OnPluginDetach()
{

}

///////////////////////////////////////////////////

bool CanPlay(const musikCore::String& filename)
{
    musikCore::Filename MFN(filename);
    if (MFN.GetExtension() == _T("ape"))
        return true;

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
        Seek(stream, offset);

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
    bool ret = false;

    g_StreamMutex.lock();    // decoding while seeking is bad, so use a
                            // critical section to avoid it
    for (size_t i = 0; i < g_ActiveStreams.size(); i++)
    {
        if (g_ActiveStreams.at(i).m_StreamID == stream)
        {
            float total = (float)g_ActiveStreams.at(i).m_pAPEDecompress->GetInfo(APE_DECOMPRESS_TOTAL_BLOCKS);
            float seek_per = (float)ms / (float)g_ActiveStreams.at(i).m_LengthMS;
            total *= seek_per;

            g_ActiveStreams.at(i).m_pAPEDecompress->Seek(total);
            ret = true;
        }
    }
    g_StreamMutex.unlock();

    return false;
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

    for (size_t i = 0; i < g_ActiveStreams.size(); i++)
    {
        if (g_ActiveStreams.at(i).m_StreamID == stream)
        {
            delete g_ActiveStreams.at(i).m_pAPEDecompress;        // free ape
            g_ActiveStreams.erase(g_ActiveStreams.begin() + i);    // get it out of the array
        }
    }

    return true;
}

///////////////////////////////////////////////////

int  GetDuration(HSTREAM stream)
{
    for (size_t i = 0; i < g_ActiveStreams.size(); i++)
    {
        if (stream == g_ActiveStreams.at(i).m_StreamID)
            return g_ActiveStreams.at(i).m_LengthMS;
    }
    return 0;
}

///////////////////////////////////////////////////

int GetTime(HSTREAM stream)
{
    for (size_t i = 0; i < g_ActiveStreams.size(); i++)
    {
        if (g_ActiveStreams.at(i).m_StreamID == stream)
            return g_ActiveStreams.at(i).m_pAPEDecompress->GetInfo(APE_DECOMPRESS_CURRENT_MS);
    }
    return 0;
}

///////////////////////////////////////////////////

bool IsActive(HSTREAM stream)
{
    DWORD state = BASS_ChannelIsActive(stream);
    if (state == BASS_ACTIVE_PLAYING || state == BASS_ACTIVE_PAUSED)
        return true;

    return false;
}

///////////////////////////////////////////////////

DWORD CALLBACK APE_Decode_Proc(HSTREAM stream, void* buffer, DWORD length, DWORD user)
{
    g_StreamMutex.lock();    // cannot seek and decode at the same time,
                            // so enter a critical section
    DWORD final = 0;
    for (size_t i = 0; i < g_ActiveStreams.size(); i++)
    {
        if (g_ActiveStreams.at(i).m_StreamID == stream)
        {
            // ok to decode...
            int result;
            g_ActiveStreams.at(i).m_pAPEDecompress->GetData((char*)buffer, length / g_ActiveStreams.at(i).m_BytesPerBlock, &result);
            final = result * g_ActiveStreams.at(i).m_BytesPerBlock;;
        }
    }
    g_StreamMutex.unlock();

    return final;
}

///////////////////////////////////////////////////

HSTREAM LoadFile(const musikCore::String& filename)
{
    int errorval;
    APE_StreamInfo info;
    info.m_pAPEDecompress = CreateIAPEDecompress(filename.c_str(), &errorval);
    HSTREAM stream = NULL;
    if (info.m_pAPEDecompress)
    {
        info.m_Bitrate = info.m_pAPEDecompress->GetInfo(APE_INFO_AVERAGE_BITRATE);
        info.m_BitsPerSample = info.m_pAPEDecompress->GetInfo(APE_INFO_BITS_PER_SAMPLE);
        info.m_SampleRate = info.m_pAPEDecompress->GetInfo(APE_INFO_SAMPLE_RATE);
        info.m_LengthMS = info.m_pAPEDecompress->GetInfo(APE_DECOMPRESS_LENGTH_MS);
        info.m_Channels = info.m_pAPEDecompress->GetInfo(APE_INFO_CHANNELS);
        info.m_BytesPerBlock = info.m_Channels * (info.m_BitsPerSample >> 3);
        info.m_StreamID = BASS_StreamCreate(info.m_SampleRate, info.m_Channels, BASS_STREAM_AUTOFREE, (STREAMPROC*)APE_Decode_Proc, NULL);
        stream = info.m_StreamID;
        g_ActiveStreams.push_back(info);
    }

    return stream;
}

///////////////////////////////////////////////////

#define GET_TAG(APE_INFO) ((CAPETag *) (APE_INFO)->GetInfo(APE_INFO_TAG))

///////////////////////////////////////////////////

bool WriteTag(const musikCore::SongInfo& info)
{
    int errorval;
    IAPEDecompress* m_pAPEDecompress = CreateIAPEDecompress(info.GetFilename().c_str(), &errorval);

    if (m_pAPEDecompress)
    {
    //    GET_TAG(m_pAPEDecompress)->SetFieldString(APE_TAG_FIELD_ARTIST, info.GetArtist().c_str());
    //    GET_TAG(m_pAPEDecompress)->SetFieldString(APE_TAG_FIELD_ALBUM, info.GetAlbum().c_str());
    //    GET_TAG(m_pAPEDecompress)->SetFieldString(APE_TAG_FIELD_GENRE, info.GetGenre().c_str());
    //    GET_TAG(m_pAPEDecompress)->SetFieldString(APE_TAG_FIELD_YEAR, info.GetYear().c_str());
    //    GET_TAG(m_pAPEDecompress)->SetFieldString(APE_TAG_FIELD_COMMENT, info.GetNotes().c_str());
    //    GET_TAG(m_pAPEDecompress)->SetFieldString(APE_TAG_FIELD_TITLE, info.GetTitle().c_str());
    //    GET_TAG(m_pAPEDecompress)->SetFieldString(APE_TAG_FIELD_TRACK, info.GetTrackNum().c_str());
    //    GET_TAG(m_pAPEDecompress)->Save();

        delete m_pAPEDecompress;
        return true;
    }
    return false;
}

///////////////////////////////////////////////////

bool ReadTag(const musikCore::String& fn, musikCore::SongInfo& target)
{
    int errorval;
    IAPEDecompress* m_pAPEDecompress = CreateIAPEDecompress(fn.c_str(), &errorval);

    target.SetFormat(_T("Monkey's Audio"));
    if (m_pAPEDecompress)
    {
        wchar_t buffer[256];

    //    int chars = 256;
    //    if (GET_TAG(m_pAPEDecompress)->GetFieldString(APE_TAG_FIELD_TITLE, buffer, &chars) == 0)
    //        target.SetTitle(buffer);

    //    chars = 256;
    //    if (GET_TAG(m_pAPEDecompress)->GetFieldString(APE_TAG_FIELD_ARTIST, buffer, &chars) == 0)
    //        target.SetArtist(buffer);

    //    chars = 256;
    //    if (GET_TAG(m_pAPEDecompress)->GetFieldString(APE_TAG_FIELD_ALBUM, buffer, &chars) == 0)
    //        target.SetAlbum(buffer);

    //    chars = 256;
    //    if (GET_TAG(m_pAPEDecompress)->GetFieldString(APE_TAG_FIELD_GENRE, buffer, &chars) == 0)
    //        target.SetGenre(buffer);

    //    chars = 256;
    //    if (GET_TAG(m_pAPEDecompress)->GetFieldString(APE_TAG_FIELD_YEAR, buffer, &chars) == 0)
    //        target.SetYear(buffer);

    //    chars = 256;
    //    if (GET_TAG(m_pAPEDecompress)->GetFieldString(APE_TAG_FIELD_COMMENT, buffer, &chars) == 0)
    //        target.SetNotes(buffer);

    //    chars = 256;
    //    if (GET_TAG(m_pAPEDecompress)->GetFieldString(APE_TAG_FIELD_TRACK, buffer, &chars) == 0)
    //        target.SetTrackNum(buffer);

        target.SetBitrate(musikCore::IntToString(m_pAPEDecompress->GetInfo(APE_INFO_AVERAGE_BITRATE)));
        target.SetDuration(musikCore::IntToString(m_pAPEDecompress->GetInfo(APE_DECOMPRESS_LENGTH_MS)));

        delete m_pAPEDecompress;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////

