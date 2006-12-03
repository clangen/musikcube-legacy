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

#ifndef C_MUSIK_PLAYLIST_H
#define C_MUSIK_PLAYLIST_H

///////////////////////////////////////////////////

#include "musikConfig.h"

#include <sqlite3.h>
#include <vector>
#include <iostream>

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

class Library;
class LibraryCallbacks;
class Player;

///////////////////////////////////////////////////

/**
* Once the application initializes it's musikCore::Player derived 
* object, he needs to call musikCore::SetPlayer() to let musikCore
* know where it is located. musikCore will use the reference
* to this object to do various things, specifically querying
* plugins for tag information. 
*
* \param library address of the active musikCore::Library object.
*/
MUSIKEXPORT void SetPlayer(Player* player);

/**
 * Retrieve a pointer to the player used by the application, as set
 * with musikCore::SetPlayer().
 *
 * \return a pointer to the application's Player.
 */
MUSIKEXPORT Player* GetPlayer();
extern musikCore::Player* g_SongPlayer;

///////////////////////////////////////////////////

class MUSIKEXPORT Song
{
    friend class    Library;
    friend class    LibraryCallbacks;
    friend class    Player;

public:

    /*ctor*/        Song(Library* owner);
    /*ctor*/        Song();
    /*dtor*/        ~Song();

    int             GetFilesize() const                     { return m_Filesize; }
    int             GetDuration() const                     { return m_Duration; }
    String          GetField(int field) const;

    Library*        GetLibrary()                            { return m_Library; }
    void            SetLibrary(Library* library)            { m_Library = library;    }

    int             GetID() const                           { return m_ID; }
    void            SetID(int id, bool requery = false);

protected:

    void            SetDuration(int duration)               { m_Duration = duration; }
    void            SetFilesize(int filesize)               { m_Filesize = filesize; }

    int             m_Duration;
    int             m_Filesize;
    int             m_ID;
    Library*        m_Library;
};

typedef MUSIKEXPORT std::vector<Song> SongArray;

///////////////////////////////////////////////////

class MUSIKEXPORT PlaylistInfo
{
public:
    /*ctor*/            PlaylistInfo();
    /*ctor*/            PlaylistInfo(String name, int type, int id);
    /*dtor*/            ~PlaylistInfo();

    String              GetName() const     { return m_Name; }
    int                 GetType() const     { return m_Type; }
    int                 GetID() const       { return m_ID; }
    int                 GetOrder() const    { return m_Order; }
    String              GetQuery()          { return m_Query; }

    String              m_Name;
    String              m_Query;
    int                 m_Type;
    int                 m_ID;
    int                 m_Order;
};

typedef MUSIKEXPORT std::vector<PlaylistInfo> PlaylistInfoArray;

///////////////////////////////////////////////////

class MUSIKEXPORT Playlist : public SongArray
{
    friend class Library;

public:

    /*ctor*/        Playlist();
    /*dtor*/        ~Playlist();


    // song methods
    String          GetField(int index, int field) const;
    int             GetSongID(int index) const;
    Song            GetSong(int index) const;

    // info methods
    PlaylistInfo    GetPlaylistInfo() const     { return this->m_PlaylistInfo; }
    int             GetID() const               { return this->m_PlaylistInfo.m_ID; }
    int             GetType() const             { return this->m_PlaylistInfo.m_Type; }
    int             GetOrder() const            { return this->m_PlaylistInfo.m_Order; }
    String          GetQuery() const            { return this->m_PlaylistInfo.m_Query; }
    //
    void            SetPlaylistInfo(const PlaylistInfo& info);
    void            SetID(int id)                   { this->m_PlaylistInfo.m_ID = id; }
    void            SetType(int type)               { this->m_PlaylistInfo.m_Type = type; }
    void            SetOrder(int order)             { this->m_PlaylistInfo.m_Order = order; }
    void            SetQuery(const String& query)   { this->m_PlaylistInfo.m_Query = query; }

    // container methods
    void            Add(const Song& song);
    size_t          GetCount() const;
    void            DeleteAt(size_t index);
    void            InsertAt(Song& song, int index);
    void            InsertBefore(SongArray& songs, int index);
    void            InsertAfter(SongArray& songs, int index);

    void Clear();

    size_t          GetTotalTime() const        { return m_TotalTime; }
    double          GetTotalSize() const        { return m_TotalSize; }

protected:

    void            SetTotalTime(size_t tot)    { m_TotalTime = tot; }
    void            SetTotalSize(double tot)    { m_TotalSize = tot; }
    void            SetField(int index, int field);

    PlaylistInfo    m_PlaylistInfo;
    size_t          m_TotalTime;
    double          m_TotalSize;
};

typedef MUSIKEXPORT std::vector<Playlist> PlaylistArray;

///////////////////////////////////////////////////

class MUSIKEXPORT SongInfo
{
    friend class Library;
    friend class Player;

public:

    /*ctor*/        SongInfo(Library* owner);
    /*ctor*/        SongInfo();
    /*dtor*/        ~SongInfo();

    bool            LoadInfo(const String& fn);
    bool            WriteInfo();

    void            Requery();

    Library*        GetLibrary() const                          { return m_Library; }
    void            SetLibrary(Library* library)                 { m_Library = library;    }
    //
    int             GetID() const                               { return m_ID; }
    void            SetID(int id, bool requery = false);
    String          GetField(int field) const;
    void            SetField(int field, const String& str);
    //
    String          GetFilename() const                         { return m_Filename; }
    void            SetFilename(const String& s)                { m_Filename = s; }
    String          GetTitle() const                            { return m_Title; }
    void            SetTitle(const String& s)                   { m_Title = s; }
    String          GetTrackNum() const                         { return m_TrackNum; }
    void            SetTrackNum(const String& s)                { m_TrackNum = s; }
    String          GetArtist() const                           { return m_Artist; }
    void            SetArtist(const String& s)                  { m_Artist = s; }
    String          GetAlbum() const                            { return m_Album; }
    void            SetAlbum(const String& s)                   { m_Album = s; }
    String          GetGenre() const                            { return m_Genre; }
    void            SetGenre(const String& s)                   { m_Genre = s; }
    String          GetDuration() const                         { return m_Duration; }
    void            SetDuration(const String& s)                { m_Duration = s; }
    String          GetFormat() const                           { return m_Format; }
    void            SetFormat(const String& s)                  { m_Format = s; }
    String          GetVBR() const                              { return m_VBR; }
    void            SetVBR(const String& s)                     { m_VBR = s; }
    String          GetYear() const                             { return m_Year; }
    void            SetYear(const String& s)                    { m_Year = s; }
    String          GetRating() const                           { return m_Rating; }
    void            SetRating(const String& s)                  { m_Rating = s; }
    String          GetBitrate() const                          { return m_Bitrate; }
    void            SetBitrate(const String& s)                 { m_Bitrate = s; }
    String          GetLastPlayed() const                       { return m_LastPlayed; }
    void            SetLastPlayed(const String& s)              { m_LastPlayed = s; }
    String          GetNotes() const                            { return m_Notes; }
    void            SetNotes(const String& s)                   { m_Notes = s; }
    String          GetTimesPlayed() const                      { return m_TimesPlayed; }
    void            SetTimesPlayed(const String& s)             { m_TimesPlayed = s; }
    String          GetTimeAdded() const                        { return m_TimeAdded; }
    void            SetTimeAdded(const String& s)               { m_TimeAdded = s; }
    String          GetFilesize() const                         { return m_Filesize; }
    void            SetFilesize(const String& s)                { m_Filesize = s; }
    String          GetDirtyFlag() const                        { return m_Dirty; }
    void            SetDirtyFlag(const String& s)               { m_Dirty = s; }
    String          GetEqualizer() const                        { return m_Equalizer; }
    void            SetEqualizer(const String& s)               { m_Equalizer = s; }

protected:

    void            SetPlayTime(int playtime){ m_PlayTime = playtime; }
    void            SetUserChoose(){ m_UserChoose = 1; }
    void            SetAutoChoose(){ m_UserChoose = 0; }

    int             m_UserChoose;
    int             m_PlayTime;
    int             m_ID;
    Library*        m_Library;

    String          m_Rating, m_Bitrate, m_Duration, m_Dirty, m_Filesize,
                    m_TimesPlayed, m_TrackNum, m_Format, m_VBR, m_Filename,
                    m_Title, m_Artist, m_Album, m_Year, m_Genre, m_LastPlayed,
                    m_TimeAdded, m_Notes, m_Equalizer;
};

typedef MUSIKEXPORT std::vector<SongInfo> SongInfoArray;

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
