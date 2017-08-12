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
#include "../include/musikPlayerBASS.h"

using namespace musikCore;

///////////////////////////////////////////////////

#if defined(USE_BASS)

std::vector<Plugin*> PlayerBASS::s_Plugins = std::vector<Plugin*>();

///////////////////////////////////////////////////

namespace musikCore
{
    static void CALLBACK BASS_EQ_Callback(HDSP handle, DWORD channel, void *buffer, DWORD length, DWORD user)
    {
        Stream* stream = (Stream*)user;
        if (stream)
        {
            BASS_CHANNELINFO info;
            BASS_ChannelGetInfo(channel, &info);
            float seconds = BASS_ChannelBytes2Seconds(channel, length);    // number of seconds this buffer is
            float total_samples = seconds * (float)info.freq;

            char*buf = (char*)buffer;
            stream->m_Shibatch.equ_modifySamples((char*)buffer, (int)total_samples, info.chans, 16);
        }
    }
}

///////////////////////////////////////////////////

// returns true if the metadata gotten from the stream actually made anything change

bool GetMetaData(const char* meta_tags, PlayerBASS* player, HSTREAM channel)
{
    if (!player)
        return false;

    // store the previous values
    musikCore::String prev_artist = player->GetPlaying()->GetArtist();
    musikCore::String prev_album = player->GetPlaying()->GetAlbum();
    musikCore::String prev_title = player->GetPlaying()->GetTitle();

    // its a good idea to check for ogg tags first.
    const char* tags = BASS_ChannelGetTags(channel, BASS_TAG_OGG);
    if (tags)
    {
        bool artist = false, album = false, title = false;
        while (*tags)
        {
            musikCore::String tag = tags;
            if (tag.Left(7).ToLower() == _T("artist="))
            {
                player->GetPlaying()->SetArtist(tag.Right(tag.GetLength() - 7));
                artist = true;
            }
            else if (tag.Left(6).ToLower() == _T("title="))
            {
                player->GetPlaying()->SetTitle(tag.Right(tag.GetLength() - 6));
                title = true;
            }
            else if (tag.Left(6).ToLower() == _T("album="))
            {
                player->GetPlaying()->SetAlbum(tag.Right(tag.GetLength() - 6));
                album = true;
            }

            tags += strlen(tags) + 1 ; // move on to next tag
        }

        if (!artist)
            player->GetPlaying()->SetArtist(_T("netstream"));
        if (!album)
            player->GetPlaying()->SetAlbum(_T(""));
        if (!title)
            player->GetPlaying()->SetArtist(player->GetPlaying()->GetFilename());
    }
    else if (meta_tags)
    {
        bool title = false, artist = false;
        while (*meta_tags)
        {
            musikCore::String tag = meta_tags;
            if (tag.Left(13).ToLower() == _T("streamtitle='"))
            {
                int pos = tag.Find(wchar_t(';'));
                tag = tag.Left(pos - 1);
                tag = tag.Right(tag.GetLength() - 13);
                player->GetPlaying()->SetTitle(tag);
                title = true;
            }
            else if (tag.Left(9).ToLower() == _T("icy-name:"))
            {
                if (title)
                {
                    artist = true;
                    player->GetPlaying()->SetArtist(tag.Right(tag.GetLength() - 9));
                }
                else
                {
                    title = true;
                    player->GetPlaying()->SetTitle(tag.Right(tag.GetLength() - 9));
                }
            }

            meta_tags += strlen(meta_tags) + 1; // move on to next tag
        }

        player->GetPlaying()->SetAlbum(_T(""));
        if (!artist)
            player->GetPlaying()->SetArtist(_T(""));
        if (!title)
            player->GetPlaying()->SetTitle(player->GetPlaying()->GetFilename());
    }

    // check to see if the strings actually changed
    if (!prev_title.Equals(player->GetPlaying()->GetTitle()))
        return true;
    if (!prev_artist.Equals(player->GetPlaying()->GetArtist()))
        return true;
    if (!prev_album.Equals(player->GetPlaying()->GetAlbum()))
        return true;

    return false;
}

///////////////////////////////////////////////////

void CALLBACK BASS_NetStream_Tag(HSYNC handle, DWORD channel, DWORD data, DWORD user)
{
    PlayerBASS* player = (PlayerBASS*)user;

    if (!user)
        return;

    if (GetMetaData((char*)data, player, channel))
        player->MetaDataChanged();
}

///////////////////////////////////////////////////

PlayerBASS::PlayerBASS(Functor* functor, Library* library)
: Player(functor, library)
{
    LoadPlugins();

    // create a static list of known formats
    for (size_t i = 0; i < s_Plugins.size(); i++)
    {
        String formats = s_Plugins.at(i)->GetPluginFormats();

        StringArray target;
        Filename::DelimitStr(formats, _T(","), target, false, true, true);

        for (size_t j = 0; j < target.size(); j++)
        {
            musikCore::g_PlayerFormats.push_back(target.at(j).ToLower());
        }
    }
}

///////////////////////////////////////////////////

PlayerBASS::~PlayerBASS()
{
    UnloadPlugins();
}

///////////////////////////////////////////////////

HSTREAM PlayerBASS::GetCurrHandle()
{
    if (GetCurrStream())
    {
        return GetCurrStream()->GetStreamID();
    }

    return 0;
}

///////////////////////////////////////////////////

void PlayerBASS::GetSpectrum(float* target)
{
    Stream* ptrStr = GetCurrStream();
    if (ptrStr)
    {
        BASS_ChannelGetData(
            ptrStr->GetStreamID(),
            (void*)target,
            BASS_DATA_FFT1024);
    }
}

///////////////////////////////////////////////////

Stream* PlayerBASS::LoadNetStream()
{
    for (size_t i = 0; i < s_Plugins.size(); i++)
    {
        if (s_Plugins[i]->CanPlay(m_CurrSong.GetFilename()))
        {
            Plugin* plugin = s_Plugins[i];
            HSTREAM streamID = plugin->LoadFile(m_CurrSong.GetFilename());

            if (streamID != 0)
            {
                Stream* stream = new Stream(plugin, streamID);

                GetMetaData(
                    BASS_ChannelGetTags(
                        stream->GetStreamID(),
                        BASS_SYNC_META),
                    this,
                    streamID);

                BASS_ChannelSetSync(
                    stream->GetStreamID(),
                    BASS_SYNC_META,
                    0,
                    (SYNCPROC*)&BASS_NetStream_Tag,
                    (void *)this);

                return stream;
            }
        }
    }

    // not loaded
    return NULL;
}

///////////////////////////////////////////////////

bool PlayerBASS::PlayStream(Stream* ptrStream, int offset)
{
    if (!ptrStream)
    {
        return false;
    }

    return ptrStream->Play(offset);
}

///////////////////////////////////////////////////

Stream* PlayerBASS::LoadStream(const String& filename)
{
    for (size_t i = 0; i < s_Plugins.size(); i++)
    {
        if (s_Plugins[i]->CanPlay(filename))
        {
            Plugin* plugin = s_Plugins[i];
            HSTREAM streamID = plugin->LoadFile(filename);

            if (streamID != 0)
            {
                Stream* stream = new Stream(plugin, streamID);

                if (IsEqualizerEnabled())
                {
                    stream->m_Shibatch.equ_init(14);  // 14 is the magic number
                    InitEQTable(stream);
                    stream->m_DSP = BASS_ChannelSetDSP(
                        stream->GetStreamID(),
                        (DSPPROC*)musikCore::BASS_EQ_Callback,
                        (void*)stream,
                        0);
                }

                return new Stream(plugin, streamID);
            }
        }
    }

    // not loaded
    return NULL;
}

///////////////////////////////////////////////////

void PlayerBASS::InitEQTable(Stream* ptrStream)
{
    float curr_preamp = musikCore::GetEQPreampLevel();
    float preamp = (96 - curr_preamp) / 96;

    float cleft, cright;
    float left[18], right[18];
    for(int i = 0; i < 18; i++)
    {
        cleft = m_EQSettings.m_Left[i];
        cright = m_EQSettings.m_Right[i];
        left[i] = preamp * ((96 - cleft) / 96) * 1.58;
        right[i] = preamp * ((96 - cright) / 96) * 1.58;
    }

    ptrStream->m_Shibatch.equ_makeTable(left, right, &ptrStream->m_Paramlist, m_Rate);
}

///////////////////////////////////////////////////

void PlayerBASS::OnEQEnable()
{
    if (IsPlaying())
    {
        GetCurrStream()->m_Shibatch.equ_init(14); // 14 is the magic number

        InitEQTable(GetCurrStream());

        GetCurrStream()->m_DSP = BASS_ChannelSetDSP(
            GetCurrStream()->GetStreamID(),
            (DSPPROC*)musikCore::BASS_EQ_Callback,
            (void*)GetCurrStream(),
            0);
    }
}

///////////////////////////////////////////////////

void PlayerBASS::OnEQUpdate()
{
    for (size_t i = 0; i < m_ActiveStreams.size();i ++)
    {
        InitEQTable(m_ActiveStreams[i]);
    }
}

///////////////////////////////////////////////////

void PlayerBASS::OnEQDisable()
{
    if (IsPlaying())
    {
        for (size_t i = 0; i < m_ActiveStreams.size(); i++)
        {
            if (m_ActiveStreams[i]->m_DSP)
            {
                BASS_ChannelRemoveDSP(
                    m_ActiveStreams[i]->GetStreamID(),
                    m_ActiveStreams[i]->m_DSP);
                m_ActiveStreams[i]->m_Shibatch.equ_quit();
                m_ActiveStreams[i]->m_DSP = 0;
            }
        }
    }
}

///////////////////////////////////////////////////

int PlayerBASS::GetStreamDuration(Stream* ptrStream)
{
    if (!ptrStream)
    {
        return 0;
    }

    return ptrStream->GetDuration();
}

///////////////////////////////////////////////////

bool PlayerBASS::SetStreamVolume(Stream* ptrStream, int volume)
{
    BASS_ChannelSetAttribute(
        ptrStream->GetStreamID(),
        BASS_ATTRIB_VOL,
        (float)volume / 255.0f);

    return true;
}

///////////////////////////////////////////////////

int PlayerBASS::GetStreamVolume(Stream* ptrStream)
{
    float volume;
    BASS_ChannelGetAttribute(
        ptrStream->GetStreamID(),
        NULL,
        &volume);

    return (int)((float)volume * 255.0f);
}

///////////////////////////////////////////////////

bool PlayerBASS::SetStreamPos(Stream* ptrStream, int offset)
{
    if (!ptrStream || !ptrStream->CanSeek())
    {
        return false;
    }

    return ptrStream->Seek(offset);
}

///////////////////////////////////////////////////

int PlayerBASS::GetStreamPos(Stream* ptrStream)
{
    if (!ptrStream)
    {
        return 0;
    }

    return ptrStream->GetTime();
}

///////////////////////////////////////////////////

bool PlayerBASS::StreamPause(Stream* ptrStream)
{
    if (!ptrStream)
    {
        for (size_t i = 0; i < m_ActiveStreams.size(); i++)
        {
            m_ActiveStreams[i]->Pause();
        }
    }
    else
    {
        return ptrStream->Pause();
    }

    return true;
}

///////////////////////////////////////////////////

bool PlayerBASS::StreamResume(Stream* ptrStream)
{
    if (!ptrStream)
    {
        if (GetCurrStream())
        {
            return GetCurrStream()->Resume();
        }
    }

    return ptrStream->Resume();
}

///////////////////////////////////////////////////

void PlayerBASS::OnDeleteStream(Stream* ptrStream)
{
    if (ptrStream->m_DSP)
    {
        BASS_ChannelRemoveDSP(ptrStream->GetStreamID(), ptrStream->m_DSP);
        ptrStream->m_Shibatch.equ_quit();
        ptrStream->m_DSP = 0;
    }

    ptrStream->Stop();

    delete ptrStream;
}

///////////////////////////////////////////////////

int PlayerBASS::InitSoundSystem(int device, int driver, int rate)
{
    BASS_SetConfig(BASS_CONFIG_DEV_DEFAULT, 1);
    if (BASS_Init(driver, rate, 0, NULL, NULL))
    {
        return MUSIK_PLAYER_INIT_SUCCESS;
    }

    return MUSIK_PLAYER_INIT_ERROR;
}

///////////////////////////////////////////////////

void PlayerBASS::UpdateNetstreamStatus()
{

}

///////////////////////////////////////////////////

void PlayerBASS::DeinitSoundSystem()
{
    BASS_Free();
}

///////////////////////////////////////////////////

void PlayerBASS::GetSoundDevices(StringArray& target)
{
}

///////////////////////////////////////////////////

void PlayerBASS::GetSoundDrivers(StringArray& target)
{
    int count = 1;

    BASS_DEVICEINFO info;
    BOOL result = BASS_GetDeviceInfo(0, &info);

    while (result)
    {
        target.push_back(info.name);
        result = BASS_GetDeviceInfo(count++, &info);
    }
}

///////////////////////////////////////////////////

Stream* PlayerBASS::LoadCDTrack(int track)
{
    // the cdda plugins expects a delimited string with drive number and track number.
    String cddaFilename;
    cddaFilename.Format(_T("cdda, %d, %d") , m_DiscDrive, track);

    for (size_t i = 0; i < s_Plugins.size(); i++)
    {
        if (s_Plugins[i]->CanPlay(cddaFilename))
        {
            Plugin* plugin = s_Plugins[i];
            HSTREAM streamID = plugin->LoadFile(cddaFilename);

            if (streamID != 0)
            {
                return new Stream(plugin, streamID);
            }
        }
    }

    // not loaded
    return NULL;
}

///////////////////////////////////////////////////

bool PlayerBASS::IsStreamActive(Stream* pStream)
{
    if (!pStream)
    {
        return false;
    }

    return pStream->IsActive();
}

///////////////////////////////////////////////////

// a static function to use the audio decoder
// to retrieve the song's length

int PlayerBASS::GetFileDuration(const String& filename)
{
    Stream* str = LoadStream(filename);
    int secs = str->GetDuration();
    delete str;

    return secs;
}

///////////////////////////////////////////////////

void PlayerBASS::SetBufferSize(int msecs)
{
    BASS_SetConfig(
        BASS_CONFIG_BUFFER,
        (DWORD)msecs);
}

///////////////////////////////////////////////////

void PlayerBASS::LoadPlugins()
{
    wchar_t buffer[2000];
    GetModuleFileName(NULL, buffer, sizeof(buffer));
    String path = buffer;

    path = path.Left(path.ReverseFind('\\'));    // remove filename, we only want the directory
    path += _T("\\formats\\*.*");

    StringArray dllfiles;
    FindPlugins(path, &dllfiles);

    for(size_t i = 0; i < dllfiles.size(); i++)
    {
        Plugin* curr = new Plugin(dllfiles[i]);
        if (curr->IsValid())
        {
            s_Plugins.push_back(curr);
        }
        else
        {
            delete curr;
        }
    }
}

///////////////////////////////////////////////////

void PlayerBASS::UnloadPlugins()
{
    for(size_t i = 0; i < s_Plugins.size(); i++)
    {
        delete s_Plugins[i];
    }

    s_Plugins.clear();
}

///////////////////////////////////////////////////

void PlayerBASS::FindPlugins(String dir, StringArray* target)
{
    // stopping conditions
    if (dir.IsEmpty() || !target)
        return;

    // find the first file in the directory...
    WIN32_FIND_DATA lpFindFileData;
    HANDLE hDir = FindFirstFile(dir.c_str(), &lpFindFileData);

    // assure the first file exists...
    if (hDir != INVALID_HANDLE_VALUE)
    {
        musikCore::String fn, temp;
        musikCore::Filename MFN;
        do
        {
            if (lpFindFileData.cFileName[0] != '.')
            {
                // file is a directory
                if (lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    fn = dir.Left(dir.GetLength() - 3);    // remove *.* from full path
                    fn += lpFindFileData.cFileName;            // append to make new path

                    if (fn.Left(0) != _T("\\"))
                        fn += _T("\\*.*");                    // add \\*.* to the end of the new path
                    else
                        fn += _T("*.*");                        // add *.* to the end of the new path

                    FindPlugins(fn, target);
                }

                // file is a file...
                else
                {
                    fn = dir.Left(dir.GetLength() - 3);    // remove *.* from full path...
                    fn += lpFindFileData.cFileName;            // append filename

                    MFN.SetFilename(fn);
                    if (MFN.GetExtension() == _T("dll"))
                        target->push_back(fn);
                }
            }

        }
        while (FindNextFile(hDir, &lpFindFileData));

        FindClose(hDir);
    }
}

///////////////////////////////////////////////////

void PlayerBASS::MetaDataChanged()
{
    m_Functor->OnNewSong();
}

///////////////////////////////////////////////////

#endif // USE_BASS

///////////////////////////////////////////////////
