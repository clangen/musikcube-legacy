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
#include <io.h>
#include <time.h>

#include "mp4.h"

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***

#define PLUGIN_NAME _T("core_m4a")
#define PLUGIN_VERSION _T("0.03")
#define PLUGIN_DESCRIPTION _T("MPEG 4 Audio format plugin")

musikCore::String g_Name = PLUGIN_NAME;                     // your plugin's name
musikCore::String g_Description = PLUGIN_DESCRIPTION;       // your plugin's description
musikCore::String g_Formats = _T("mp4,aac,m4a,m4b");      // list of supported extensions, comma separated
bool g_CanConfigure = true;
bool g_CanAbout = true;
HPLUGIN handle = 0;
HPLUGIN handleAlac = 0;

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
{
    // return normal stuffs
    return g_Formats;
}

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
    wsprintf(text,L"%s - %s\nby Otto",PLUGIN_NAME,PLUGIN_VERSION);
    wsprintf(title,L"%s %s",PLUGIN_NAME,PLUGIN_VERSION);
    MessageBox(NULL,text,title,MB_OK);
}

///////////////////////////////////////////////////

// optional

void OnPluginAttach()
{
    wchar_t buffer[MAX_PATH+1];
    ::GetModuleFileName(NULL, buffer, sizeof(buffer));
    std::wstring currentDirectory = buffer;

    size_t pos = currentDirectory.rfind('\\');
    currentDirectory = currentDirectory.substr(0, pos + 1);

    if (handle == 0)
    {
        std::wstring aacDll = currentDirectory + L"bass_aac.dll";
        handle = BASS_PluginLoad((char*)aacDll.c_str(), BASS_UNICODE);
    }

    if (handleAlac == 0)
    {
        std::wstring alacDll = currentDirectory + L"bass_alac.dll";
        handleAlac = BASS_PluginLoad((char*)alacDll.c_str(), BASS_UNICODE);
    }
}

///////////////////////////////////////////////////

// optional

void OnPluginDetach()
{
    if (handle != 0)
    {
        BASS_PluginFree(handle); 
        handle = 0;
    }
    if (handleAlac != 0)
    {
        BASS_PluginFree(handleAlac);
        handle = 0;
    }
}

///////////////////////////////////////////////////

bool CanPlay(const musikCore::String& filename)
{
    musikCore::Filename MFN(filename);
    musikCore::String extension = MFN.GetExtension();

    if (
        extension == _T("m4a")
        || extension == _T("mp4")
        || extension == _T("aac")
        || extension == _T("m4b")        
//        || extension == _T("m4p") // TODO: Add support for this later, maybe?
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

    if (offset && CanSeek())
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
    float pos = ms;
    pos /= 1000;

    QWORD seekpos = BASS_ChannelSeconds2Bytes(stream, pos);
    int set = BASS_ChannelSetPosition(stream, seekpos);
    return (set == 1);
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

    // any additional cleanup here

    return true;
}

///////////////////////////////////////////////////

int GetDuration(HSTREAM stream)
{
    // get duration in milliseconds
    QWORD len=BASS_ChannelGetLength(stream); // length in bytes
    float dur=BASS_ChannelBytes2Seconds(stream,len); // the time length
    return (int)(dur * 1000);
}

///////////////////////////////////////////////////

int GetTime(HSTREAM stream)
{
    // return current time in milliseconds
    QWORD pos = BASS_ChannelGetPosition(stream); 
    float time = BASS_ChannelBytes2Seconds(stream, pos);
    return (int)(time * 1000);
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
    // load your file, then use BASS_StreamCreate(...)
    // to create a new stream, and return it. 
    HSTREAM load = NULL;

    musikCore::Filename fn(filename);

    // standard BASS formats
    if (fn.GetExtension() == _T("aac") 
        || fn.GetExtension() == _T("mp4")
        || fn.GetExtension() == _T("m4a")
        || fn.GetExtension() == _T("m4b")
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

bool ReadTag(const musikCore::String& fn, musikCore::SongInfo& target)
{    
    bool ret = false;

    try
    {
        // set the filename and format
        musikCore::Filename mfn(fn);
        target.SetFilename(fn);
        musikCore::String extension = mfn.GetExtension();
        target.SetFormat(extension);
        
        // check that this is an MPEG 4 file
        if (  extension == _T("m4a")
            || extension == _T("mp4")
            || extension == _T("m4b")   )
        {
            // open the file
            MP4FileHandle file = NULL;
            file = MP4ReadW(fn.c_str(), 0);
            if (!file) return false;

            // find the first audio track in the file
            MP4TrackId track = MP4FindTrackId(file,0,MP4_AUDIO_TRACK_TYPE);
            if (!track) return false;

            // get the duration
            int duration = MP4ConvertFromTrackDuration(file,track,MP4GetTrackDuration(file,track),MP4_MILLISECONDS_TIME_SCALE);
            target.SetDuration(musikCore::IntToString(duration));

            // holder for the tags
            char * tagdata;

            // get the title
            tagdata = NULL;
            if (MP4GetMetadataName(file, &tagdata)) 
            {
                target.SetTitle(musikCore::utf8to16(tagdata));
                free(tagdata);
            }
            else
            {
                // if no title, use the filename instead
                target.SetTitle(mfn.GetJustFilename());
            }
            // get the artist
            tagdata = NULL;
            if (MP4GetMetadataArtist(file, &tagdata)) 
            {
                target.SetArtist(musikCore::utf8to16(tagdata));
                free(tagdata);
            }
            // get the album
            tagdata = NULL;
            if (MP4GetMetadataAlbum(file, &tagdata)) 
            {
                target.SetAlbum(musikCore::utf8to16(tagdata));
                free(tagdata);
            }
            // get the year
            tagdata = NULL;
            if (MP4GetMetadataYear(file, &tagdata))
            {
                target.SetYear(musikCore::utf8to16(tagdata));
                free(tagdata);
            }
            // get the genre
            tagdata = NULL;
            if (MP4GetMetadataGenre(file, &tagdata))
            {
                target.SetGenre(musikCore::utf8to16(tagdata));
                free(tagdata);
            }
            // get the comment
            tagdata = NULL;
            if (MP4GetMetadataComment(file, &tagdata)) 
            {
                target.SetNotes(musikCore::utf8to16(tagdata));
                free(tagdata);
            }
            // get the tracknum
            uint16_t tracknum, totaltracks;
            if (MP4GetMetadataTrack(file, &tracknum, &totaltracks)) 
            {
                char temp[32];
                sprintf(temp,"%d/%d",tracknum,totaltracks);
                target.SetTrackNum(temp);
            }

            // get the avg bitrate
            unsigned int bitrate = MP4GetTrackBitRate(file,track);
            target.SetBitrate(musikCore::IntToString(bitrate/1000));

            int type = MP4GetTrackAudioMpeg4Type(file,track);

            //close and cleanup
            MP4Close(file);
            ret = true;
        }
        else
        {
            // AAC file has no tags.. TODO: consider getting better info here
            target.SetTitle(mfn.GetJustFilename());
            ret = true;
        }
    }
    catch (...)
    {
        ret = false;
    }
    return ret;
}


bool WriteTag(const musikCore::SongInfo& info)
{
    bool ret = false;
/*    
    try
    {
        mp4ff_metadata_t tags;
    
        // init the tags
        tags.count = 0;
        tags.tags = NULL;

        // set the filename and format
        musikCore::Filename mfn(info.GetFilename());
        musikCore::String extension = mfn.GetExtension();
        
        // check that this is an MPEG 4 file
        if (!(extension == _T("m4a")
            || extension == _T("mp4")
            || extension == _T("m4b")   ))
        {
            // *.AAC files have no tags
            return true;
        }

        // open the file
        FILE * file = _tfopen(info.GetFilename(),_T("r+b"));
        if (!file) throw;

        // mp4ff stuffs
        mp4ff_t *mp4file = NULL;
        mp4ff_callback_t * mp4cb = NULL;

        // setup the callback
        mp4cb = getMP4FF_cb(file);
        if (!mp4cb)
        {
            fclose(file);
            throw;
        }

        // read the file in
        mp4file = mp4ff_open_read(mp4cb);
        if (mp4file == NULL)
        {
            fclose(file);
            throw;
        }

        // read the existing tags from the file
        ReadMP4Tag(mp4file,&tags);

        // modify the tags

        musikCore::String tag;
        string temp;
        tag = info.GetTitle();
        temp = musikCore::utf16to8(tag);
        tag_set_field(&tags,"title", temp.c_str(), temp.length());
        tag = info.GetArtist();
        temp = musikCore::utf16to8(tag);
        tag_set_field(&tags,"artist", temp.c_str(), temp.length());
        tag = info.GetAlbum();
        temp = musikCore::utf16to8(tag);
        tag_set_field(&tags,"album", temp.c_str(), temp.length());
        tag = info.GetGenre();
        temp = musikCore::utf16to8(tag);
        tag_set_field(&tags,"genre", temp.c_str(), temp.length());
        tag = info.GetYear();
        temp = musikCore::utf16to8(tag);
        tag_set_field(&tags,"date", temp.c_str(), temp.length());
        tag = info.GetTrackNum();
        temp = musikCore::utf16to8(tag);
        tag_set_field(&tags,"track", temp.c_str(), temp.length());
        tag = info.GetNotes();
        temp = musikCore::utf16to8(tag);
        tag_set_field(&tags,"comment", temp.c_str(), temp.length());

        mp4ff_meta_update(mp4cb, &tags);

        //close and cleanup
        free(mp4cb);
        tag_delete(&tags);
        mp4ff_close(mp4file);
        fclose(file);
        ret = true;
    }
    catch (...)
    {
        ret = false;
    }
*/    
    return ret;
}

int gettimeofday(struct timeval *t, void *)
{
    if (!t)
    {
        return -1;
    }

    t->tv_sec = time(NULL);
    t->tv_usec = t->tv_sec * 1000000;    // lets hope this doesn't break anything

    return 0;
}
