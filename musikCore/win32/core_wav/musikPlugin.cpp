///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPlugin.h"

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***
#define PLUGIN_NAME _T("core_wav")
#define PLUGIN_VERSION _T("0.04")
#define PLUGIN_DESCRIPTION _T("WAV/AIFF File Format")

musikCore::String g_Name = PLUGIN_NAME;
musikCore::String g_Description = PLUGIN_DESCRIPTION;
musikCore::String g_Formats = _T("wav,aif,aiff");       // list of supported extensions, comma separated
bool g_CanConfigure = false;
bool g_CanAbout = true;

///////////////////////////////////////////////////

// these will get initialized automatically by musikCube

HMODULE g_Instance = NULL;
HSYNC g_Sync = NULL;

///////////////////////////////////////////////////

// specific to THIS plugin

///////////////////////////////////////////////////

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved  )
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
    WCHAR text[1024];
    WCHAR title[32];
    wsprintf(text,L"%s - %s\n%s\nby Otto",PLUGIN_NAME, PLUGIN_VERSION, PLUGIN_DESCRIPTION);
    wsprintf(title,L"%s %s",PLUGIN_NAME,PLUGIN_VERSION);
    MessageBox(NULL,text,title,MB_OK);
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
    if (MFN.GetExtension() == _T("wav")
        || MFN.GetExtension() == _T("aif")
        || MFN.GetExtension() == _T("aiff")
        )
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
    QWORD bytelength = BASS_ChannelGetLength(stream);
    float secs = BASS_ChannelBytes2Seconds(stream, bytelength); 
    secs *= 1000; // to ms
    return (int)secs;
}

///////////////////////////////////////////////////

int GetTime(HSTREAM stream)
{
    QWORD pos = BASS_ChannelGetPosition(stream);
    float secs = BASS_ChannelBytes2Seconds(stream, pos);
    secs *= 1000.0f;

    return abs(secs);
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

HSTREAM LoadFile(const musikCore::String& filename)
{
    HSTREAM load = NULL;
    musikCore::Filename fn(filename);

    // standard BASS formats
    if (fn.GetExtension() == _T("wav") 
         || fn.GetExtension() == _T("aif") 
         || fn.GetExtension() == _T("aiff") 
        )
    {
        load = BASS_StreamCreateFile(
            FALSE,
            filename.c_str(),
            0,
            0,
            BASS_STREAM_AUTOFREE | BASS_UNICODE);
    }

    if (load)
        BASS_ChannelPreBuf(load , 500);
    
    return load;
}

///////////////////////////////////////////////////

bool WriteTag(const musikCore::SongInfo& info)
{
    // WAV files don't have any tags, so this does nothing
    return true;
}

///////////////////////////////////////////////////

bool ReadTag(const musikCore::String& fn, musikCore::SongInfo& target)
{
    // WAV files don't have any tags, so this sticks the filename
    // into the song title field and leaves the rest blank.

    musikCore::Filename mfn(fn);
    target.SetFilename(fn);
    target.SetFormat(mfn.GetExtension());
    target.SetTitle(mfn.GetJustFilenameNoExt());

    HSTREAM s = 0;
    s = LoadFile(fn);
    if (s != 0)
    {
        // get the bitrate and such
        BASS_CHANNELINFO chaninfo;
        if (BASS_ChannelGetInfo(s, &chaninfo) == TRUE)
        {
            int bitrate = chaninfo.freq * chaninfo.chans * chaninfo.origres;
            target.SetBitrate(musikCore::IntToString(bitrate/1000));
        }
        // get the duration
        QWORD bytelength = BASS_ChannelGetLength(s);
        float secs = BASS_ChannelBytes2Seconds(s, bytelength); 
        target.SetDuration(musikCore::IntToString((int)(secs * 1000)));

        BASS_StreamFree(s);
    }


    return true;
}

///////////////////////////////////////////////////

