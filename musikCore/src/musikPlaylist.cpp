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

#include "../include/musikPlaylist.h"
#include "../include/musikLibrary.h"
#include "../include/musikFilename.h"
#include "../include/musikPlayer.h"

#if defined (USE_BASS)
#include "../include/musikPlayerBASS.h"
#endif

// taglib is compiled inside the core_Internal.dll 
#if !defined(WIN32) && !defined(__MINGW32__)
#include <taglib/fileref.h>
#include <taglib/tag.h>
#endif

///////////////////////////////////////////////////

namespace musikCore {
    Library* s_SongLibrary = NULL;
    Player* g_SongPlayer = NULL;

    void SetPlayer(Player* player){ musikCore::g_SongPlayer = player; }
    Player* GetPlayer(){ return musikCore::g_SongPlayer; }
    void SetLibrary(Library* library){ musikCore::s_SongLibrary = library; }
    Library* GetLibrary(){ return musikCore::s_SongLibrary; }
}

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// Song: a single song ID

///////////////////////////////////////////////////

Song::Song(Library* lib)
{
    m_ID = -1;
    m_Filesize = 0;
    m_Duration = 0;
    m_Library = lib;
}

///////////////////////////////////////////////////

Song::Song()
{
    m_ID = -1;
    m_Filesize = 0;
    m_Duration = 0;
    m_Library = musikCore::GetLibrary();
}

///////////////////////////////////////////////////

Song::~Song()
{
}

///////////////////////////////////////////////////

String Song::GetField(int field) const
{
    String ret;
    m_Library->GetFieldFromID(m_ID, field, ret);
    return ret;
}

///////////////////////////////////////////////////

void Song::SetID(int id, bool requery)
{ 
    if (requery)
    {
        Song tempSong;
        musikCore::GetLibrary()->GetSongAttrFromID(id, tempSong);

        m_Filesize = tempSong.GetFilesize();
        m_Duration = tempSong.GetDuration();
    }

    m_ID = id;
}

///////////////////////////////////////////////////

// Playlist: a playlist object

///////////////////////////////////////////////////

SongInfo::SongInfo(Library* lib)
{
    m_ID = -1;
    m_UserChoose = 0;
    m_PlayTime = 0;

    if (!lib)
        m_Library = musikCore::GetLibrary();
    else
        m_Library = lib;
}

///////////////////////////////////////////////////

SongInfo::SongInfo()
{
    m_ID = -1;
    m_UserChoose = 0;
    m_PlayTime = 0;
    m_Library = musikCore::GetLibrary();
}

///////////////////////////////////////////////////

SongInfo::~SongInfo()
{

}

///////////////////////////////////////////////////

String SongInfo::GetField(int field) const
{
    switch(field)
    {
    case MUSIK_LIBRARY_TYPE_ARTIST:
        return m_Artist;
        break;
    case MUSIK_LIBRARY_TYPE_ALBUM:
        return m_Album;
        break;
    case MUSIK_LIBRARY_TYPE_YEAR:
        return m_Year;
        break;
    case MUSIK_LIBRARY_TYPE_GENRE:
        return m_Genre;
        break;
    case MUSIK_LIBRARY_TYPE_TITLE:
        return m_Title;
        break;
    case MUSIK_LIBRARY_TYPE_TRACKNUM:
        return m_TrackNum;
        break;
    case MUSIK_LIBRARY_TYPE_TIMEADDED:
        return m_TimeAdded;
        break;
    case MUSIK_LIBRARY_TYPE_LASTPLAYED:
        return m_LastPlayed;
        break;
    case MUSIK_LIBRARY_TYPE_FILESIZE:
        return m_Filesize;
        break;
    case MUSIK_LIBRARY_TYPE_FORMAT:
        return m_Format;
        break;
    case MUSIK_LIBRARY_TYPE_DURATION:
        return m_Duration;
        break;
    case MUSIK_LIBRARY_TYPE_RATING:
        return m_Rating;
        break;
    case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
        return m_TimesPlayed;
        break;
    case MUSIK_LIBRARY_TYPE_BITRATE:
        return m_Bitrate;
        break;
    case MUSIK_LIBRARY_TYPE_FILENAME:
        return m_Filename;
        break;
    case MUSIK_LIBRARY_TYPE_COMMENTS:
        return m_Notes;
        break;
    }

    return m_Filename;
}    

///////////////////////////////////////////////////

void SongInfo::SetID(int id, bool requery)
{
    if (requery)
        Requery();

    m_ID = id;
}

///////////////////////////////////////////////////

void SongInfo::Requery()
{
    if (m_Library)
        m_Library->GetSongInfoFromID(GetID(), *this);
}

///////////////////////////////////////////////////

#if !defined(WIN32) && !defined(__MINGW32__)

///////////////////////////////////////////////////

bool SongInfo::LoadInfo(const String& fn)
{
    bool ret = true;

    musikCore::Filename mfn(fn);

    SetFilename(fn);
    SetFormat(mfn.GetExtension());

    try
    {
#if defined (WIN32)
        TagLib::FileRef tag_file(fn.c_str());
#else    
        TagLib::FileRef tag_file(utf16to8(fn, true).c_str());
#endif
        if (!tag_file.isNull())
        {
            if (tag_file.tag())
            {
                TagLib::Tag *tag = tag_file.tag();        

                SetArtist(utf8to16(tag->artist().to8Bit(true)));
                SetAlbum(utf8to16(tag->album().to8Bit(true)));      
                SetTitle(utf8to16(tag->title().to8Bit(true)));
                SetGenre(utf8to16(tag->genre().to8Bit(true)));
                SetNotes(utf8to16(tag->comment().to8Bit(true)));

                SetYear(IntToString(tag->year()));
                SetTrackNum(IntToString(tag->track()));
            }

            if (tag_file.audioProperties())
            {
                TagLib::AudioProperties *properties = tag_file.audioProperties();

                int duration = properties->length() * 1000;
                if (!duration)
                    duration = Player::GetFileLengthMS(fn);

                SetBitrate(IntToString(properties->bitrate()));
                SetDuration(IntToString(duration));
            }
        }

        // if the title is empty, then use the
        // filename...
        if (GetTitle().IsEmpty())
        {
            Filename MFN(fn);
            SetTitle(MFN.GetJustFilename());
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

bool SongInfo::WriteInfo()
{ 
    bool ret = true;

    try
    {
#if defined (WIN32)
        TagLib::FileRef tag_file(GetFilename().c_str());
#else
        TagLib::FileRef tag_file(utf16to8(GetFilename(), true).c_str());
#endif
        if (!tag_file.isNull())
        {
            TagLib::Tag *tag = tag_file.tag();

            tag->setArtist(GetArtist().c_str());
            tag->setAlbum(GetAlbum().c_str());
            tag->setTitle(GetTitle().c_str());
            tag->setGenre(GetGenre().c_str());
            tag->setYear(StringToInt(GetYear()));
            tag->setTrack(StringToInt(GetTrackNum()));
            tag->setComment(GetNotes().c_str());

            tag_file.save();
        }
    }
    catch (...)
    {
        ret = false;
        cout << "taglib crashed trying to write: " << GetFilename().c_str() << endl;
    }

    return ret;
}

///////////////////////////////////////////////////

#else // !WIN32 && !__MINGW32__

///////////////////////////////////////////////////

// load the tag from the respective plugin

bool SongInfo::LoadInfo(const String& fn)
{
    musikCore::PlayerBASS* player = (musikCore::PlayerBASS*) g_SongPlayer;
    if (!player)
    {
        return false;
    }

    for (size_t i = 0; i < player->s_Plugins.size(); i++)
    {
        if (player->s_Plugins[i]->CanPlay(fn))
        {
            return player->s_Plugins[i]->ReadTag(fn, *this);
        }
    }

    return false;
}

///////////////////////////////////////////////////

bool SongInfo::WriteInfo()
{
    musikCore::PlayerBASS* player = (musikCore::PlayerBASS*) g_SongPlayer;
    if (!player)
    {
        return false;
    }

    for (size_t i = 0; i < player->s_Plugins.size(); i++)
    {
        if (player->s_Plugins[i]->CanPlay(GetFilename()))
        {
            return player->s_Plugins[i]->WriteTag(*this);
        }
    }

    return false;
}

///////////////////////////////////////////////////

#endif // !WIN32 && !__MINGW32__

///////////////////////////////////////////////////

void SongInfo::SetField(int field, const String& str)
{
    switch(field)
    {
    case MUSIK_LIBRARY_TYPE_ARTIST:
        m_Artist = str;
        break;
    case MUSIK_LIBRARY_TYPE_ALBUM:
        m_Album = str;
        break;
    case MUSIK_LIBRARY_TYPE_YEAR:
        m_Year = str;
        break;
    case MUSIK_LIBRARY_TYPE_GENRE:
        m_Genre = str;
        break;
    case MUSIK_LIBRARY_TYPE_TITLE:
        m_Title = str;
        break;
    case MUSIK_LIBRARY_TYPE_TRACKNUM:
        m_TrackNum = str;
        break;
    case MUSIK_LIBRARY_TYPE_TIMEADDED:
        m_TimeAdded = str;
        break;
    case MUSIK_LIBRARY_TYPE_LASTPLAYED:
        m_LastPlayed = str;
        break;
    case MUSIK_LIBRARY_TYPE_FILESIZE:
        m_Filesize = str;
        break;
    case MUSIK_LIBRARY_TYPE_FORMAT:
        m_Format = str;
        break;
    case MUSIK_LIBRARY_TYPE_DURATION:
        m_Duration = str;
        break;
    case MUSIK_LIBRARY_TYPE_RATING:
        m_Rating = str;
        break;
    case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
        m_LastPlayed = str;
        break;
    case MUSIK_LIBRARY_TYPE_BITRATE:
        m_Bitrate = str;
        break;
    case MUSIK_LIBRARY_TYPE_FILENAME:
        m_Filename = str;
        break;
    }
}

///////////////////////////////////////////////////

// Playlist: a playlist object

///////////////////////////////////////////////////

Playlist::Playlist()
{
    m_TotalTime = 0;
    m_TotalSize = 0.0;
}

///////////////////////////////////////////////////

Playlist::~Playlist()
{
}

///////////////////////////////////////////////////

String Playlist::GetField(int index, int field) const
{
    return at(index).GetField(field);
}

///////////////////////////////////////////////////

void Playlist::Clear()
{
    clear();

    m_TotalTime = 0;
    m_TotalSize = 0.0;
}

///////////////////////////////////////////////////

void Playlist::Add(const Song& song)
{ 
    push_back(song); 
    m_TotalTime += song.GetDuration();
    m_TotalSize += (double)song.GetFilesize();
}

///////////////////////////////////////////////////

size_t Playlist::GetCount() const
{
    return size();
}

///////////////////////////////////////////////////

int Playlist::GetSongID(int index) const 
{
    if (index >= (int)size() || index < 0)
        return -1;

    return at(index).GetID();
}

///////////////////////////////////////////////////

Song Playlist::GetSong(int index) const
{
    if (index >= (int)size() || index < 0)
    {
        Song bad;
        return bad;
    }

    return at(index);
}

///////////////////////////////////////////////////

void Playlist::DeleteAt(size_t pos)
{
    m_TotalTime -= at(pos).GetDuration();
    m_TotalSize -= (double)at(pos).GetFilesize();
    erase(begin() + pos);
}

///////////////////////////////////////////////////

void Playlist::InsertAt(Song& song, int at)
{
    if (at == -1)
        push_back(song);
    else
        insert(begin() + at, song);

    m_TotalTime += song.GetDuration();
    m_TotalSize += (double)song.GetFilesize();
}

///////////////////////////////////////////////////

void Playlist::InsertBefore(SongArray& songs, int pos)
{
    for (size_t i = 0; i < songs.size(); i++)
    {
        if (pos == -1)
            push_back(songs.at(i));
        else
        {
            insert(begin() + pos, songs.at(i));
            pos++;
        }

        m_TotalTime += songs.at(i).GetDuration();
        m_TotalSize += (double)songs.at(i).GetFilesize();
    }
}

///////////////////////////////////////////////////

void Playlist::InsertAfter(SongArray& songs, int pos)
{
    pos++;

    if (size() == 0)
        pos = 0;

    if ((size_t)pos > size() + 1 && size() != 0)
        pos = size() + 1;

    for (size_t i = 0; i < songs.size(); i++)
    {
        if (pos == -1)
            push_back(songs.at(i));
        else
        {
            insert(begin() + pos, songs.at(i));
            pos++;
        }

        m_TotalTime += songs.at(i).GetDuration();
        m_TotalSize += (double)songs.at(i).GetFilesize();
    }
}


///////////////////////////////////////////////////

void Playlist::SetPlaylistInfo(const PlaylistInfo& playlistInfo)
{
    this->m_PlaylistInfo = playlistInfo;
}

///////////////////////////////////////////////////

// PlaylistInfo

///////////////////////////////////////////////////

PlaylistInfo::PlaylistInfo()
{
    m_Type = -1;
    m_ID = -1;
    m_Order = -1;
    m_Query = L"";
}

///////////////////////////////////////////////////

PlaylistInfo::PlaylistInfo(String name, int type, int id)
{
    m_Name = name;
    m_Type = type;
    m_ID = id;
    m_Query = L"";
}

///////////////////////////////////////////////////

PlaylistInfo::~PlaylistInfo()
{
}

///////////////////////////////////////////////////
