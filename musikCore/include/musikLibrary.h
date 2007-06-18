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

#ifndef C_MUSIK_LIBRARY_H
#define C_MUSIK_LIBRARY_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikPlaylist.h"
#include "musikCrossfader.h"
#include "musikEQSettings.h"
#include "musikTaskQueue.h"

#include <sqlite3.h>
#include <OpenThreads/Mutex>
using namespace OpenThreads;

#include <iostream>

///////////////////////////////////////////////////

#define MUSIK_PREV_SONG 2

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

MUSIKEXPORT enum
{
    MUSIK_LIBRARY_VERSION = 9
};

///////////////////////////////////////////////////

MUSIKEXPORT enum
{
    MUSIK_LIBRARY_TYPE_NONE = -1,        // -1
    MUSIK_LIBRARY_TYPE_ARTIST,            // 0
    MUSIK_LIBRARY_TYPE_ALBUM,            // 1
    MUSIK_LIBRARY_TYPE_YEAR,            // 2
    MUSIK_LIBRARY_TYPE_GENRE,            // 3
    MUSIK_LIBRARY_TYPE_TITLE,            // 4
    MUSIK_LIBRARY_TYPE_TRACKNUM,        // 5
    MUSIK_LIBRARY_TYPE_TIMEADDED,        // 6
    MUSIK_LIBRARY_TYPE_LASTPLAYED,        // 7
    MUSIK_LIBRARY_TYPE_FILESIZE,        // 8
    MUSIK_LIBRARY_TYPE_FORMAT,            // 9
    MUSIK_LIBRARY_TYPE_DURATION,        // 10
    MUSIK_LIBRARY_TYPE_RATING,            // 11
    MUSIK_LIBRARY_TYPE_TIMESPLAYED,        // 12
    MUSIK_LIBRARY_TYPE_BITRATE,            // 13
    MUSIK_LIBRARY_TYPE_FILENAME,        // 14
    MUSIK_LIBRARY_TYPE_EQUALIZER,        // 15
    MUSIK_LIBRARY_TYPE_COMMENTS,        // 16
    MUSIK_LIBRARY_FIELD_COUNT
};

///////////////////////////////////////////////////

#define MUSIK_LIBRARY_FORMAT_MP3 _T("mp3")
#define MUSIK_LIBRARY_FORMAT_OGG _T("ogg")
#define MUSIK_LIBRARY_FORMAT_FLAC _T("flac")
#define MUSIK_LIBRARY_FORMAT_NET _T("net")
#define MUSIK_LIBRARY_FORMAT_CDROM _T("cdda")

///////////////////////////////////////////////////

MUSIKEXPORT enum
{
    MUSIK_PLAYLIST_TYPE_UNKNOWN = -1,
    MUSIK_PLAYLIST_TYPE_LIBRARY = 0,
    MUSIK_PLAYLIST_TYPE_STANDARD,
    MUSIK_PLAYLIST_TYPE_DYNAMIC,
    MUSIK_PLAYLIST_TYPE_NOWPLAYING,
    MUSIK_PLAYLIST_TYPE_CDPLAYER,
    MUSIK_PLAYLIST_TYPE_REMOVABLE,
    MUSIK_PLAYLIST_TYPE_PLAYER,
    MUSIK_PLAYLIST_TYPE_COUNT // must be last
};

///////////////////////////////////////////////////

#define    SONG_TABLE_NAME _T("songs")
#define    TEMP_SONG_TABLE_NAME _T("temp_songs")
#define    STD_PLAYLIST_TABLE_NAME _T("std_playlist")
#define    STD_PLAYLIST_SONGS _T("std_playlist_song")
#define    DYN_PLAYLIST_TABLE_NAME _T("dyn_playlist")
#define    CROSSFADER_PRESET _T("crossfader_preset")
#define    CROSSFADER_DEFAULT _T("crossfader_default")
#define    EQUALIZER_PRESET _T("equalizer_preset")
#define    EQUALIZER_DEFAULT _T("equalizer_default")
#define    MANAGED_PATH _T("managed_path")
#define    FILENAME_MASK _T("filename_mask")
#define CHANGE_CASE_TABLE _T("change_case")
#define VERSION_TABLE_NAME _T("library_version")

///////////////////////////////////////////////////

MUSIKEXPORT enum
{
    MUSIK_LIBRARY_OK = SQLITE_OK,
    MUSIK_LIBRARY_ID_EXISTS = -2,
    MUSIK_LIBRARY_NO_ID_EXISTS = -3,
    MUSIK_LIBRARY_NOT_OPEN = -4, 
    MUSIK_LIBRARY_INVALID_PLAYLIST_TYPE = -5
};

///////////////////////////////////////////////////

/**
Library and Player are the heart and soul of musikCore.

Library is a wrapper for an embedded SQL databased based on
sqlite (http://www.sqlite.org). The database stores a variety of 
fields and has native support to manage the follwing:

    - song information
    - standard playlists (list of files)
    - dyanmic playlists (playlists based on sql queries)
    - filename masks for tagging purposes (example: $ARTIST/$ALBUM/$TRACK-$TITLE
    - synchronized directories
    - crossfader settings
    - equalizer settings

This class is thread safe.

@author Casey Langen
@version alpha 0.7
*/
class MUSIKEXPORT Library
{
    // we trust the player
    friend class Player;

public:

    /**
     * \param filename Required. Filename of the database to be managed. If the
     * file does it exist it will be created.
     */
    Library(const String& filename);
    ~Library();

    void SetFilename(const String& filename, bool reinit = true);

    /**
     * Clears the internal database.
     *
     * \param clear_all_tables if true all tables will be cleared, including SONG_TABLE_NAME,
     * STD_PLAYLIST_TABLE_NAME, DYN_PLAYLIST_TABLE_NAME, CROSSFADER_PRESET, CROSSFADER_DEFAULT,
     * EQUALIZER_PRESET, EQUALIZER_DEFAULT, MANAGED_PATH, FILENAME_MASK
     */
    void ClearLibrary(bool clear_all_tables = false);
    /**
     * Remove the specified songs and their respective information from the database
     *
     * \param songs songs to be deleted
     * \param delete_from_disk if true the files will be deleted from the computer as well
     */
    static void DeleteSongs(Playlist& songs);
    void DeleteSong(String& fn);

    /**
     * Sorts the specified playlist by the specified field
     *
     * \param playlist address of the playlist to be sorted
     * \param field field to sort by, MUSIK_LIBRARY_TYPE_xxx
     * \param ascending if true the playlist will be sorted ascending, descending otherwise
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int SortPlaylist(Playlist* playlist, int field, bool ascending = true);

    /**
     * Any thread or function that will perform batch SQL queries (i.e, in a loop) should call
     * this before the batch job begins, and call EndTransaction() when done. BeginTransaction()
     * will keep the database open until EndTransaction() is called. Problems such as extreme
     * slowdowns may occur if the developer forgets to call EndTransaction() when he is done.
     */
    void BeginTransaction();
    /**
     * Any thread or function that will perform batch SQL queries (i.e, in a loop) should call
     * this before the batch job begins, and call EndTransaction() when done. BeginTransaction()
     * will keep the database open until EndTransaction() is called. Problems such as extreme
     * slowdowns may occur if the developer forgets to call EndTransaction() when he is done.
     */
    void EndTransaction();
    /**
     * \return number the number of open transaction. If the library is idle this should be 0.
     */
    int GetOpenTransactions() const;

    /**
     * Clear the values of the temporary song table. 
     */
    void ClearTempSongTable();
    /**
     * Populate the temporary song table with the contents of a playlist. It is up to the user 
     * to populate it and is supplied as a convience to the developer who does not wish to 
     * directly modify the song table.
     *
     * \param source playlist that will be used to populate the table
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    void PopulateTempSongTable(Playlist& source); 

    /**
     * Retrieve a list of song information fields, in order, in readable (GUI) format.
     *
     * \param target target array to recieve song fields
     */
    void  GetSongFields(StringArray& target) const;
    /**
     * Retrieve a list of song information fields, in order, in readable format.
     *
     * \param target target array to recieve song fields
     */
    void  GetSongFieldsDB(StringArray& target) const;
    /**
     * \param field_id MUSIK_LIBRARY_TYPE_xxx
     * \return the string representation, in GUI format, of the specified field
     */
    String GetSongFieldStr(const size_t field_id) const;
    /**
     * \param field_id MUSIK_LIBRARY_TYPE_xxx
     * \return the string representation, in database format, of the specified field
     */
    String GetSongFieldDBStr(const size_t field_id) const;
    /**
     * \return the total number of song information fields the database manages
     * in the song information table.
     */
    int    GetSongFieldCnt() const { return MUSIK_LIBRARY_FIELD_COUNT; }
    /**
     * Reverse of GetSongFieldStr(), finds the database ID based on a GUI string
     * that is found using GetSongFields() or GetSongFieldStr()
     *
     * \param field string value of the field to lookup
     * \return MUSIK_LIBRARY_TYPE_xxx
     */
    int    GetSongFieldID(String field) const;
    /**
     * Reverse of GetSongFieldDBStr(), finds the database ID based on a database string
     * that is found using GetSongFieldsDB() or GetSongFieldDBStr()
     *
     * \param field string value of the field to lookup
     * \return MUSIK_LIBRARY_TYPE_xxx
     */
    int    GetSongFieldDBID(String field) const;

    /**
     * Get all the songs in the database.
     *
     * \param target playlist to receive songs
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetAllSongs(Playlist & target);
    /**
    * Get all the songs in the temporary table.
    *
    * \param target playlist to receive songs
    * \return MUSIK_LIBRARY_OK if successful, error code otherwise
    */
    int GetAllTempSongs(Playlist& target);
    int GetAllSongs(Playlist & target, PlaylistInfo& info);

    int GetRelatedSongs(String partial_query, int order_by, PlaylistInfo& info, Playlist& target, bool clear_target = true);
    int RemoveFromStdPlaylist(const SongArray& items, int playlistid);
    int GetAllFns(String& target);


    /**
     * A general purpose query tool that can be used to find related songs
     * This function is nearly identical to GetRelatedItems(), but returns values to
     * a Playlist instead of a StringArray.
     * 
     * If the "sub_query" argument is false, the database is queryed in the following fashon:
     *
     *   SELECT DISTINCT [field], UPPER([field]) AS UP FROM songs [partial_query] ORDER BY [auto];
     * 
     *   The user must supply the partial_query field. An example would be:
     *   "WHERE(artist like 'Jethro Tull' or artist like 'The Beatles')"
     *
     * If "sub_query" is true, the the library is queried in the following fashon:
     *
     * SELECT DISTINCT [field], UPPER([field]) AS UP FROM [partial_query] ORDER BY [auto]
     *
     *   The user must again supply the partial_query field. An example would be:
     *   "(SELECT %T FROM songs WHERE artist like 'Jethro Tull' or artist like 'The Beatles') WHERE (album like 'Revolver')"
     *
     *   %%T represents a sub query.
     *
     * If the function returns successfully, target should be populated with the related material.
     *
     * \param partial_query the required portion of the query
     * \param order_by field to order by
     * \param target array to recieve the related items
     * \param sub_query if true, partial_query is assumed to have sub queries. 
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetRelatedSongs(String partial_query, int order_by, Playlist& target, bool sub_query = false, bool clear_target = true);
    /**
     * Get all the distinct information for the specified playlist field. 
     *
     * Example:
     *
     * GetAllDistinct(MUSIK_LIBRARY_TYPE_ARTIST, target); 
     *
     * Will recieve all distinct artists into the target array.
     *
     * \param source_type field to query. MUSIK_LIBRARY_TYPE_xxx
     * \param target array to recieve the values
     * \param clear_target clear the target before querying
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */

    /**
     *
     */
    int GetNextArtist(String& artist_name, Playlist& target);
    /**
     *
     */
    int GetPrevArtist(String& artist_name, Playlist& target);
    /**
     *
     */
    int GetNextAlbum(String& artist_name, String& album_name, Playlist& target);
    /**
     *
     */
    int GetPrevAlbum(String& artist_name, String& album_name, Playlist& target);

    int GetAllDistinct(int source_type, StringArray& target, bool clear_target = true);
    /**
     * \return the amount of songs in the library
     */
    int    GetSongCount();
    /**
     * \return the number of distinct entires of a specified field
     */
    int GetDistinctCount(int field);

    /**
     * Get the specified field from the specified song ID
     *
     * \param id song ID to query
     * \param field MUSIK_LIBRARY_TYPE_xxx
     * \param string string to recieve the field
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetFieldFromID(int id, int field, String& string);
    /**
     * Get all song information for the song with the specified ID
     *
     * \param id song ID to query
     * \param info SongInfo to recieve the information
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetSongInfoFromID(int id, SongInfo& info);
    /**
     * Get song attributes, that is filename and duration, from the specified song id
     *
     * \param id song ID to query
     * \param song Song to recieve the attributes
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetSongAttrFromID(int id, Song& song);
    /**
     * Get song attributes, that is filename and duration, from the specified a filename
     *
     * \param fn filename to look up
     * \param song Song to recieve the attributes
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetSongAttrFromFilename(const String& fn, Song& song);
    /**
     * \param fn filename to check for
     * \return true if the file is in the library, false otherwise
     */
    bool IsSongInLibrary(const String& fn);
    /**
     * \param songid id of the song to check for
     * \return true if the file is in the library, false otherwise
     */
    bool IsSongInLibrary(int songid);
    /**
     * Retrieve the song format from it's ID.
     *
     * \param id id of the song to query
     * \param target format to recive format. Types include MUSIK_LIBRARY_FORMAT_MP3 and
     * MUSIK_LIBRARY_FORMAT_OGG.
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetSongFormatFromID(int id, String& target);
    /**
     * Retrieves all the song information for the songs in the specified playlist
     *
     * This function can also optionally replace all values in a specific field
     * with a specified string.
     *
     * \param playlist playlist containing songs to be queried
     * \param info SongInfoArray to recieve information
     * \param replace_field_type field type to replace information with. MUSIK_LIBRARY_TYPE_xxx
     * \param new_field value of the field to be replaced
     * \param clear clear the target array before filling
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    static void GetInfoArrayFromPlaylist(Playlist& playlist, SongInfoArray& info, int replace_field_type = MUSIK_LIBRARY_TYPE_NONE, String new_field = "", bool clear = true);

    static void GetInfoArrayFromPlaylist(Playlist& playlist, SongInfoArray& info, int from, int to);

    /**
     * Get all songs that have unsynchronized tags (songs where fields in the library
     * may no longer relate to the fields in the actual file).
     *
     * \param target Playlist to recieve unsynchronized files
     * \param clear if true the target will be cleared before populating
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetDirtySongs(Playlist& target);
    /**
     * Finalizes all songs that have unsynchronized tags for database only. That is,
     * they will not be written to file.
     *
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int FinalizeDirtySongs();
    /**
     * Run a quick, wildcard query on the the following fields:
     *
     * genre, title, artist, album, filename
     *
     * And return the data to the target array.
     *
     * \param str string to query for
     * \param target Playlist to recieve queried valeus
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int QuickQuery(String str, Playlist& target);

    /**
     * Update song information
     *
     * \param info information that library will be updated with
     * \param songid if -1, info.GetID() will be used 
     * \return true if successful, false otherwise
     */
    bool SetSongInfo(SongInfo& info, int songid = -1);
    /**
     * Update the song's "Last Playing" field to right now.
     *
     * \param ID of the song to update
     * \return true if successful, false otherwise
     */
    bool SetLastPlayedToNow(int songid);
    /**
     * Update the specified song's rating
     *
     * \param songid ID of the song to update
     * \param rating new rating
     * \return true if successful, false otherwise
     */
    bool SetSongRating(int songid, int rating);
    /**
     * Update the specified song's default equalizer. Each song in the library
     * has the potential to have a different equalizer -- this is should be taken
     * care of by the front end
     *
     * \param songid id of the song to update
     * \param eq_id new equalizer id
     * \return true if successful, false otherwise
     */
    bool SetSongEqualizer(int songid, int eq_id);

    /**
     * Create a new standard (non-dynamic) playlist
     *
     * \param name name of the playlist
     * \param files list of files that the playlist will contain
     * \param groupid
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int CreateStdPlaylist(const String& name, const StringArray& files, int order = 0);
    /**
     * Create a new standard (non-dynamic) playlist
     *
     * \param name name of the playlist
     * \param songs Playlist containing songs to add to the managed playlist
     * \param type type of playlist (currently unused)
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int CreateStdPlaylist(const String& name, Playlist& songs, int order = 0);
    /**
     * Append an already existing standard (non-dynamic) playlist with the specified files.
     *
     * \param id ID of the playlist to append
     * \param files an array of filenames to be added
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int AppendStdPlaylist(int id, const StringArray& files);
    /**
     * Append an already existing standard (non-dynamic) playlist with the specified songs.
     *
     * \param id ID of the playlist to append
     * \param songs array of songs to add
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int AppendStdPlaylist(int id, const Playlist& songs);
    /**
     * Rename an existing standard (non-dynamic) playlist with the specified name.
     *
     * \param id ID of the playlist to rename
     * \param new_name the new name of the playlist
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int RenameStdPlaylist(int id, const String& new_name);
    /**
     * Deletes a playlist based on a playlist ID.
     * \param id ID of the playlist to remove
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int DeleteStdPlaylist(int id);
    /**
     * Rewrite the specified playlist. playlist.GetID() is used to determine
     * which playlist is to be written.
     *
     * \param playlist playlist to update. Playlist::m_ID must be set.
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int RewriteStdPlaylist(Playlist& playlist);
    /**
     * Retrieve information about all the managed standard (non-dynamic) playlists
     * into a target array.
     *
     * \param target array to recieve the information
     * \param groupid group id of playlist
     * \param clear_target if true then the array will be cleared before populating
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetAllStdPlaylists(PlaylistInfoArray& target, bool clear_target = true);
    /**
     * Get the specified managed playlist into a target Playlist object.
     *
     * \param id ID of the playlist to query
     * \param target Playlist to recieve the songs
     * \param clear_target if true the target will be cleared before populating
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetStdPlaylist(int id, Playlist& target, bool clear_target = true);

    /**
     * Get all the filenames for the specified managed playlist.
     *
     * \param id ID of the playlist to query
     * \param target array of strings to recieve the filenames
     * \param clear_target clear the target array before populating
     * \return true if successful, false otherwise
     */
    bool GetStdPlaylistFns(int id, StringArray& target, bool clear_target = true);
    /**
     * Get all the filenames for the specified NON-MANAGED playlist.
     *
     * \param playlist playlist to lookupx values from
     * \param target array to recieve the filenames
     * \param clear_target clear the target array before populating
     * \return true if successful, false otherwise
     */
    bool GetStdPlaylistFns(Playlist& playlist, StringArray& target, bool clear_target = true);
    /**
     * Get all the filenames for the specified MANAGED playlist.
     *
     * \param playlist playlist to lookupx values from
     * \param target string to recieve filenames. each filename will be
     *  separated by a null terminator, and the list will be ended
     *  by a double null terminator. this function is really only
     *  useful in windows when using SHFileOperation's.
     * \param clear_target clear the target array before populating
     * \return true if successful, false otherwise
     */
    bool GetStdPlaylistFns(int id, String& target);
    /**
     * Get all the filenames for the specified NON-MANAGED playlist.
     *
     * \param source source playlist
     * \param target string to recieve filenames. each filename will be
     *  separated by a null terminator, and the list will be ended
     *  by a double null terminator. this function is really only
     *  useful in windows when using SHFileOperation's.
     * \return true if successful
     */    
    bool GetStdPlaylistFns(Playlist& source, String& target);

    /**
     * Get all the filenames for the specified managed dynamic playlist.
     *
     * \param id ID of the dynamic playlist to query
     * \param target string to recieve filenames. each filename will be
     *  separated by a null terminator, and the list will be ended
     *  by a double null terminator. this function is really only
     *  useful in windows when using SHFileOperation's.
     * \param clear_target clear the target array before populating
     * \return true if successful, false otherwise
     */
    bool GetDynPlaylistFns(int id, String& target);
    /**
     * Get all the filenames for the specified managed dynamic playlist.
     *
     * \param id ID of the dynamic playlist to query
     * \param target array of strings to recieve the filenames
     * \param clear_target clear the target array before populating
     * \return true if successful, false otherwise
     */
    bool GetDynPlaylistFns(int id, StringArray& target, bool clear_target = true);

    /**
     * Create a new managed dynamic playlist
     *
     * \param name name assigned to the playlist
     * \param query the dynamic playlist's query
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */    
    int CreateDynPlaylist(const String& name, const String& query, int order = 0);
    /**
     * Remove a dynamic playlist from the library.
     *
     * \param id the id of the dynamic playlist to delete.
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */    
    int DeleteDynPlaylist(int id);
    /**
     * Retrieve a list of all the dynamic playlists in the database.
     *
     * \param target the array to recieve the playlists
     * \param clear_target clear target array before retrieving
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */    
    int GetAllDynPlaylists(PlaylistInfoArray& target, bool clear_target = true);
    /**
     * Get all songs related to the specified dynamic playlist
     *
     * \param id the id of the playlist to use as a query
     * \param target target buffer to recieve the list of songs    
     * \param clear_target clear target array before retrieving
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */    
    int QueryDynPlaylist(int id, Playlist& target, bool clear_target = true);
    /**
     * Get dynamic playlist name and query associated with the specified id
     *
     * \param id the id of the playlist to query
     * \param name target buffer to recieve the playlist's name
     * \param query target buffer to recieve the playlist's query
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */    
    int GetDynPlaylistInfo(int id, String& name, String& query);
    /**
     * Update an already existing dynamic playlist's query
     *
     * \param id the id of the playlist to query
     * \param query the updated query string
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */    
    int RewriteDynPlaylistQuery(int id, const String& query);
    /**
     * Rename an already existing dynamic playlist
     *
     * \param id the id of the playlist to query
     * \param name the updated name
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */    
    int RenameDynPlaylist(int id, const String& name);

    /**
     * Stores a new crossfader in the table.
     *
     * \param fader fader to create. will use fader.m_Name for the name
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int CreateCrossfader(Crossfader& fader);
    /** 
     * Removes a crossfader from the table
     *
     * \param id crossfader ID to remove. 
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int DeleteCrossfader(int id);
    /**
     * Gets the settings for the specified crossfader ID 
     *
     * \param id crossfader id to query
     * \param fader target settings class to recieve the values
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetCrossfader(int id, Crossfader& fader);
    /**
     * Get a list of all crossfaders and their respective ID's
     *
     * \param target_names array to recieve the names of the crossfaders
     * \param target_ids array to recieve the ids of the crossfaders
     * \param clear_targets if true the target arrays will be cleared before populating
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetAllCrossfaderPresets(StringArray& target_names, IntArray& target_ids, bool clear_targets = true);
    /**
     * Retrieve a crossfader ID by name. Multiple crossfaders with the 
     * same name are not allowed.
     *
     * \param name string name to query
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetCrossfaderIDFromName(const String& name);
    /**
     * The default crossfader is contained within its own table with only
     * its values. 
     *
     * \param fader target fader to recieve settings
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetDefaultCrossfader(Crossfader& fader);
    /**
     * Update the default crosstable settings based on the specified Crossfader
     *
     * \param fader Crossfader containing settings that are to be written
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int UpdateDefaultCrossfader(const Crossfader& fader);
    /**
     * Reset the default crossfader to stock settings
     *
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int ResetDefaultCrossfader();

    /**
     * Create a new equalizer based on specified settings
     *
     * \param src settings to be written to file
     * \param is_preset if true the equalizer is marked as a preset, that is,
     * a group of settings set manually by the user, not automatically by
     * the Player
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int CreateEqualizer(EQSettings& src, bool is_preset = false);
    /**
     * Delete the specified equalizer
     *
     * \param id equalizer to delete
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int DeleteEqualizer(int id);
    /**
     * Get the equalizer settings for the respective equalizer ID
     *
     * \param eq_id equalizer id to query
     * \param target target to recieve settings
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetEqualizer(int eq_id, EQSettings& target);
    /**
     * Update the specified equalizer settings with new values
     *
     * \param id equalizer to query
     * \param source settings that will be written to disk
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int UpdateEqualizer(int id, const EQSettings& src);
    /**
     * Each song can have a seperate equalizer, whose ID is stored in
     * the song information table. 
     *
     * \param id song id to query
     * \return the song's equalizer id
     */
    int GetEqualizerIDFromSongID(int id);
    /**
     * Attempt to look up an equalizer ID by name
     *
     * \param name string name of the equalizer to look up
     * \return the id of the equalizer, -1 if default.
     */
    int GetEqualizerIDFromName(const String& name);
    /**
     * Gets a list of all equalizer preset names and ids
     *
     * \param target_names array of strings to recieve names
     * \param target_ids array of ints to recieve ids
     * \param clear_targets if true the targets will be cleared before populating
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetAllEqualizerPresets(StringArray& target_names, IntArray& target_ids, bool clear_targets = true);
    /**
     * Get the default equalizer settings. These are contained within
     * their own table, separate from the equalizer preset table.
     *
     * \param target EQSettings to recieve settings
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetDefaultEqualizer(EQSettings& target);
    /**
     * Store new default equalizer settings
     *
     * \param src settings that will be stored
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int UpdateDefaultEqualizer(const EQSettings& src);
    /**
     * Resets the default equalizer to stock settings. 
     *
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int ResetDefaultEqualizer();

    /**
     * Add the specified mask to the managed filename masks table
     *
     * \param mask mask to add
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int AddMask(const String& mask);
    /**
     * Remove the specified mask from the managed filename masks table
     *
     * \param mask mask to remove
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int RemoveMask(const String& mask);
    /**
     * Get all the managed filename masks
     *
     * \param target array to receive masks.
     * \param clear_target clear the target array before adding the items
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetAllMasks(StringArray& target, bool clear_target = true);

    /**
     * Add the specified lower-case-only-string 
     *
     * \param word word to add to the list of lower-case-only-strings
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int AddChgCase(const String& word);
    /**
     * Remove the specified lower-case-only word from the database
     *
     * \param word word to remove
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int RemoveChgCase(const String& word);
    /**
     * Get all the lower-case-only words
     *
     * \param target array to receive words.
     * \param clear_target clear the target array before adding the items
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetAllChgCase(StringArray& target, bool clear_target = true);

    /**
     * Add the specified path to the managed synchronized directories
     *
     * \param path path to add
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int    AddPath(const String& path);
    /**
     * Remove the specified path from the managed synchronized directories
     *
     * \param path path to remove
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int RemovePath(const String& path);
    /**
     * Get all the managed synchronized directories.
     *
     * \param target array to receive paths.
     * \param clear_target clear the target array before adding the items
     * \return MUSIK_LIBRARY_OK if successful, error code otherwise
     */
    int GetAllPaths(StringArray& target, bool clear_target = true);

    /**
     * Adds the specified song to the library. AddOGG() or AddMP3() or AddNet() will
     * be called based on the filename.
     *
     * \param fn filename of the file to add
     * \return true if added successfully, false otherwise
     */
    bool AddSong(const String& fn);
    /**
     * Add a NetStream to the library.
     *
     * \param fn filename of the file to add
     * \return true if added successfully, false otherwise
     */
    bool AddNet(const String& fn);
    /**
     * Remove a song from the library based on it's ID
     *
     * \param songid songid of song to remove
     * \return true if removed successfully, false otherwise
     */
    bool RemoveSong(int songid);
    /**
     * Remove a song from the library based on it's filename
     *
     * \param fn filename of song to remove
     * \return true if removed successfully, false otherwise
     */
    bool RemoveSong(const String& fn);
    /**
     * Enqueue a task related to the library, such as musikBatchChgCase,
     * musikTagToFn, musikTagFromFn, etc. All tasks in the queue will
     * be managed internally.
     *
     * \param ptrTask pointer to the task to be added to the queue
     */
    void QueueTask(Task* ptrTask);
    /**
     * Remove (and delete) all the queued tasks.
     */
    void FlushTaskQueue();
    /**
     * \return number of tasks in the queue.
     */
    size_t GetTaskCount(){ return m_Tasks.size(); }
    /**
     * \return the current task type
     */
    int GetTaskType();
 
    /**
     * Tell the library to use UTC time or not.
     */
    static void SetUseUTC(bool use);

    /**
     * Get a selection of random entries from a specified field.
     *
     * \param field database field type to query
     * \param count number of random results to return
     * \return a formatted string "artist, artist, artist, etc"
     */
    const String GetRandom(int field, size_t count = 1);
    /**
     * Get a playlist of randomly selected entries from a specified field
     *
     * \param field field type to query
     * \param target Playlist to recieve data
     * \param clear_target clear the specified Playlist before querying?
     * \param count number of random results to query
     */
    void GetRandom(int field, Playlist& target, bool clear_target = true, size_t count = 1);
  
    /**
     * Generate a bounded random number
     *
     * \param min lower bound
     * \param max upper bound
     * \return random number
     */
    static int Rand(int min, int max);

	/**
	 * Get the last played song
	 *
	 * \param index current song
	 * \return previous index
	 */
	static int GetLastSong( int songid, Library *lib );

    int QuerySongs(const String & query, Playlist & target);
    int QuerySongs(const String & query, Playlist & target, int orderBy);
    int RawQuerySongs(String query, Playlist& target);
    int RawQueryStringArray(const String& query, StringArray& target);
    int UpdatePlaylistOrder(const PlaylistInfo& target);

    bool Startup();
    void Shutdown();
    void InitDefaultDynPlaylists();
    void ReloadTags(Song& song);

    static String GetOrder(int type, bool terminate = true, bool by_upper = false, bool descend = false);

    bool TransactionOpen(){ return m_Transactions == 0 ? false : true; }

protected:

    /**
    * Add an file to the library.
    *
    * \param fn filename of the file to add
    * \return true if added successfully, false otherwise
    */
    bool AddFile(const SongInfo& source);

    void Claim(Playlist& playlist);
    void Claim(SongInfoArray& songs);
    void GetPlaylistAttr(Playlist& playlist);

    // variables used by the library
    String m_Filename;
    sqlite3 *m_pDB;
    bool m_DatabaseOpen;
    int m_Transactions;
    Mutex m_ProtectingLibrary;
    StringArray m_Fields;
    StringArray m_FieldsDB;
    TaskQueue m_Tasks;

    // internal song functions
    void VerifyYearList(StringArray & list);
    int UpdatePlayTime(SongInfo& song);
    int UpdateTimesPlayed(SongInfo& song);
	int UpdateLastPlayed(SongInfo& song);

    void InitFields();

    bool InitVersionTable();
    bool InitLibTable();
    bool InitTempLibTable();
    bool InitStdTables();
    bool InitDynTable(bool create_stock_playlists = true);
    bool InitEqTable();
    bool InitPathTable();
    bool InitMaskTable();
    bool InitCrossfaderTable();
    bool InitChgCaseTable();
    bool InitHistoryTable();

    // functions used to patch previous versions. at some point these may be
    // declared as deprecated and be removed to make room for new patches.
    void PatchLibrary();
    int Patch4GetStdPlaylist(int id, Playlist& target, bool clear_target = true);
    int Patch5GetAllStdPlaylists(PlaylistInfoArray& target, bool clear_target = true);    
    int Patch5GetAllDynPlaylists(PlaylistInfoArray& target, bool clear_target = true);
};

///////////////////////////////////////////////////

#if defined WIN32
#define MUSIK_REMLIBRARY_PATH _T("removablepath")
#else
#define MUSIK_REMLIBRARY_PATH (const wchar_t*) "removablepath"
#endif

class MUSIKEXPORT RemLibrary : public Library
{
public:

    RemLibrary(const String& filename);
    void SetRemPath(const String& path, bool update_all_songs = true);
    String GetRemPath();

protected:

    bool InitRemPathTable();
};

///////////////////////////////////////////////////

} // namespace musikCore

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
