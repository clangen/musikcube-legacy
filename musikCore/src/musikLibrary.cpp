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
#include "../include/musikLibrary.h"
#include "../include/musikFilename.h"
#include "../include/musikBatchChgCase.h"
#include "../include/musikLibraryCallbacks.h"
#if defined (USE_BASS)
#include "../include/musikPlayerBASS.h"    // we use this to query plugins for additional formats
#endif
#include "stdafx.h"
#include "time.h"
#include <stack>
#include <sys/stat.h>

extern "C" {
#include "sqliteInt.h"
}

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

bool g_UTC = true;

///////////////////////////////////////////////////

Library::Library(const String& filename)
{
	srand(time(0));

    m_pDB = NULL;
    m_Transactions = 0;
    m_DatabaseOpen = false;

    m_Filename = filename;

    InitFields();

    if (!filename.IsEmpty())
        Startup();
}

///////////////////////////////////////////////////

Library::~Library()
{
	Shutdown();
}

///////////////////////////////////////////////////

void Library::SetUseUTC(bool use)
{
	::g_UTC = use;
}

///////////////////////////////////////////////////

void Library::SetFilename(const String& filename, bool reinit)
{
	m_Filename = filename;
    if (reinit)
    {
        Shutdown();
        Startup();
    }
}

///////////////////////////////////////////////////

void Library::InitFields()
{
	m_Fields.push_back("Artist");
    m_Fields.push_back("Album");
    m_Fields.push_back("Year");
    m_Fields.push_back("Genre");
    m_Fields.push_back("Title");
    m_Fields.push_back("Track");
    m_Fields.push_back("Time Added");
    m_Fields.push_back("Last Played");
    m_Fields.push_back("Filesize");
    m_Fields.push_back("Format");
    m_Fields.push_back("Time");
    m_Fields.push_back("Rating");
    m_Fields.push_back("Times Played");
    m_Fields.push_back("Bitrate");
    m_Fields.push_back("Filename");
    m_Fields.push_back("Equalizer");
    m_Fields.push_back("Comments");

    m_FieldsDB.push_back("artist");
    m_FieldsDB.push_back("album");
    m_FieldsDB.push_back("year");
    m_FieldsDB.push_back("genre");
    m_FieldsDB.push_back("title");
    m_FieldsDB.push_back("tracknum");
    m_FieldsDB.push_back("timeadded");
    m_FieldsDB.push_back("lastplayed");
    m_FieldsDB.push_back("filesize");
    m_FieldsDB.push_back("format");
    m_FieldsDB.push_back("duration");
    m_FieldsDB.push_back("rating");
    m_FieldsDB.push_back("timesplayed");
    m_FieldsDB.push_back("bitrate");
    m_FieldsDB.push_back("filename");
    m_FieldsDB.push_back("equalizer");
    m_FieldsDB.push_back("notes");        // represented by "Comments" for the ui
}

///////////////////////////////////////////////////

int Library::GetSongFieldID(String field) const
{
	for (size_t i = 0; i < m_Fields.size(); i++)
    {
        if (field == m_Fields.at(i))
            return i;
    }
    return -1;
}

///////////////////////////////////////////////////

void Library::GetSongFields(StringArray& target) const
{
	target.clear();

    for (size_t i = 0; i < m_Fields.size(); i ++)
        target.push_back(m_Fields.at(i));
}

///////////////////////////////////////////////////

void Library::GetSongFieldsDB(StringArray& target) const
{
	target.clear();

    for (size_t i = 0; i < m_FieldsDB.size(); i ++)
        target.push_back(m_FieldsDB.at(i));
}

///////////////////////////////////////////////////

String Library::GetSongFieldStr(const size_t field_id) const
{
	if (field_id >= m_Fields.size())
        return "";

    return m_Fields.at(field_id);
}

///////////////////////////////////////////////////

String Library::GetSongFieldDBStr(const size_t field_id) const
{
	if (field_id >= m_FieldsDB.size())
        return "";

    return m_FieldsDB.at(field_id);
}

///////////////////////////////////////////////////

int Library::GetSongFieldDBID(String field) const
{
	for (size_t i = 0; i < m_FieldsDB.size(); i++)
    {
        if (field == m_FieldsDB.at(i))
            return i;
    }
    return -1;
}

///////////////////////////////////////////////////

bool Library::InitStdTables()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table that contains a list of
    // all the standard playlist names
    String szCreateDBQuery1;
    szCreateDBQuery1.Format( 
        _T("CREATE TABLE %ls (")
        _T("std_playlist_id INTEGER PRIMARY KEY, ")
        _T("std_playlist_name varchar(255), ")
        _T("std_playlist_order INTEGER")
        _T(");"),
        STD_PLAYLIST_TABLE_NAME);

    // construct the table that will store all the
    // songs that pertain to all the playlists
    String szCreateDBQuery2;
    szCreateDBQuery2.Format( 
        _T("CREATE TABLE %ls (")    
        _T("std_playlist_songid INTEGER PRIMARY KEY, ")
        _T("std_playlist_id INTEGER, ")
        _T("songid INTEGER ")
        _T(");"),
        STD_PLAYLIST_SONGS);

    // put a lock on the library and open it up
    char *pErr1 = NULL;
    char *pErr2 = NULL;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery1).c_str(), NULL, NULL, &pErr1);
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery2).c_str(), NULL, NULL, &pErr2);
    m_ProtectingLibrary.unlock();

    if (pErr1)
    {
        error = true;
        sqlite3_free(pErr1);
    }

    if (pErr2)
    {
        error = true;
        sqlite3_free(pErr2);
    }

    return error;
}

///////////////////////////////////////////////////

bool Library::InitHistoryTable()
{
	return false;
}

///////////////////////////////////////////////////

bool Library::InitChgCaseTable()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table that contains a list of
    // all the standard playlist names
    String szCreateDBQuery;
    szCreateDBQuery.Format(
        _T("CREATE TABLE %ls (")
        _T("case_ignore PRIMARY_KEY varchar(255) ")
        _T(");"),
        CHANGE_CASE_TABLE);

    // put a lock on the library and open it up
    char *pErr1 = NULL;

    m_ProtectingLibrary.lock();

        sqlite3_exec(
            m_pDB, 
            utf16to8(szCreateDBQuery).c_str(), 
            NULL, NULL, &pErr1);

    m_ProtectingLibrary.unlock();

    if (pErr1)
    {
        error = true;
        sqlite3_free(pErr1);
    }
    else
    {
        StringArray cases;

        cases.push_back(_T("a"));
        cases.push_back(_T("and"));
        cases.push_back(_T("as"));
        cases.push_back(_T("at"));
        cases.push_back(_T("but"));        
        cases.push_back(_T("for"));
        cases.push_back(_T("if"));
        cases.push_back(_T("in"));
        cases.push_back(_T("is"));
        cases.push_back(_T("it"));
        cases.push_back(_T("of"));
        cases.push_back(_T("on"));
        cases.push_back(_T("the"));
        cases.push_back(_T("to"));

        BeginTransaction();
        for (size_t i = 0; i < cases.size(); i++)
        {
            m_ProtectingLibrary.lock();

            String query;
            query.Format(
                _T(" INSERT INTO %ls ")
                _T(" VALUES ('%ls');"),
                CHANGE_CASE_TABLE,
                q_str(cases.at(i)).c_str());
            
            sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(), 
                NULL, NULL, NULL); 

            m_ProtectingLibrary.unlock();
        }
        EndTransaction();        
    }

    return error;    
}

///////////////////////////////////////////////////

bool Library::InitEqTable()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table that contains a list of
    // all the standard playlist names
    String szCreateDBQuery1;
    szCreateDBQuery1.Format(
        _T("CREATE TABLE %ls (")
        _T("equalizer_id INTEGER PRIMARY KEY, ")
        _T("equalizer_name VARCHAR(255), ")
        _T("equalizer_is_preset INTEGER, ")
        _T("hz55_left INTEGER, ")
        _T("hz77_left INTEGER, ")
        _T("hz110_left INTEGER, ")
        _T("hz156_left INTEGER, ")
        _T("hz220_left INTEGER, ")
        _T("hz311_left INTEGER, ")
        _T("hz440_left INTEGER, ")
        _T("hz622_left INTEGER, ")
        _T("hz880_left INTEGER, ")
        _T("hz1244_left INTEGER, ")
        _T("hz1760_left INTEGER, ")
        _T("hz2489_left INTEGER, ")
        _T("hz3520_left INTEGER, ")
        _T("hz4978_left INTEGER, ")
        _T("hz7040_left INTEGER, ")
        _T("hz9956_left INTEGER, ")
        _T("hz14080_left INTEGER, ")
        _T("hz19912_left INTEGER, ")
        _T("hz55_right INTEGER, ")
        _T("hz77_right INTEGER, ")
        _T("hz110_right INTEGER, ")
        _T("hz156_right INTEGER, ")
        _T("hz220_right INTEGER, ")
        _T("hz311_right INTEGER, ")
        _T("hz440_right INTEGER, ")
        _T("hz622_right INTEGER, ")
        _T("hz880_right INTEGER, ")
        _T("hz1244_right INTEGER, ")
        _T("hz1760_right INTEGER, ")
        _T("hz2489_right INTEGER, ")
        _T("hz3520_right INTEGER, ")
        _T("hz4978_right INTEGER, ")
        _T("hz7040_right INTEGER, ")
        _T("hz9956_right INTEGER, ")
        _T("hz14080_right INTEGER, ")
        _T("hz19912_right INTEGER ")
        _T("); "),
        EQUALIZER_PRESET);

    // this is the default equalizer to use, there
    // should never be more than A SINGLE ROW in this
    // table...
    String szCreateDBQuery2;
    szCreateDBQuery2.Format(
        _T("CREATE TABLE %ls (")
        _T("equalizer_id INTEGER PRIMARY KEY, ")
        _T("equalizer_name VARCHAR(255), ")
        _T("equalizer_is_preset INTEGER, ")
        _T("hz55_left INTEGER, ")
        _T("hz77_left INTEGER, ")
        _T("hz110_left INTEGER, ")
        _T("hz156_left INTEGER, ")
        _T("hz220_left INTEGER, ")
        _T("hz311_left INTEGER, ")
        _T("hz440_left INTEGER, ")
        _T("hz622_left INTEGER, ")
        _T("hz880_left INTEGER, ")
        _T("hz1244_left INTEGER, ")
        _T("hz1760_left INTEGER, ")
        _T("hz2489_left INTEGER, ")
        _T("hz3520_left INTEGER, ")
        _T("hz4978_left INTEGER, ")
        _T("hz7040_left INTEGER, ")
        _T("hz9956_left INTEGER, ")
        _T("hz14080_left INTEGER, ")
        _T("hz19912_left INTEGER, ")
        _T("hz55_right INTEGER, ")
        _T("hz77_right INTEGER, ")
        _T("hz110_right INTEGER, ")
        _T("hz156_right INTEGER, ")
        _T("hz220_right INTEGER, ")
        _T("hz311_right INTEGER, ")
        _T("hz440_right INTEGER, ")
        _T("hz622_right INTEGER, ")
        _T("hz880_right INTEGER, ")
        _T("hz1244_right INTEGER, ")
        _T("hz1760_right INTEGER, ")
        _T("hz2489_right INTEGER, ")
        _T("hz3520_right INTEGER, ")
        _T("hz4978_right INTEGER, ")
        _T("hz7040_right INTEGER, ")
        _T("hz9956_right INTEGER, ")
        _T("hz14080_right INTEGER, ")
        _T("hz19912_right INTEGER ")
        _T("); "),
        EQUALIZER_DEFAULT);

    // put a lock on the library and open it up
    char *pErr1 = NULL;
    char *pErr2 = NULL;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery1).c_str(), NULL, NULL, &pErr1);
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery2).c_str(), NULL, NULL, &pErr2);
    m_ProtectingLibrary.unlock();

    if (!pErr2)
        ResetDefaultEqualizer();

    if (pErr1)
    {
        error = true;
        sqlite3_free(pErr1);
    }

    if (pErr2)
    {
        error = true;
        sqlite3_free(pErr2);
    }

    return error;
}

///////////////////////////////////////////////////

bool Library::InitPathTable()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table that contains a list of
    // all the standard playlist names
    String szCreateDBQuery;
    szCreateDBQuery.Format(
        _T("CREATE TABLE %ls (")
        _T("path_name varchar(1024) PRIMARY KEY);"),
        MANAGED_PATH);

    // put a lock on the library and open it up
    char *pErr = NULL;

    m_ProtectingLibrary.lock();
    
        sqlite3_exec(
            m_pDB, 
            utf16to8(szCreateDBQuery).c_str(), 
            NULL, NULL, &pErr);
    
    m_ProtectingLibrary.unlock();

    if (pErr)
    {
        error = true;
        sqlite3_free(pErr);
    }

    return error;
}

///////////////////////////////////////////////////

bool Library::InitMaskTable()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table that contains a list of
    // all the managed filename masks
    String szCreateDBQuery;
    szCreateDBQuery.Format(
        _T("CREATE TABLE %ls ("   )
        _T("filename_mask varchar(1024) PRIMARY KEY);"),
        FILENAME_MASK);

    // put a lock on the library and open it up
    char *pErr = NULL;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery).c_str(), NULL, NULL, &pErr);
    m_ProtectingLibrary.unlock();

    if (pErr)
    {
        error = true;
        sqlite3_free(pErr);
    }

    return error;
}

///////////////////////////////////////////////////

bool Library::InitDynTable(bool create_stock_playlists)
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table that contains a list of
    // all the standard playlist names
    String szCreateDBQuery1;
    szCreateDBQuery1.Format( 
        _T("CREATE TABLE %ls (")    
        _T("dyn_playlist_id INTEGER PRIMARY KEY, ")
        _T("dyn_playlist_name varchar(255), ")
        _T("dyn_playlist_query varchar(1024), ")
        _T("dyn_playlist_order INTEGER")
        _T(");"),
        DYN_PLAYLIST_TABLE_NAME);

    // put a lock on the library and open it up
    char *pErr1 = NULL;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery1).c_str(), NULL, NULL, &pErr1);
    m_ProtectingLibrary.unlock();

    if (pErr1)
    {
        error = true;
        sqlite3_free(pErr1);
    }
    else
    {
        if (create_stock_playlists)
            InitDefaultDynPlaylists();
    }

    return error;
}

///////////////////////////////////////////////////

bool Library::InitCrossfaderTable()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table that contains a list of
    // all the crossfader presets
    String szCreateDBQuery1;
    szCreateDBQuery1.Format(
        _T("CREATE TABLE %ls (")
        _T("crossfader_id INTEGER PRIMARY KEY, ")
        _T("crossfader_name VARCHAR(255), ")
        _T("newsong FLOAT, ")
        _T("pause_resume FLOAT, ")
        _T("seek FLOAT, ")
        _T("stop FLOAT, ")
        _T("exit FLOAT ")
        _T(");"),
        CROSSFADER_PRESET);

    String szCreateDBQuery2;
    szCreateDBQuery2.Format( 
        _T("CREATE TABLE %ls (")
        _T("crossfader_id INTEGER PRIMARY KEY, ")
        _T("newsong FLOAT, ")
        _T("pause_resume FLOAT, ")
        _T("seek FLOAT, ")
        _T("stop FLOAT, ")
        _T("exit FLOAT ")
        _T(");"),
        CROSSFADER_DEFAULT);

    // put a lock on the library and open it up
    char *pErr1 = NULL;
    char *pErr2 = NULL;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery1).c_str(), NULL, NULL, &pErr1);
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery2).c_str(), NULL, NULL, &pErr2);
    m_ProtectingLibrary.unlock();

    if (!pErr1 && !pErr2)
        ResetDefaultCrossfader();

    if (pErr1)
    {
        error = true;
        sqlite3_free(pErr1);
    }

    if (pErr2)
    {
        error = true;
        sqlite3_free(pErr2);
    }


    return error;
}

///////////////////////////////////////////////////

bool Library::InitVersionTable()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table
    String szCreateDBQuery;
    szCreateDBQuery.Format(
        _T("CREATE TABLE %ls (") 
        _T("version INTEGER PRIMARY KEY")
        _T(");"),
        VERSION_TABLE_NAME);

    // put a lock on the library and open it up
    char *pErr1 = NULL;

    m_ProtectingLibrary.lock();

    sqlite3_exec(
        m_pDB, 
        utf16to8(szCreateDBQuery).c_str(), 
        NULL, NULL, &pErr1);
    
    m_ProtectingLibrary.unlock();

    if (pErr1)
    {
        error = true;
        sqlite3_free(pErr1);
    }
    else
    {
        // table was created, so insert default value
        m_ProtectingLibrary.lock();

            String query;
            query.Format(
                _T(" INSERT INTO %ls ")
                _T(" VALUES (%d) "), 
                VERSION_TABLE_NAME,
                MUSIK_LIBRARY_VERSION);

            sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(), 
                NULL, NULL, NULL);

        m_ProtectingLibrary.unlock();
    }

    PatchLibrary();

    return error;
}

///////////////////////////////////////////////////

void Library::InitDefaultDynPlaylists()
{
	String query;

    query.Format(
        _T(" lastplayed < julianday('now') AND ")
        _T(" lastplayed > 0 ")
        _T(" ORDER BY julianday(lastplayed) DESC")
        _T(" LIMIT 50 "));
    CreateDynPlaylist(_T("50 Last Played"), query, 0);

    query.Format(
        _T(" julianday(timeadded) > 0 ")
        _T(" ORDER BY julianday(timeadded) DESC ")
        _T(" LIMIT 50;"));
    CreateDynPlaylist(_T("50 Newest"), query, 1);

    query.Format(
        _T(" timesplayed > 0 ")
        _T(" ORDER BY timesplayed DESC ")
        _T(" LIMIT 10; "));
    CreateDynPlaylist(_T("10 Most Played"), query, 2);

    query.Format(
        _T(" LOWER(artist) = (")
        _T("   SELECT DISTINCT LOWER(artist) ")
        _T("   FROM (")
        _T("     SELECT DISTINCT artist,sum(timesplayed) AS total ")
        _T("     FROM songs ")
        _T("     GROUP BY LOWER(artist) ") 
        _T("     ORDER BY total DESC ")
        _T("     LIMIT 10 ")
        _T("  ) ")
        _T("   WHERE artist <> ''")
        _T(") "));
    CreateDynPlaylist(_T("Favorite Artist"), query, 3);

    query.Format(
        _T(" LOWER(album) = (")
        _T("   SELECT DISTINCT LOWER(album) ")
        _T("   FROM (")
        _T("     SELECT DISTINCT album,sum(timesplayed) AS total ") 
        _T("     FROM songs ")
        _T("     GROUP BY LOWER(album) ")
        _T("     ORDER BY total DESC ") 
        _T("     LIMIT 10 ")
        _T("  ) ")
        _T("   WHERE album <> ''")
        _T(") "));
    CreateDynPlaylist(_T("Favorite Album"), query, 4);
}

///////////////////////////////////////////////////

bool Library::InitLibTable()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table
    String szCreateDBQuery;
    szCreateDBQuery.Format(
        _T("CREATE TABLE %ls (")
        _T("   songid INTEGER PRIMARY KEY, ")
        _T("   format varchar(6), ")
        _T("   vbr number(1), ")        
        _T("   filename varchar(255), ")        
        _T("   artist varchar(255), ")
        _T("   title varchar(255), ")    
        _T("   album varchar(255), ")    
        _T("   tracknum number(3), ")    
        _T("   year number(4), ")    
        _T("   genre varchar(255), ")    
        _T("   rating number(1), "   )    
        _T("   bitrate number(10), ")    
        _T("   lastplayed timestamp, "   )
        _T("   notes varchar(255), ")
        _T("   timesplayed number(5), ")    
        _T("   duration number(10), ")
        _T("   timeadded timestamp, ")
        _T("   filesize number(10), ")
        _T("   dirty number(10), ")
        _T("   equalizer number(10), ")
        _T("   totplaytime number(10), ")
        _T("   userplayed number(5), ")
        _T("   autoadvanced number(5) ")
        _T(");"),
        SONG_TABLE_NAME);

    // construct the index
    String szCreateIdxQuery;
    szCreateIdxQuery.Format(
        _T("CREATE INDEX songs_title_idx on %ls (title);")
        _T("CREATE UNIQUE INDEX songs_filename_idx on %ls (filename);")
        _T("CREATE INDEX songs_artist_idx on %ls (artist);")
        _T("CREATE INDEX songs_album_idx on %ls (album);")
        _T("CREATE INDEX songs_genre_idx on %ls (genre);")
        _T("CREATE INDEX songs_artist_album_tracknum_idx on %ls (artist,album,tracknum);"),
        SONG_TABLE_NAME, SONG_TABLE_NAME, SONG_TABLE_NAME,
        SONG_TABLE_NAME, SONG_TABLE_NAME, SONG_TABLE_NAME);

    // put a lock on the library and open it up
    char *pErr1 = NULL;
    char* pErr2 = NULL;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery).c_str(), NULL, NULL, &pErr1);
        sqlite3_exec(m_pDB, utf16to8(szCreateIdxQuery).c_str(), NULL, NULL, &pErr2);
    m_ProtectingLibrary.unlock();

    if (pErr1)
    {
        error = true;
        sqlite3_free(pErr1);
    }

    if (pErr2)
    {
        error = true;
        sqlite3_free(pErr2);
    }

    return error;
}

///////////////////////////////////////////////////

bool Library::InitTempLibTable()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table
    String szCreateDBQuery;
    szCreateDBQuery.Format( 
        _T("CREATE TABLE %ls (")
        _T("  songid INTEGER); "),
        TEMP_SONG_TABLE_NAME);

    // put a lock on the library and open it up
    char *pErr1 = NULL;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, utf16to8(szCreateDBQuery).c_str(), NULL, NULL, &pErr1);
    m_ProtectingLibrary.unlock();

    if (pErr1)
    {
        error = true;
        sqlite3_free(pErr1);
    }

    return error;
}

///////////////////////////////////////////////////

bool Library::Startup()
{
	if (this->m_DatabaseOpen)
    {
        Shutdown();
    }

    char* pErr = NULL;

    m_ProtectingLibrary.lock();
    {
        Filename mfn(m_Filename);
        String path(mfn.GetPath());
        Filename::RecurseMkDir(path.GetBuffer());

        // the filename we give sqlite needs to be multibyte
        // in win32.
    #if defined(WIN32) && !defined(__MINGW32__)
        sqlite3_open16((void*)m_Filename.c_str(), &m_pDB);
    #else
        std::string cfn = utf16to8(m_Filename);
        sqlite3_open(cfn.c_str(), &m_pDB);
    #endif

        if (m_pDB)
        {
            if ( ! pErr)    
            {
                this->m_DatabaseOpen = true;
            }
            else
            {
                sqlite3_close(this->m_pDB);
                this->m_pDB = NULL;
            }
        }
    }
    m_ProtectingLibrary.unlock();

    if (this->m_DatabaseOpen)
    {
        InitVersionTable();
        InitLibTable();
        InitTempLibTable();
        InitStdTables();
        InitDynTable();
        InitEqTable();
        InitPathTable();
        InitMaskTable();
        InitCrossfaderTable();
        InitChgCaseTable();
    }

   if (pErr)
   {
       sqlite3_free(pErr);
   }

   return this->m_DatabaseOpen;
}

///////////////////////////////////////////////////

void Library::Shutdown()
{
	// lock it up and close it down.
    m_ProtectingLibrary.lock();
        if (m_DatabaseOpen)
        {
            sqlite3_close(m_pDB);
            m_pDB = NULL;
            m_DatabaseOpen = false;
        }    
    m_ProtectingLibrary.unlock();
}

///////////////////////////////////////////////////

void Library::BeginTransaction()
{
	if (!m_DatabaseOpen)
        return;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, "begin transaction;", NULL, NULL, NULL);
    m_ProtectingLibrary.unlock();

    if (m_Transactions == 0)
        cout << "musikCore::Library: begin transaction. clean.\n";
    else
        cout << "musikCore::Library: begin transaction. dirty.\n";

    ++m_Transactions;
}

///////////////////////////////////////////////////

void Library::EndTransaction()
{
	if (!m_DatabaseOpen)
        return;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, "end transaction;", NULL, NULL, NULL);
    m_ProtectingLibrary.unlock();

    if (m_Transactions <= 0)
    {
        cout << "musikCore::Library: end transaction. out of sync";
        m_Transactions = 0;
        return;
    }
    else
    {
        cout << "musikCore::Library: end transaction\n";
        m_Transactions--;
    }
}

///////////////////////////////////////////////////

int Library::DeleteCrossfader(int id)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;
    m_ProtectingLibrary.lock();
    {
        String query;
        query.Format(
            _T(" DELETE FROM %ls ")
            _T(" WHERE crossfader_id = %d; "),
            CROSSFADER_PRESET,
            id);

        nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(), 
                NULL, NULL, NULL);
    }
    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::CreateStdPlaylist(const String& name, const StringArray& songids, int order)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nID, nRet;

    m_ProtectingLibrary.lock();
    // insert the new playlist name
    {
        String query;
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES (NULL, '%ls', %d);"),
            STD_PLAYLIST_TABLE_NAME,
            q_str(name).c_str(),
            order);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);
    }

    m_ProtectingLibrary.unlock();

    if (nRet != MUSIK_LIBRARY_OK)
        return -1;

    // get the ID of the newly created entry
    nID = (int)sqlite3_last_insert_rowid(m_pDB);

    // insert songs into playlist
    if (nID >= 0)
    {
        Song target;
        BeginTransaction();
        for (size_t i = 0; i < songids.size(); i++)
        {
            AddSong(songids.at(i));
            GetSongAttrFromFilename(songids.at(i), target);

            m_ProtectingLibrary.lock();
            {
                String query;
                query.Format(
                    _T(" INSERT INTO %ls ")
                    _T(" VALUES (NULL, %d, %d);"),
                    STD_PLAYLIST_SONGS,
                    nID,
                    target.GetID());

                nRet = sqlite3_exec(
                    m_pDB, 
                    utf16to8(query).c_str(),
                    NULL, NULL, NULL); 
            }
            m_ProtectingLibrary.unlock();

            if (nRet != MUSIK_LIBRARY_OK)
            {
                EndTransaction();
                return nRet;
            }
        }
        EndTransaction();
    }    

    return nID;
}

///////////////////////////////////////////////////

int Library::CreateStdPlaylist(const String& name, Playlist& playlist, int order)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nID, nRet;

    // lock it up
    m_ProtectingLibrary.lock();
    
    // insert the new playlist name
    {
        String query;
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES (NULL, '%ls', %d);"),
            STD_PLAYLIST_TABLE_NAME,
            q_str(name).c_str(),
            order);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);

    }
    m_ProtectingLibrary.unlock();

    if (nRet != MUSIK_LIBRARY_OK)
        return -1;

    nID = (int)sqlite3_last_insert_rowid(m_pDB);

    // insert songs into playlist
    if (nID >= 0)
    {
        BeginTransaction();
        for (size_t i = 0; i < playlist.GetCount(); i++)
        {
            m_ProtectingLibrary.lock();
            {
                String query;
                query.Format(
                    _T(" INSERT INTO %ls ") 
                    _T(" VALUES (NULL, %d, %d); "),
                    STD_PLAYLIST_SONGS,
                    nID,
                    playlist.at(i).GetID());

                nRet = sqlite3_exec(
                    m_pDB, 
                    utf16to8(query).c_str(),
                    NULL, NULL, NULL);

            }
            m_ProtectingLibrary.unlock();

            if (nRet != MUSIK_LIBRARY_OK)
            {
                EndTransaction();
                return nRet;
            }
        }
        EndTransaction();
    }    

    Claim(playlist);

    return nID;
}

///////////////////////////////////////////////////

int Library::AppendStdPlaylist(int id, const StringArray& files)
{
	if (!m_DatabaseOpen) 
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet = 1;
    Song target;

    if (id >= 0)
    {
        BeginTransaction();    
        for (size_t i = 0; i < files.size(); i++)
        {
            AddSong(files.at(i));
            GetSongAttrFromFilename(files.at(i), target);

            m_ProtectingLibrary.lock();
            {
                String query;
                query.Format(
                    _T(" INSERT INTO %ls ")
                    _T(" VALUES (NULL, %d, %d); "),
                    STD_PLAYLIST_SONGS,
                    id,
                    target.GetID());

                nRet = sqlite3_exec(
                    m_pDB, 
                    utf16to8(query.c_str()).c_str(),
                    NULL, NULL, NULL);

            }
            m_ProtectingLibrary.unlock();

            if (nRet != MUSIK_LIBRARY_OK)
            {
                EndTransaction();
                return nRet;
            }
        }
        EndTransaction();
    }
    else
        return SQLITE_ERROR;

    return nRet;
}

///////////////////////////////////////////////////

int Library::AppendStdPlaylist(int id, const Playlist& playlist)
{
	if (!m_DatabaseOpen) 
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;
    String curr_fn;

    if (id >= 0)
    {
        BeginTransaction();    
        for (size_t i = 0; i < playlist.GetCount(); i++)
        {        
            Song curSong = playlist[0];
            if (curSong.GetLibrary() != this)
            {
                continue;
            }

            m_ProtectingLibrary.lock();
            {
                String query;
                query.Format(
                    _T(" INSERT INTO %ls ")
                    _T(" VALUES (NULL, %d, %d); "),
                    STD_PLAYLIST_SONGS,
                    id,
                   playlist.at(i).GetID());

                nRet = sqlite3_exec(
                    m_pDB, 
                    utf16to8(query).c_str(),
                    NULL, NULL, NULL);

            }
            m_ProtectingLibrary.unlock();
           
            if (nRet != MUSIK_LIBRARY_OK)
            {
                EndTransaction();
                return nRet;
            }
        }
        EndTransaction();
    }
    else
        return SQLITE_ERROR;

    return nRet;
}

///////////////////////////////////////////////////

int Library::RewriteDynPlaylistQuery(int id, const String& query)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;
    
    int nRet;

    m_ProtectingLibrary.lock();
    {
        String sQuery;
        sQuery.Format(
            _T(" UPDATE %ls ")
            _T(" SET dyn_playlist_query = '%ls' ")
            _T(" WHERE dyn_playlist_id = %d; "),
            DYN_PLAYLIST_TABLE_NAME,
            q_str(query).c_str(),
            id);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(sQuery).c_str(),
            NULL, NULL, NULL);
    }
    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetDynPlaylistInfo(int id, String& name, String& query)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    BeginTransaction();
    m_ProtectingLibrary.lock();
    
        String sQuery;
        sQuery.Format(
            _T(" SELECT dyn_playlist_name ")
            _T(" FROM %ls ")
            _T(" WHERE dyn_playlist_id = %d; "),
            DYN_PLAYLIST_TABLE_NAME,
            id);
    
        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(sQuery).c_str(), 
            &LibraryCallbacks::GetStringFromRow, 
            &name, 
            NULL);

        if (nRet == MUSIK_LIBRARY_OK)
        {
            sQuery.Format(
                _T(" SELECT dyn_playlist_query ")
                _T(" FROM %ls ")
                _T(" WHERE dyn_playlist_id = %d; "),
                DYN_PLAYLIST_TABLE_NAME,
                id);

            nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(sQuery).c_str(), 
                &LibraryCallbacks::GetStringFromRow, 
                &query, 
                NULL);
        }

    m_ProtectingLibrary.unlock();
    EndTransaction();

    return nRet;
}

///////////////////////////////////////////////////

int Library::RewriteStdPlaylist(Playlist& playlist)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    if (playlist.GetID() >= 0)
    {
        // remove all the old values in the playlist 
        // songs table
        m_ProtectingLibrary.lock();
        {
            String query;
            query.Format(
                _T(" DELETE FROM %ls ")
                _T(" WHERE std_playlist_id = %d; "),
                STD_PLAYLIST_SONGS,
                playlist.GetID());

            nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                NULL, NULL, NULL);

        }
        m_ProtectingLibrary.unlock();

        BeginTransaction();
        
        // add all the songs in the new order
        String fn;
        for (size_t i = 0; i < playlist.GetCount(); i++)
        {
            m_ProtectingLibrary.lock();
                
                String query;
                query.Format(
                    _T(" INSERT INTO %ls ")
                    _T(" VALUES (NULL, %d, %d);"),
                    STD_PLAYLIST_SONGS,
                    playlist.GetID(),
                    playlist.at(i).GetID() );

                nRet = sqlite3_exec(
                    m_pDB, 
                    utf16to8(query).c_str(),
                    NULL, NULL, NULL);

            m_ProtectingLibrary.unlock();

            if (nRet != MUSIK_LIBRARY_OK)
            {
                EndTransaction();
                return nRet;
            }
        }

        EndTransaction();
    }
    else
        return SQLITE_ERROR;

    return nRet;
}

///////////////////////////////////////////////////

int Library::RenameStdPlaylist(int id, const String& str)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    if (id >= 0)
    {
        m_ProtectingLibrary.lock();
    
            String query;
            query.Format(
                _T(" UPDATE %ls ")
                _T(" SET std_playlist_name = '%ls' ")
                _T(" WHERE std_playlist_id = %d;"),
                STD_PLAYLIST_TABLE_NAME,
                q_str(str).c_str(),
                id);

            nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                NULL, NULL, NULL);

        m_ProtectingLibrary.unlock();
    }
    else
        return SQLITE_ERROR;

    return nRet;
}

///////////////////////////////////////////////////

int Library::RenameDynPlaylist(int id, const String& name)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    if (id >= 0)
    {
        m_ProtectingLibrary.lock();
        {
            String query;
            query.Format(
                _T(" UPDATE %ls ")
                _T(" SET dyn_playlist_name = '%ls' ")
                _T(" WHERE dyn_playlist_id = %d; "),
                DYN_PLAYLIST_TABLE_NAME,
                q_str(name).c_str(),
                id);

            nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                NULL, NULL, NULL);
        }
        m_ProtectingLibrary.unlock();
    }
    else
        return SQLITE_ERROR;

    return nRet;
}

///////////////////////////////////////////////////

void Library::GetPlaylistAttr(Playlist& target)
{
	if (!m_DatabaseOpen)
        return;

    target.SetTotalSize(0.0f);
    target.SetTotalTime(0);

    BeginTransaction();
    for (size_t i = 0; i < target.size(); i++)
    {
        GetSongAttrFromID(target.at(i).GetID(), target.at(i));
        target.m_TotalTime += target.at(i).GetDuration();
        target.m_TotalSize += (double)target.at(i).GetFilesize();
    }
    EndTransaction();
}

///////////////////////////////////////////////////

int Library::GetStdPlaylist(int id, Playlist& target, bool clear_target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    String query;
    query.Format(
        _T("   SELECT songs.songid as songid, songs.duration as duration, songs.filesize as filesize")
        _T("   FROM songs, std_playlist_song ")
        _T("   WHERE std_playlist_id = %d ")
        _T("   AND songs.songid=std_playlist_song.songid ")
        _T("   ORDER BY std_playlist_songid ASC "),
        id);

    target.SetID(id);
    target.SetType(musikCore::MUSIK_PLAYLIST_TYPE_STANDARD);

    int res = RawQuerySongs(query, target);
    GetPlaylistAttr(target);

    return res;
}

///////////////////////////////////////////////////

int Library::QueryDynPlaylist(int id, Playlist& target, bool clear_target)
{
	if (!m_DatabaseOpen)
    {
        return MUSIK_LIBRARY_NOT_OPEN;
    }

    if (clear_target)
    {
        target.Clear();
    }

    int nRet;
    
    m_ProtectingLibrary.lock();

        // get the query
        String query, sub;
        query.Format(
            _T(" SELECT dyn_playlist_query ")
            _T(" FROM %ls ")
            _T(" WHERE dyn_playlist_id = %d; "),
            DYN_PLAYLIST_TABLE_NAME,
            id);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::GetStringFromRow, 
            &sub, 
            NULL);

    m_ProtectingLibrary.unlock();

    if (nRet == MUSIK_LIBRARY_OK)
    {
        // get the playlist
        query.Format(
            _T(" SELECT songid, duration, filesize ")
            _T(" FROM %ls ")
            _T(" WHERE %ls; "),
            SONG_TABLE_NAME,
            sub.c_str());

        this->RawQuerySongs(query, target);
    }

    target.SetType(MUSIK_PLAYLIST_TYPE_DYNAMIC);
    target.SetID(id);

    return nRet;
}

///////////////////////////////////////////////////

bool Library::GetDynPlaylistFns(int id, StringArray& target, bool clear_target)
{
	Playlist ids;
    QueryDynPlaylist(id, ids, false);

    if (!ids.GetCount())
        return false;


    if (clear_target)
        target.clear();

    // do it
    BeginTransaction();

    for (size_t i = 0; i < ids.GetCount(); i++)
        target.push_back(ids.GetField(i, MUSIK_LIBRARY_TYPE_FILENAME));

    EndTransaction();

    return (target.size() > 0 ? true : false);
}

///////////////////////////////////////////////////

bool Library::GetDynPlaylistFns(int id, String& target)
{
	Playlist ids;
    QueryDynPlaylist(id, ids, false);

    if (!ids.GetCount())
        return false;

    // do it
    BeginTransaction();

    for (size_t i = 0; i < ids.GetCount(); i++)
    {
        target += ids.GetField(i, MUSIK_LIBRARY_TYPE_FILENAME);
        target += String(_T("\0"));
    }
    target += String(_T("\0"));

    EndTransaction();

    return (target.size() > 0 ? true : false);
}

///////////////////////////////////////////////////

bool Library::GetStdPlaylistFns(Playlist& ids, String& target)
{
	// do it
    BeginTransaction();

    for (size_t i = 0; i < ids.GetCount(); i++)
    {
        target += ids.GetField(i, MUSIK_LIBRARY_TYPE_FILENAME);
        target += String(_T("\0"));
    }
    target += String(_T("\0"));

    EndTransaction();

    return (target.size() > 0 ? true : false);
}

///////////////////////////////////////////////////

bool Library::GetStdPlaylistFns(int id, String& target)
{
	Playlist ids;
    GetStdPlaylist(id, ids, false);

    if (!ids.GetCount())
        return false;

    // do it
    BeginTransaction();

    for (size_t i = 0; i < ids.GetCount(); i++)
    {
        target += ids.GetField(i, MUSIK_LIBRARY_TYPE_FILENAME);
        target += String(_T("\0"));
    }
    target += String(_T("\0"));

    EndTransaction();

    return (target.size() > 0 ? true : false);
}

///////////////////////////////////////////////////

bool Library::GetStdPlaylistFns(int id, StringArray& target, bool clear_target)
{
	Playlist ids;
    GetStdPlaylist(id, ids, false);

    if (!ids.GetCount())
        return false;

    if (clear_target)
        target.clear();

    // do it
    BeginTransaction();

    for (size_t i = 0; i < ids.GetCount(); i++)
        target.push_back(ids.GetField(i, MUSIK_LIBRARY_TYPE_FILENAME));

    EndTransaction();

    return (target.size() > 0 ? true : false);
}

///////////////////////////////////////////////////

bool Library::GetStdPlaylistFns(Playlist& playlist, StringArray& target, bool clear_target)
{
	if (!playlist.GetCount())
        return false;

    if (clear_target)
        target.clear();

    // do it
    BeginTransaction();

    for (size_t i = 0; i < playlist.GetCount(); i++)
        target.push_back(playlist.GetField(i, MUSIK_LIBRARY_TYPE_FILENAME));

    EndTransaction();

    return (target.size() > 0 ? true : false);
}

///////////////////////////////////////////////////

int Library::CreateDynPlaylist(const String& name, const String& query, int order)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nID, nRet;

    // lock it up
    {
        m_ProtectingLibrary.lock();

            // insert the new playlist name
            String sQuery;
            sQuery.Format(
                _T(" INSERT INTO %ls ")
                _T(" VALUES (NULL, '%ls', '%ls', %d); "),
                DYN_PLAYLIST_TABLE_NAME,
                q_str(name).c_str(),
                q_str(query).c_str(),
                order);

            nRet = sqlite3_exec(
                m_pDB,
                utf16to8(sQuery).c_str(), 
                NULL, NULL, NULL);

        m_ProtectingLibrary.unlock();
    }

    if (nRet != MUSIK_LIBRARY_OK)
        return nRet;

    // get the ID of the newly created entry
    nID = (int)sqlite3_last_insert_rowid(m_pDB);

    if (nRet != MUSIK_LIBRARY_OK)
        return nRet;

    return nRet;
}

///////////////////////////////////////////////////

int Library::DeleteStdPlaylist(int id)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    // remove entry from table containing
    // the list of standard playlists
    m_ProtectingLibrary.lock();
    {
        String query;
        query.Format(
            _T(" DELETE FROM %ls ")
            _T(" WHERE std_playlist_id = %d; "),
            STD_PLAYLIST_TABLE_NAME,
            id);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);
    }
    m_ProtectingLibrary.unlock();

    if (nRet != MUSIK_LIBRARY_OK)
        return nRet;

    // now remove corresponding entries in
    // the table that contains the songs
    m_ProtectingLibrary.lock();
    {
        String query;
        query.Format(
            _T(" DELETE FROM %ls ")
            _T(" WHERE std_playlist_id = %d; "),
            STD_PLAYLIST_SONGS,
            id);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);
    }
    m_ProtectingLibrary.unlock();


    return nRet;
}

///////////////////////////////////////////////////

int Library::DeleteDynPlaylist(int id)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    // do it
    int nRet;
    m_ProtectingLibrary.lock();
    
        String query;
        query.Format(
            _T(" DELETE FROM %ls ")
            _T(" WHERE dyn_playlist_id = %d; "),
            DYN_PLAYLIST_TABLE_NAME,
            id);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

String Library::GetOrder(int type, bool terminate, bool by_upper, bool descend)
{
	String sTerminate = _T("");
    if (terminate)
        sTerminate = _T(";");

    String sDescend;
    if (descend)
        sDescend = _T(" desc");
    else
        sDescend = _T(" asc");

    if (!by_upper)
    {
        switch(type)
        {
        case MUSIK_LIBRARY_TYPE_COMMENTS:
            sTerminate.Format(_T("ORDER BY notes%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_ARTIST:
            sTerminate.Format(_T("ORDER BY artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_ALBUM:
            sTerminate.Format(_T("ORDER BY album%ls,tracknum%ls,artist%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_YEAR:
            sTerminate.Format(_T("ORDER BY year%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_GENRE:
            sTerminate.Format(_T("ORDER BY genre%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_TITLE:
            sTerminate.Format(_T("ORDER BY title%ls,artist%ls,album%ls,tracknum%ls,filename%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_TRACKNUM:
            sTerminate.Format(_T("ORDER BY tracknum%ls,filename%ls,artist%ls,album%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_TIMEADDED:
            sTerminate.Format(_T("ORDER BY julianday(timeadded)%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_LASTPLAYED:
            sTerminate.Format(_T("ORDER BY julianday(lastplayed)%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_FILESIZE:
            sTerminate.Format(_T("ORDER BY filesize%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_FORMAT:
            sTerminate.Format(_T("ORDER BY format%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_DURATION:
            sTerminate.Format(_T("ORDER BY duration%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_RATING:
            sTerminate.Format(_T("ORDER BY rating%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
            sTerminate.Format(_T("ORDER BY timesplayed%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_BITRATE:
            sTerminate.Format(_T("ORDER BY bitrate%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_FILENAME:
            sTerminate.Format(_T("ORDER BY filename%ls,artist%ls,album%ls,tracknum%ls,filename%ls,title%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;
        }
    }

    else
    {
        switch(type)
        {
        case MUSIK_LIBRARY_TYPE_COMMENTS:
            sTerminate.Format(_T("ORDER BY UPPER(notes)%ls,UPPER(artist)%ls,UPPER(album)%ls,UPPER(tracknum)%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_ARTIST:
            sTerminate.Format(_T("ORDER BY UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_ALBUM:
            sTerminate.Format(_T("ORDER BY UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(artist)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_YEAR:
            sTerminate.Format(_T("ORDER BY year%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_GENRE:
            sTerminate.Format(_T("ORDER BY UPPER(genre)%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_TITLE:
            sTerminate.Format(_T("ORDER BY UPPER(title)%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_TRACKNUM:
            sTerminate.Format(_T("ORDER BY tracknum%ls,UPPER(filename)%ls,UPPER(artist)%ls,UPPER(album)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_TIMEADDED:
            sTerminate.Format(_T("ORDER BY julianday(timeadded)%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_LASTPLAYED:
            sTerminate.Format(_T("ORDER BY julianday(lastplayed)%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_FILESIZE:
            sTerminate.Format(_T("ORDER BY filesize%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_FORMAT:
            sTerminate.Format(_T("ORDER BY UPPER(format)%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_DURATION:
            sTerminate.Format(_T("ORDER BY duration%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_RATING:
            sTerminate.Format(_T("ORDER BY rating%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_TIMESPLAYED:
            sTerminate.Format(_T("ORDER BY timesplayed%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_BITRATE:
            sTerminate.Format(_T("ORDER BY bitrate%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;

        case MUSIK_LIBRARY_TYPE_FILENAME:
            sTerminate.Format(_T("ORDER BY UPPER(filename)%ls,UPPER(artist)%ls,UPPER(album)%ls,tracknum%ls,UPPER(filename)%ls,UPPER(title)%ls%ls"), 
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sDescend.c_str(),
                sTerminate.c_str());
            return sTerminate;
            break;
        }
    }

    return sTerminate;
}

///////////////////////////////////////////////////

void Library::VerifyYearList(StringArray & list)
{
	size_t count = list.size();

    for (size_t i = 0; i < count ; i++)
    {
        if (list.at(i).IsEmpty() || list.at(i) == _T("0"))
        {
            list.erase(list.begin() + i);
            count--;
        }
    }
}

///////////////////////////////////////////////////

int Library::GetAllSongs(Playlist& target)
{
	int nRet = QuerySongs("filename <> '' ORDER BY artist,album,tracknum", target);

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetAllTempSongs(Playlist& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    target.Clear();

    // lock it up and query it
    int nRet;
    m_ProtectingLibrary.lock();

    String sQuery;
    sQuery.Format(
        _T(" SELECT DISTINCT songid,duration,filesize ")
        _T(" FROM %ls WHERE songid IN (")
        _T("   SELECT songid FROM %ls ")
        _T("); "),
        SONG_TABLE_NAME,
        TEMP_SONG_TABLE_NAME);

    nRet = sqlite3_exec(
        m_pDB, 
        utf16to8(sQuery).c_str(), 
        &LibraryCallbacks::AddSongToPlaylist, 
        &target, 
        NULL);
    m_ProtectingLibrary.unlock();

    Claim(target);

    return nRet;
}

///////////////////////////////////////////////////

int Library::QuickQuery(String str, Playlist& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    str = "%" + str + "%";

    int nRet;
    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT songid,duration,filesize ")
            _T(" FROM %ls ")
            _T(" WHERE artist LIKE '%ls' OR album LIKE '%ls' OR title LIKE '%ls' OR filename LIKE '%ls' OR genre LIKE '%ls' OR notes LIKE '%ls' ORDER BY artist,album,tracknum; "),
            SONG_TABLE_NAME, 
            q_str(str).c_str(),
            q_str(str).c_str(),
            q_str(str).c_str(),
            q_str(str).c_str(),
            q_str(str).c_str(),
            q_str(str).c_str());

    m_ProtectingLibrary.unlock();
    
    nRet = this->RawQuerySongs(query, target);
    return nRet;
}

///////////////////////////////////////////////////

int Library::RawQueryStringArray(const String& query, StringArray& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    target.clear();

    int nRet;
    m_ProtectingLibrary.lock();

    nRet = sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        &LibraryCallbacks::AddSongToStringArray, 
        &target, 
        NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::QuerySongs(const String& query, Playlist& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    target.Clear();

    // lock it up and query it
    int nRet;
    m_ProtectingLibrary.lock();
    {
        String sQuery;
        //
        sQuery.Format(
            _T(" SELECT DISTINCT songid,duration,filesize ")
            _T(" FROM %ls ")
            _T(" WHERE %ls "),
            SONG_TABLE_NAME, 
            query.c_str());

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(sQuery).c_str(), 
            &LibraryCallbacks::AddSongToPlaylist, 
            &target, 
            NULL);

        target.SetQuery(sQuery);
    }
    m_ProtectingLibrary.unlock();

    Claim(target);

    return nRet;
}

///////////////////////////////////////////////////

int Library::QuerySongs(const String& query, Playlist& target, int orderBy)
{
	String newQuery;
    newQuery.Format(_T("%ls %ls"), query.c_str(), Library::GetOrder(orderBy, true).c_str());

    return QuerySongs(newQuery, target);
}

///////////////////////////////////////////////////

int Library::RawQuerySongs(String query, Playlist& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    target.Clear();

    // lock it up and query it
    int nRet;
    m_ProtectingLibrary.lock();

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::AddSongToPlaylist,
            &target,
            NULL);

    m_ProtectingLibrary.unlock();

    query.Replace(L";", L"");
    target.SetQuery(query);

    Claim(target);

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetRelatedSongs(String partial_query, int source_type, PlaylistInfo& info, Playlist& target, bool clear_target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    musikCore::String pre_stmt;
    if (info.GetType() == musikCore::MUSIK_PLAYLIST_TYPE_STANDARD)
    {
        pre_stmt.Format(
            _T(" AND songid IN (SELECT songid FROM std_playlist_song WHERE std_playlist_id = %d) "),
            info.GetID());
    }
    else if (info.GetType() == musikCore::MUSIK_PLAYLIST_TYPE_DYNAMIC)
    {
        m_ProtectingLibrary.lock();

        // get the query
        String query, sub;
        query.Format(
            _T(" SELECT dyn_playlist_query ")
            _T(" FROM %ls ")
            _T(" WHERE dyn_playlist_id = %d; "),
            DYN_PLAYLIST_TABLE_NAME,
            info.GetID());

        sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::GetStringFromRow, 
            &pre_stmt, 
            NULL);

        m_ProtectingLibrary.unlock();

        pre_stmt.Replace(_T(";"), _T(""));
        pre_stmt.Format(
            _T(" AND songid IN (")
            _T("   SELECT songid ")
            _T("   FROM %ls ")
            _T("   WHERE %ls) "),
            SONG_TABLE_NAME,
            pre_stmt.c_str());
    }

    if (clear_target)
        target.Clear();

    // do it
    int nRet;

    m_ProtectingLibrary.lock();
    String query;

    query.Format(
        _T(" SELECT DISTINCT songid,duration,filesize ")
        _T(" FROM (")
        _T(" %ls%ls) ")
        _T(" %ls "),
        partial_query.c_str(),
        pre_stmt.c_str(),
        Library::GetOrder(source_type).c_str());

    nRet = sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        &LibraryCallbacks::AddSongToPlaylist, 
        &target, 
        NULL);

    m_ProtectingLibrary.unlock();

    Claim(target);

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetRelatedSongs(String partial_query, int source_type, Playlist& target, bool sub_query, bool clear_target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (clear_target)
        target.Clear();

    // do it
    int nRet;

    m_ProtectingLibrary.lock();
        String query;

        if (!sub_query)
        {
            query.Format(
                _T(" SELECT DISTINCT songid,duration,filesize ")
                _T(" FROM %ls ")
                _T(" %ls ")
                _T(" %ls; "),
                SONG_TABLE_NAME,
                partial_query.c_str(),
                Library::GetOrder(source_type).c_str());

            nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(), 
                &LibraryCallbacks::AddSongToPlaylist, 
                &target, 
                NULL);
        }
        else
        {
            query.Format(
                _T(" SELECT DISTINCT songid,duration,filesize ")
                _T(" FROM (%ls) ")
                _T(" %ls; "),
                partial_query.c_str(),
                Library::GetOrder(source_type).c_str());

            nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(), 
                &LibraryCallbacks::AddSongToPlaylist, 
                &target, 
                NULL);
        }

    m_ProtectingLibrary.unlock();

    Claim(target);

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetOpenTransactions() const
{
	return m_Transactions;
}

///////////////////////////////////////////////////

////here

int Library::GetAllDistinct(int source_type, StringArray& target, bool clear_target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (clear_target)
        target.clear();

    String sField = GetSongFieldDBStr(source_type);

    // do it
    int nRet;
    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT DISTINCT %ls ")
            _T(" FROM  %ls ")
            _T(" GROUP BY %ls ")
            _T(" ORDER BY LOWER(%ls) ASC; "),
            sField.c_str(), 
            SONG_TABLE_NAME,
            sField.c_str(),
            sField.c_str());

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddSongToStringArray, &target, NULL);

    m_ProtectingLibrary.unlock();

    if (source_type == MUSIK_LIBRARY_TYPE_YEAR)
        VerifyYearList(target);

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetDistinctCount(int field)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    String query;
    query.Format(
        _T("SELECT COUNT(*) FROM (SELECT DISTINCT %ls FROM %ls);"), 
        GetSongFieldDBStr(field).c_str(),
        SONG_TABLE_NAME);

    int result = 0;
    sqlite3_exec(
        m_pDB,
        utf16to8(query).c_str(),
        LibraryCallbacks::GetIntFromRow,
        &result,
        NULL);

    return result;
}

///////////////////////////////////////////////////

int Library::GetSongCount()
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    String query;
    query.Format(
        _T("SELECT COUNT(*) FROM %ls;"), 
        SONG_TABLE_NAME);

    int result = 0;
    sqlite3_exec(
        m_pDB,
        utf16to8(query).c_str(),
        LibraryCallbacks::GetIntFromRow,
        &result,
        NULL);

    return result;
}

///////////////////////////////////////////////////

int Library::GetFieldFromID(int id, int field, String& string)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    String type = GetSongFieldDBStr(field);

    // do it
    int nRet;
    m_ProtectingLibrary.lock();
    {
        String query;
        query.Format(
            _T(" SELECT %ls ")
            _T(" FROM %ls ")
            _T(" WHERE songid = %d; "),
            type.c_str(),
            SONG_TABLE_NAME, 
            id);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::GetFieldFromID, 
            &string, 
            NULL);

    }
    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetSongInfoFromID(int id, SongInfo& info)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    info.SetID(id);
	
	Player *player = GetPlayer();

    // do it
    int nRet;
    m_ProtectingLibrary.lock();
    {
        String query;

        query.Format(
            _T(" SELECT tracknum,artist,album,genre,title,duration,format,vbr,year,rating,bitrate,datetime(lastplayed,'%ls'),notes,timesplayed,datetime(timeadded,'%ls'),filesize,filename,equalizer,dirty ")
            _T(" FROM %ls ")
            _T(" WHERE songid = %d;"),
            ::g_UTC ? _T("utc") : _T("localtime"),
            ::g_UTC ? _T("utc") : _T("localtime"),
            SONG_TABLE_NAME,
            id);

		int (*callback)( void*, int, char**, char** );
		callback = ( player->g_isNext ? &LibraryCallbacks::GetSongInfoFromIDUpdate : &LibraryCallbacks::GetSongInfoFromID );

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            //&LibraryCallbacks::GetSongInfoFromID, 
			callback,
            &info, 
            NULL);
    }
    m_ProtectingLibrary.unlock();

    info.SetID(id);
    info.SetLibrary(this);

    return nRet;
}

///////////////////////////////////////////////////

bool Library::SetSongInfo(SongInfo& info, int songid)
{
	if (!m_DatabaseOpen)
        return false;

    int result = 0;

    if (songid == -1)
        songid = info.GetID();

    // lock it up and run the query
    m_ProtectingLibrary.lock();

        //
        // break into two parts because CStdString::Format can't take 21 args
        //

        String query;
        query.Format(
            _T(" UPDATE %ls ")
            _T(" SET format = '%ls', vbr = %d, filename = '%ls', artist = '%ls', title = '%ls', album = '%ls', tracknum = %d, year = %d, genre = '%ls', "),
            SONG_TABLE_NAME,
            q_str(info.GetFormat()).c_str(),
            StringToInt(info.GetVBR()),

#if defined (WIN32) || defined (__MINGW32__)
            q_str(info.GetFilename().ToLower()).c_str(),                // filename (lowercase for win32)
#else
            q_str(info.GetFilename()).c_str(),                        // filename 
#endif

            q_str(info.GetArtist()).c_str(),
            q_str(info.GetTitle()).c_str(), 
            q_str(info.GetAlbum()).c_str(),
            StringToInt(info.GetTrackNum()),
            StringToInt(info.GetYear()),
            q_str(info.GetGenre()).c_str());
    
        query.Format(
            _T("%ls ")
            _T(" rating = %d, bitrate = %d, lastplayed = '%ls', notes = '%ls', timesplayed = %d, duration = %d, timeadded = '%ls', filesize = %d, dirty = %d")
            _T(" WHERE songid = %d; "),
            query.c_str(),
            StringToInt(info.GetRating()),
            StringToInt(info.GetBitrate()),
            info.GetLastPlayed().c_str(),
            q_str(info.GetNotes()).c_str(),
            StringToInt(info.GetTimesPlayed()),
            StringToInt(info.GetDuration()),
            info.GetTimeAdded().c_str(),
            StringToInt(info.GetFilesize()),
            StringToInt(info.GetDirtyFlag()),
            songid);

        result = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    if (result != MUSIK_LIBRARY_OK)
        return false;
    
    return true;    
}

///////////////////////////////////////////////////

bool Library::SetSongRating(int songid, int rating)
{
	if (!m_DatabaseOpen)
        return false;

    int result = 0;

    // lock it up and run the query
    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" UPDATE %ls ")
            _T(" SET rating = %d ")
            _T(" WHERE songid = %d; "),
            SONG_TABLE_NAME,
            rating,
            songid);

        result = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    if (result != MUSIK_LIBRARY_OK)
        return false;
    
    return true;    
}

///////////////////////////////////////////////////

bool Library::SetSongEqualizer(int songid, int eq_id)
{
	if (!m_DatabaseOpen)
        return false;

    int result = 0;

    // lock it up and run the query
    m_ProtectingLibrary.lock();

        String query;

        query.Format(
            _T(" UPDATE %ls ")
            _T(" SET equalizer = %d ")
            _T(" WHERE songid = %d; "),
            SONG_TABLE_NAME,
            eq_id,
            songid);

        result = sqlite3_exec(
            m_pDB,
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    if (result != MUSIK_LIBRARY_OK)
        return false;
    
    return true;  
}

///////////////////////////////////////////////////

int Library::GetAllStdPlaylists(PlaylistInfoArray& target, bool clear_target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (clear_target)
        target.clear();

    // do it
    int nRet;
    m_ProtectingLibrary.lock();

        String query;

        query.Format(
            _T(" SELECT std_playlist_name,std_playlist_id,std_playlist_order")
            _T(" FROM %ls ")
            _T(" ORDER BY std_playlist_order ASC; "),
            STD_PLAYLIST_TABLE_NAME);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::AddStdPlaylistInfoArray, 
            &target, 
            NULL);

    m_ProtectingLibrary.unlock();

    // make sure the correct type is set
    for (size_t i = 0; i < target.size(); i++)
        target.at(i).m_Type = MUSIK_PLAYLIST_TYPE_STANDARD;

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetAllDynPlaylists(PlaylistInfoArray& target, bool clear_target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (clear_target)
        target.clear();

    // do it
    int nRet;
    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT dyn_playlist_name,dyn_playlist_id,dyn_playlist_order ")
            _T(" FROM %ls ")
            _T(" ORDER BY dyn_playlist_order ASC; "),
            DYN_PLAYLIST_TABLE_NAME);

        nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                &LibraryCallbacks::AddDynPlaylistInfoArray, 
                &target, 
                NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetAllCrossfaderPresets(StringArray& target, IntArray& target_ids, bool clear_targets)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (clear_targets)
    {
        target.clear();
        target_ids.clear();
    }

    int nRet1, nRet2 = 0;
    BeginTransaction();

    m_ProtectingLibrary.lock();

        //
        // names
        //
        String query;
        query.Format(
            _T(" SELECT crossfader_name ")
            _T(" FROM %ls ")
            _T(" ORDER BY crossfader_id; "),
            CROSSFADER_PRESET);

        nRet1 = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddRowToStringArray, 
            &target, 
            NULL);

        //
        // ids
        //
        query.Format(
            _T(" SELECT crossfader_id ")
            _T(" FROM %ls ")
            _T(" ORDER BY crossfader_id; "),
            CROSSFADER_PRESET);

        nRet2 = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::AddRowToIntArray, 
            &target_ids, 
            NULL);    

        m_ProtectingLibrary.unlock();

    EndTransaction();

    if (nRet1 != 0)
        return nRet1;

    return nRet2;
}

///////////////////////////////////////////////////

int Library::GetCrossfader(int id, Crossfader& fader)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT newsong,pause_resume,seek,stop,exit,crossfader_name ")
            _T(" FROM %ls ")
            _T(" WHERE crossfader_id = %d; "),
            CROSSFADER_PRESET,
            id);

        nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(), 
                &LibraryCallbacks::GetCrossfader, 
                &fader, 
                NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetSongAttrFromFilename(const String& fn, Song& song)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    // do it
    int nRet;
    m_ProtectingLibrary.lock();

    String realfn = fn;
#if defined(WIN32) || defined(__MINGW32__)
    realfn.ToLower();
#endif

    String query;
    query.Format(
        _T(" SELECT songid,duration,filesize ")
        _T(" FROM %ls ")
        _T(" WHERE filename = '%ls'; "),
        SONG_TABLE_NAME,
        q_str(realfn).c_str());

    nRet = sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        &LibraryCallbacks::GetSongAttr, 
        &song, 
        NULL);

    m_ProtectingLibrary.unlock();
    song.SetLibrary(this);

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetSongAttrFromID(int id, Song& song)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    // do it
    int nRet;
    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT songid,duration,filesize ")
            _T(" FROM %ls ")
            _T(" WHERE songid = %d; "),
            SONG_TABLE_NAME,
            id);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::GetSongAttr,
            &song, 
            NULL);

    m_ProtectingLibrary.unlock();
    song.SetLibrary(this);

    return nRet;
}

///////////////////////////////////////////////////

bool Library::RemoveSong(int songid)
{
	if (!m_DatabaseOpen)
        return false;

    int nRes;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" DELETE FROM %ls ")
            _T(" WHERE songid = %d; "),
            SONG_TABLE_NAME,
            songid);

        nRes = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    if (nRes == MUSIK_LIBRARY_OK)
        return true;

    return false;
}    

///////////////////////////////////////////////////

bool Library::RemoveSong(const String& fn)
{
	if (!m_DatabaseOpen)
        return false;

    int nRet;

    m_ProtectingLibrary.lock();

    String realfn = fn;
#if defined(WIN32) || defined(__MINGW32__)
    realfn.ToLower();
#endif

    String query;
    query.Format(
        _T(" DELETE FROM %ls ")
        _T(" WHERE filename = '%ls'; "),
        SONG_TABLE_NAME,
        q_str(realfn).c_str());

    nRet = sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    if (nRet == MUSIK_LIBRARY_OK)
        return true;

    return false;
}    

///////////////////////////////////////////////////

bool Library::AddSong(const String& fn)
{
	bool result = false;

    if (!IsSongInLibrary(fn))
    {
        Filename MFN(fn);
        String sExt = MFN.GetExtension();


#if defined (USE_BASS)

        // netstreams are treated specially
        if (fn.Left(7).ToLower() == _T("http://"))
        {
            SongInfo info;
            info.SetLibrary(this);
            info.SetFilename(fn);
            info.SetTitle(fn);
            info.SetFormat(_T("net"));
            info.SetArtist(_T("netstream"));
            info.SetAlbum(_T("netstream"));
            result = AddFile(info);
        }

        // regular formats
        else
        {
            for (size_t i = 0; i < PlayerBASS::s_Plugins.size(); i++)
            {
                if (PlayerBASS::s_Plugins[i]->CanPlay(fn))
                {
                    SongInfo info;
                    info.SetLibrary(this);
                    info.SetFilename(fn);
                    PlayerBASS::s_Plugins[i]->ReadTag(fn, info);
                    result = AddFile(info);
                    break;
                }
            }
        }

#else

        //
        // this needs to be fixed for other operating systems. basically,
        // it needs to look at the filename, figure out if it is a supported
        // type, read the tag into a SongInfo data structure, and call
        // AddFile(const SongInfo& tag);
        //

        SongInfo info;
        info.LoadInfo(fn);
               return AddFile(info);    
#endif    // USE_BASS
    }
    else
        result = true;    // song already in library

    return result;
}

///////////////////////////////////////////////////

void Library::ClearTempSongTable()
{
	if (!m_DatabaseOpen)
        return;

    m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, "delete from temp_songs;", NULL, NULL, NULL);
    m_ProtectingLibrary.unlock();
}

///////////////////////////////////////////////////

void Library::PopulateTempSongTable(Playlist& source)
{
	if (!m_DatabaseOpen)
        return;

    ClearTempSongTable();
    String query;

    BeginTransaction();
    for (size_t i = 0; i < source.GetCount(); i++)
    {
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES (%d); "),
            TEMP_SONG_TABLE_NAME,
            source.at(i).GetID());

        sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);
    }

    EndTransaction();
}

///////////////////////////////////////////////////

bool Library::AddFile(const SongInfo& info)
{
	if (!m_DatabaseOpen)
        return false;

    int nRet;

    m_ProtectingLibrary.lock();

        int isvbr = StringToInt(info.GetVBR());
        int tracknum = StringToInt(info.GetTrackNum());
        int bitrate = StringToInt(info.GetBitrate());
        int duration = StringToInt(info.GetDuration());
        int filesize = Filename::GetFilesize(info.GetFilename());

        // these may already exist, especially if we're
        // patching the library. 
        String str_songid = info.GetID() == -1 ? _T("NULL") : IntToString(info.GetID());
        String str_rating = info.GetRating().IsEmpty() ? _T("0") : info.GetRating();

        String query;
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES (%ls, '%ls', %d, '%ls', '%ls', '%ls', '%ls', %d, '%ls', '%ls', %ls, %d, %ls, '%ls', "),
            SONG_TABLE_NAME,                                            // song table         
            str_songid.c_str(),                                            // song id
            q_str(info.GetFormat()).c_str(),                            // format
            isvbr,                                                        // vbr

#if defined (WIN32) || defined (__MINGW32__)
            q_str(info.GetFilename().ToLower()).c_str(),                // filename (lowercase for windows)
#else
            q_str(info.GetFilename()).c_str(),                        // filename
#endif

            q_str(info.GetArtist()).c_str(),                            // artist 
            q_str(info.GetTitle()).c_str(),                            // title
            q_str(info.GetAlbum()).c_str(),                            // album
            tracknum,                                                    // tracknum
            q_str(info.GetYear()).c_str(),                            // year
            q_str(info.GetGenre()).c_str(),                            // genre
            q_str(str_rating).c_str(),                                // rating
            bitrate,                                                    // bitrate
            _T("0"),                                                    // last played
            q_str(info.GetNotes()).c_str());                            // notes

        query.Format(
            _T("%ls %d, %d, %ls, %d, %d, %d, %d, %d, %d);"),
            query.c_str(),            
            0,                                                            // times played
            duration,                                                       // duration
            _T("julianday('now')"),                                   // time added
            filesize,                                                    // file size
            0,                                                            // dirty
            -1,                                                            // default equalizer
            0,                                                            // average playtime (0-100%)
            0,                                                            // times user chose song
            0);                                                        // times song was advanced automatically

            nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(), 
                NULL, NULL, NULL);
    
    m_ProtectingLibrary.unlock();

    if (nRet == MUSIK_LIBRARY_OK)
        return true;

    return false;
}

///////////////////////////////////////////////////

bool Library::AddNet(const String & fn)
{
	if (!m_DatabaseOpen)
        return false;
    
    int nRet;
    
    m_ProtectingLibrary.lock();

        String query;
           
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES (NULL, %d, %d, '%ls', '%ls', '%ls', '%ls', '%ls', %d, '%ls', '%ls', "),
            SONG_TABLE_NAME,                                              // song table 
            _T("net"),                                                // format
            0,                                                            // vbr
            q_str(fn).c_str(),                                        // filename
            _T("Net Radio"),                                          // artist 
            q_str(fn).c_str(),                                        // title
            _T("Net Radio"),                                            // album
            0,                                                            // tracknum
            _T(""),                                                   // year
            _T("Net Radio"));                                        // genre
           
        query.Format(
            _T("%ls %d, %d, %d, '%ls', %d, %d, %ls, %d, %d, %d, %d, %d, %d);"),
            query.c_str(),
            0,                                                            // rating
            0,                                                            // bitrate
            0,                                                            // last played
            _T(""),                                                    // notes
            0,                                                            // times played
            0,                                                            // duration
            _T("julianday('now')"),                                   // time added
            0,                                                            // file size
            0,                                                            // dirty
            -1,                                                            // default equalizer
            0,                                                            // average playtime (0-100%)
            0,                                                            // times user chose song
            0);                                                        // times song was advanced automatically


        nRet = sqlite3_exec(
                m_pDB,
                utf16to8(query).c_str(), 
                NULL, NULL, NULL);    


    m_ProtectingLibrary.unlock();

    if (nRet == MUSIK_LIBRARY_OK)
        return true;
    
    return false;
}

///////////////////////////////////////////////////

bool Library::IsSongInLibrary(const String& fn)
{
	if (!m_DatabaseOpen)
        return false;

    // case insensitive for windows

    String realfn = fn;
#if defined(WIN32) || defined(__MINGW32__)
    realfn.ToLower();
#endif

    String query;
    query.Format ( 
        _T(" SELECT COUNT(*) ") 
        _T("   FROM %ls ") 
        _T("   WHERE filename = '%ls'; "), 
        SONG_TABLE_NAME, 
        q_str(realfn).c_str());

    int result;
    m_ProtectingLibrary.lock();
        sqlite3_exec(
            m_pDB,
            utf16to8(query).c_str(),
            LibraryCallbacks::GetIntFromRow, 
            &result,
            NULL);
    m_ProtectingLibrary.unlock();

    if (result > 0)
        return true;

    return false;
}

///////////////////////////////////////////////////

bool Library::IsSongInLibrary(int songid)
{
	if (!m_DatabaseOpen)
        return false;

    String query;
    query.Format (
        _T(" SELECT COUNT(*) ")
        _T(" FROM %ls ")
        _T(" WHERE songid = %d; "),
        SONG_TABLE_NAME,
        songid);

    int result;
    m_ProtectingLibrary.lock();
        sqlite3_exec(
            m_pDB,
            utf16to8(query).c_str(),
            LibraryCallbacks::GetIntFromRow, 
            &result,
            NULL);
    m_ProtectingLibrary.unlock();

    if (result > 0)
        return true;

    return false;
}

///////////////////////////////////////////////////

int Library::GetEqualizerIDFromSongID(int id)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int target;

    // do it
    m_ProtectingLibrary.lock();
        
        String query;
        query.Format(
            _T(" SELECT equalizer ")
            _T(" FROM %ls ")
            _T(" WHERE songid = %d; "),
            SONG_TABLE_NAME,
            id);

        sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::GetIntFromRow, 
            &target, 
            NULL);

    m_ProtectingLibrary.unlock();

    return target;
}

///////////////////////////////////////////////////

int Library::GetCrossfaderIDFromName(const String& name)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int target = -1;

    // do it
    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT crossfader_id ")
            _T(" FROM %ls ")
            _T(" WHERE crossfader_name = '%ls'; "),
            CROSSFADER_PRESET,
            q_str(name).c_str());

        sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::GetIntFromRow, 
            &target, 
            NULL);

    m_ProtectingLibrary.unlock();

    return target;
}

///////////////////////////////////////////////////

int Library::GetEqualizerIDFromName(const String& name)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int target;

    // do it
    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT COUNT (*)")
            _T(" FROM %ls ")
            _T(" WHERE equalizer_name = '%ls'; "),
            EQUALIZER_PRESET,
            q_str(name).c_str());

        sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::GetIntFromRow, 
            &target, 
            NULL);

    m_ProtectingLibrary.unlock();

    if (target <= 0)
        return -1;

    return target;
}

///////////////////////////////////////////////////

// crossfader name must be unique

int Library::CreateCrossfader(Crossfader& fader)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (GetCrossfaderIDFromName(fader.m_Name) != -1)
        return MUSIK_LIBRARY_ID_EXISTS;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES (NULL, '%ls', %f, %f, %f, %f, %f); "),
            CROSSFADER_PRESET,
            q_str(fader.m_Name).c_str(),
            fader.m_NewSong,
            fader.m_PauseResume,
            fader.m_Seek,
            fader.m_Stop,
            fader.m_Exit);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    fader.m_ID = (int)sqlite3_last_insert_rowid(m_pDB);

    return nRet;
}

///////////////////////////////////////////////////

// equalizer name must be unique

int Library::CreateEqualizer(EQSettings& eq, bool is_preset)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRes;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES (NULL, '%ls', %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, "),
            EQUALIZER_PRESET,                               // eq table
            q_str(eq.m_Name).c_str(),                        // name
            (int)is_preset,                                    // preset?
            eq.m_Left[0],
            eq.m_Left[1],
            eq.m_Left[2],
            eq.m_Left[3],
            eq.m_Left[4],
            eq.m_Left[5],
            eq.m_Left[6],
            eq.m_Left[7],
            eq.m_Left[8],
            eq.m_Left[9]);

        query.Format(
            _T("%ls %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, "),
            query.c_str(),
            eq.m_Left[10],
            eq.m_Left[11],
            eq.m_Left[12],
            eq.m_Left[13],
            eq.m_Left[14],
            eq.m_Left[15],
            eq.m_Left[16],
            eq.m_Left[17],
            eq.m_Right[0],
            eq.m_Right[1],
            eq.m_Right[2],
            eq.m_Right[3],
            eq.m_Right[4],
            eq.m_Right[5],
            eq.m_Right[6]);

        query.Format(
            _T("%ls %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f);"),
            query.c_str(),
            eq.m_Right[7],
            eq.m_Right[8],
            eq.m_Right[9],
            eq.m_Right[10],
            eq.m_Right[11],
            eq.m_Right[12],
            eq.m_Right[13],
            eq.m_Right[14],
            eq.m_Right[15],
            eq.m_Right[16],
            eq.m_Right[17]);

        nRes = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);    

    m_ProtectingLibrary.unlock();

    eq.m_ID = (int)sqlite3_last_insert_rowid(m_pDB);

    return nRes;
}

///////////////////////////////////////////////////

int Library::DeleteEqualizer(int id)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRes;
    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" DELETE FROM %ls ")
            _T(" WHERE equalizer_id = %d; "),
            EQUALIZER_PRESET,    
            id);

        nRes = sqlite3_exec(
            m_pDB,
            utf16to8(query).c_str(),
            NULL, NULL, NULL); 

    m_ProtectingLibrary.unlock();

    return nRes;
}

///////////////////////////////////////////////////

int Library::UpdateEqualizer(int id, const EQSettings& eq)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRes;

    m_ProtectingLibrary.lock();

        String query;

        query.Format(
            _T(" UPDATE %ls ")
            _T(" SET equalizer_name = '%ls', hz55_left = %f, hz77_left = %f, hz110_left = %f, hz156_left = %f, hz220_left = %f, hz311_left = %f, hz440_left = %f, hz622_left = %f, hz880_left = %f, hz1244_left = %f, hz1760_left = %f, hz2489_left = %f, hz3520_left = %f, hz4978_left = %f, "),
                EQUALIZER_PRESET,
                q_str(eq.m_Name).c_str(),
                eq.m_Left[0],
                eq.m_Left[1],
                eq.m_Left[2],
                eq.m_Left[3],
                eq.m_Left[4],
                eq.m_Left[5],
                eq.m_Left[6],
                eq.m_Left[7],
                eq.m_Left[8],
                eq.m_Left[9],
                eq.m_Left[10],
                eq.m_Left[11],
                eq.m_Left[12],
                eq.m_Left[13]);

        query.Format(
            _T(" %ls ")
            _T("  hz7040_left = %f, hz9956_left = %f, hz14080_left = %f, hz19912_left = %f, ")
            _T(" hz55_right = %f, hz77_right = %f, hz110_right = %f, hz156_right = %f, hz220_right = %f, hz311_right = %f, hz440_right = %f, hz622_right = %f, hz880_right = %f, hz1244_right = %f, "),
                query.c_str(),
                eq.m_Left[14],
                eq.m_Left[15],
                eq.m_Left[16],
                eq.m_Left[17],
                eq.m_Right[0],
                eq.m_Right[1],
                eq.m_Right[2],
                eq.m_Right[3],
                eq.m_Right[4],
                eq.m_Right[5],
                eq.m_Right[6],
                eq.m_Right[7],
                eq.m_Right[8],
                eq.m_Right[9]);

        query.Format(
            _T(" %ls ")
            _T(" hz1760_right = %f, hz2489_right = %f, hz3520_right = %f, hz4978_right = %f, hz7040_right = %f, hz9956_right = %f, hz14080_right = %f, hz19912_right = %f ")
            _T(" WHERE equalizer_id = %d; "),
                query.c_str(),
                eq.m_Right[10],
                eq.m_Right[11],
                eq.m_Right[12],
                eq.m_Right[13],
                eq.m_Right[14],
                eq.m_Right[15],
                eq.m_Right[16],
                eq.m_Right[17],
                id);

        nRes = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRes;
}

///////////////////////////////////////////////////

int Library::UpdateDefaultCrossfader(const Crossfader& fader)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRes;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" UPDATE %ls ")
            _T(" SET newsong = %f, pause_resume = %f, seek = %f, stop = %f, exit = %f ")
            _T(" WHERE crossfader_id = -1;"),
            CROSSFADER_DEFAULT,
            fader.m_NewSong,
            fader.m_PauseResume,
            fader.m_Seek,
            fader.m_Stop,
            fader.m_Exit);

        nRes = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRes;
}

///////////////////////////////////////////////////

int Library::UpdateDefaultEqualizer(const EQSettings& eq)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRes;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" DELETE FROM %ls; ")
            _T(" INSERT INTO %ls VALUES(-1, '', 0, ")
            _T(" %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, "),
            EQUALIZER_DEFAULT,
            EQUALIZER_DEFAULT,
            eq.m_Left[0],
            eq.m_Left[1],
            eq.m_Left[2],
            eq.m_Left[3],
            eq.m_Left[4],
            eq.m_Left[5],
            eq.m_Left[6],
            eq.m_Left[7],
            eq.m_Left[8],
            eq.m_Left[9],
            eq.m_Left[10],
            eq.m_Left[11],
            eq.m_Left[12],
            eq.m_Left[13],
            eq.m_Left[14],
            eq.m_Left[15]);

        query.Format(
            _T(" %ls  %f, %f, ")
            _T(" %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, "),
            query.c_str(),
            eq.m_Left[16],
            eq.m_Left[17],
            eq.m_Right[0],
            eq.m_Right[1],
            eq.m_Right[2],
            eq.m_Right[3],
            eq.m_Right[4],
            eq.m_Right[5],
            eq.m_Right[6],
            eq.m_Right[7],
            eq.m_Right[8],
            eq.m_Right[9],
            eq.m_Right[10],
            eq.m_Right[11],
            eq.m_Right[12],
            eq.m_Right[13]);

        query.Format(
            _T(" %ls  %f, %f, %f, %f ")
            _T("); "),
            query.c_str(),
            eq.m_Right[14],
            eq.m_Right[15],
            eq.m_Right[16],
            eq.m_Right[17]);

        nRes = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRes;
}

///////////////////////////////////////////////////

int Library::GetDefaultCrossfader(Crossfader& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT newsong,pause_resume,seek,stop,exit ")
            _T(" FROM %ls ")
            _T(" WHERE crossfader_id = -1; "),
            CROSSFADER_DEFAULT);

        nRet = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(), 
                &LibraryCallbacks::GetCrossfader, 
                &target, 
                NULL);
    
    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetDefaultEqualizer(EQSettings& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT * ")
            _T(" FROM %ls ")
            _T(" WHERE equalizer_id = -1; "),
            EQUALIZER_DEFAULT);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::GetEqualizer, 
            &target, 
            NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::ResetDefaultEqualizer()
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRes;

    m_ProtectingLibrary.lock();

        String query;

        query.Format(
            _T(" DELETE FROM equalizer_default;"));
        
        sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);


        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES ( -1, '', 0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,")
            _T(" 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);"),
            EQUALIZER_DEFAULT);

        nRes = sqlite3_exec(
                    m_pDB, 
                    utf16to8(query).c_str(),
                    NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRes;    
}

///////////////////////////////////////////////////

int Library::ResetDefaultCrossfader()
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRes;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" DELETE FROM crossfader_default; "));

        sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);


        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES (-1, 2.0, 0.5, 0.2, 1.0, 3.0); "),
            CROSSFADER_DEFAULT);

        nRes = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRes;
}

///////////////////////////////////////////////////

int Library::GetEqualizer(int eq_id, EQSettings& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT * ")
            _T(" FROM %ls ")
            _T(" WHERE equalizer_id = %d; "),
            EQUALIZER_PRESET,
            eq_id);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            &LibraryCallbacks::GetEqualizer, 
            &target, 
            NULL);

    m_ProtectingLibrary.unlock();
    
    if (nRet == 0)
        target.m_ID = eq_id;
    else
        target.m_ID = -2;

    return nRet;
}

///////////////////////////////////////////////////

// this could probably be a bit faster,
// but i seriously doubt anyone will
// ever notice unless they have something
// like 10,000 equalizer presets on a 486 

int  Library::GetAllEqualizerPresets(StringArray& target, IntArray& target_ids, bool clear_targets)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (clear_targets)
    {
        target.clear();
        target_ids.clear();
    }

    int nRet1, nRet2 = 0;
    BeginTransaction();

    m_ProtectingLibrary.lock();

        String query;

        query.Format(
            _T(" SELECT equalizer_name ")
            _T(" FROM %ls ")
            _T(" WHERE equalizer_is_preset = 1 ")
            _T(" ORDER BY equalizer_id;"),
            EQUALIZER_PRESET);

        nRet1 = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddRowToStringArray, 
            &target, 
            NULL);

        
        query.Format(
            _T(" SELECT equalizer_id ")
            _T(" FROM %ls ")
            _T(" WHERE equalizer_is_preset = 1 ")
            _T(" ORDER BY equalizer_id; "),
            EQUALIZER_PRESET);    

        nRet2 = sqlite3_exec(
            m_pDB,
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddRowToIntArray, 
            &target_ids, 
            NULL);

    
    m_ProtectingLibrary.unlock();

    EndTransaction();

    if (nRet1 != 0)
        return nRet1;

    return nRet2;
}

///////////////////////////////////////////////////

int Library::GetSongFormatFromID(int id, String& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

    String query;
    query.Format(
        _T(" SELECT format ")
        _T(" FROM %ls ")
        _T(" WHERE songid = %d; "),
        SONG_TABLE_NAME,
        id);

    nRet = sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        &LibraryCallbacks::GetStringFromRow, &target, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

void Library::GetInfoArrayFromPlaylist(Playlist& playlist, SongInfoArray& info, int replace_field_type, String new_field, bool clear)
{
	if (clear)
        info.clear();

    SongInfo tmp_info;
    Library* curr;
    std::stack<Library*> trans;
    for (size_t i = 0; i < playlist.GetCount(); i++)
    {
        curr = playlist.at(i).GetLibrary();
        if (!curr->TransactionOpen())
        {
            curr->BeginTransaction();
            trans.push(curr);
        }

        curr->GetSongInfoFromID(playlist.GetSongID(i), tmp_info);
        if (replace_field_type != -1)
            tmp_info.SetField(replace_field_type, new_field);

        info.push_back(tmp_info);
    }

    // kill open transactions
    while (trans.size())
    {
        curr = trans.top();
        trans.pop();
        curr->EndTransaction();
    }
}

///////////////////////////////////////////////////

void Library::GetInfoArrayFromPlaylist(Playlist& playlist, SongInfoArray& info, int from, int to)
{
	info.clear();

    SongInfo tmp_info;
    Library* curr;
    std::stack<Library*> trans;

    for (int i = from; i < to; i++)
    {
        curr = playlist.at(i).GetLibrary();
        if (!curr->TransactionOpen())
        {
            curr->BeginTransaction();
            trans.push(curr);
        }

        curr->GetSongInfoFromID(playlist.GetSongID(i), tmp_info);
        info.push_back(tmp_info);
    }

    // kill open transactions
    while (trans.size())
    {
        curr = trans.top();
        trans.pop();
        curr->EndTransaction();
    }
}

///////////////////////////////////////////////////

int Library::GetDirtySongs(Playlist& target)
{
	target.SetType(MUSIK_PLAYLIST_TYPE_STANDARD);
    return QuerySongs("dirty = 1", target);
}

///////////////////////////////////////////////////

int Library::FinalizeDirtySongs()
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" UPDATE %ls ")
            _T(" SET dirty = 0 ")
            _T(" WHERE dirty = 1 "),
            SONG_TABLE_NAME);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}


///////////////////////////////////////////////////

int Library::AddMask(const String& mask)  //Somebody STOP me!
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES ('%ls'); "),
            FILENAME_MASK,
            q_str(mask).c_str());

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::RemoveMask(const String& mask)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" DELETE FROM %ls ")
            _T(" WHERE filename_mask = '%ls'; "),
            FILENAME_MASK,
            q_str(mask).c_str());

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetAllMasks(StringArray& target, bool clear_target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (clear_target)
        target.clear();

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT filename_mask ")
            _T(" FROM %ls ")
            _T(" WHERE filename_mask <> ''; "),
            FILENAME_MASK);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddRowToStringArray, 
            &target, 
            NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::AddChgCase(const String& word)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();
    
        String query;
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES ('%ls'); "),
            CHANGE_CASE_TABLE,
            q_str(word).c_str());

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::RemoveChgCase(const String& word)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();
        
        String query;
        query.Format(
            _T(" DELETE FROM %ls ")
            _T(" WHERE case_ignore = '%ls'; "),
            CHANGE_CASE_TABLE,
            q_str(word).c_str());

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetAllChgCase(StringArray& target, bool clear_target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (clear_target)
        target.clear();

    int nRet;

    m_ProtectingLibrary.lock();
    
        String query;
        query.Format(
            _T(" SELECT case_ignore ")
            _T(" FROM %ls ")
            _T(" WHERE case_ignore<> ''; "),
            CHANGE_CASE_TABLE);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddRowToStringArray, 
            &target, 
            NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::AddPath(const String& path)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();
    {
        String query;
        query.Format(
            _T(" INSERT INTO %ls ")
            _T(" VALUES ('%ls'); "),
            MANAGED_PATH,
            q_str(path).c_str());

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);
    }
    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::RemovePath(const String& path)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" DELETE FROM %ls ")
            _T(" WHERE path_name = '%ls'; "),
            MANAGED_PATH,
            q_str(path).c_str());
    
        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

int Library::GetAllPaths(StringArray& target, bool clear_target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    if (clear_target)
        target.clear();

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" SELECT path_name ")
            _T(" FROM %ls ")
            _T(" WHERE path_name <> ''; "),
            MANAGED_PATH);
    
        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddRowToStringArray, &target, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

void Library::ReloadTags(Song& song)
{
	SongInfo info(this);
    info.SetID(song.GetID());
    info.Requery();    
    info.LoadInfo(info.GetFilename());

    info.SetDirtyFlag(_T("0"));
    SetSongInfo(info);
}

///////////////////////////////////////////////////

void Library::ClearLibrary(bool clear_all_tables)
{
	m_ProtectingLibrary.lock();
        sqlite3_exec(m_pDB, "delete from songs;", NULL, NULL, NULL);

        if (clear_all_tables)
        {
            sqlite3_exec(m_pDB, "delete from std_playlist;", NULL, NULL, NULL);
            sqlite3_exec(m_pDB, "delete from std_playlist_song;", NULL, NULL, NULL);
            sqlite3_exec(m_pDB, "delete from dyn_playlist;", NULL, NULL, NULL);
            sqlite3_exec(m_pDB, "delete from crossfader_preset;", NULL, NULL, NULL);
            sqlite3_exec(m_pDB, "delete from equalizer_preset;", NULL, NULL, NULL);
            sqlite3_exec(m_pDB, "delete from equalizer_default;", NULL, NULL, NULL);
            sqlite3_exec(m_pDB, "delete from managed_path;", NULL, NULL, NULL);
        }
    m_ProtectingLibrary.unlock();
    
    if (clear_all_tables)
        InitDefaultDynPlaylists();
}

///////////////////////////////////////////////////

int Library::UpdateTimesPlayed(SongInfo& song)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

    String query;
    query.Format(
        _T(" UPDATE %ls ") 
        _T(" SET timesplayed = (timesplayed + 1) ")
        _T(" WHERE songid = %d;"),
        SONG_TABLE_NAME,
        _T("julianday('now')"),
        song.GetID());

    nRet = sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;

    return -1;
}

///////////////////////////////////////////////////

int Library::UpdateLastPlayed(SongInfo& song)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

    String query;
    query.Format(
        _T(" UPDATE %ls ") 
        _T(" SET lastplayed = %ls ")
        _T(" WHERE songid = %d;"),
        SONG_TABLE_NAME,
        _T("julianday('now')"),
        song.GetID());

    nRet = sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;

    return -1;
}


///////////////////////////////////////////////////

int Library::UpdatePlayTime(SongInfo& song)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    int nRet;

    m_ProtectingLibrary.lock();

        String query;
        query.Format(
            _T(" UPDATE %ls ") 
            _T(" SET totplaytime = (totplaytime + %d), %ls ")
            _T(" WHERE songid = %d;"),
            SONG_TABLE_NAME,
            song.m_PlayTime,
            song.m_UserChoose == 1 ?
                _T("userplayed = (userplayed + 1)") :
                _T("autoadvanced = (autoadvanced + 1)"),
            song.GetID());
    
        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();

    return nRet;

    return -1;
}

///////////////////////////////////////////////////

int Library::SortPlaylist(Playlist* playlist, int field, bool descending)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    String sQuery;

    sQuery = _T("SELECT songid,duration,filesize FROM songs where songid in (");

    String sCurr;
    for (size_t i = 0; i < playlist->GetCount(); i++)
    {
        if (i < playlist->GetCount() - 1)
            sCurr.Format(_T("%d,"), playlist->GetSongID(i));
        else
            sCurr.Format(_T("%d"), playlist->GetSongID(i));

        sQuery += sCurr;
    }
    sQuery += _T(")");

    switch (field)
    {
    case MUSIK_LIBRARY_TYPE_FORMAT:
    case MUSIK_LIBRARY_TYPE_TRACKNUM:
    case MUSIK_LIBRARY_TYPE_YEAR:
    case MUSIK_LIBRARY_TYPE_RATING:
    case MUSIK_LIBRARY_TYPE_BITRATE:
    case MUSIK_LIBRARY_TYPE_FILESIZE:
        sCurr = Library::GetOrder(field, false, true, descending);
        break;

    default:
        sCurr = Library::GetOrder(field, false, true, descending);
        break;
    }

    sQuery += sCurr;
    sQuery += _T(";");

    playlist->Clear();

    return RawQuerySongs(sQuery, *playlist);
}

///////////////////////////////////////////////////

void Library::DeleteSong(String& fn)
{
	m_ProtectingLibrary.lock();

    String realfn = fn;
#if defined(WIN32) || defined(__MINGW32__)
    realfn.ToLower();
#endif

    String query;
    query.Format(       
        _T(" DELETE FROM %ls ")
        _T(" WHERE filename like '%ls'; "),
        SONG_TABLE_NAME,
        q_str(realfn).c_str());

    sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();
}

///////////////////////////////////////////////////

void Library::DeleteSongs(Playlist& songs)
{
	String filename;
    std::stack<Library*> libs;
    Library* curr;

    for (size_t i = 0; i < songs.GetCount(); i++)
    {
        curr = songs.at(i).GetLibrary();
        if (!curr->TransactionOpen())
        {
            curr->BeginTransaction();
            libs.push(curr);
        }

        filename = songs.at(i).GetField(MUSIK_LIBRARY_TYPE_FILENAME);
        curr->DeleteSong(filename);
    }

    // kill open transactions
    while (libs.size())
    {
        curr = libs.top();
        libs.pop();

        if (curr->TransactionOpen())
        {
            curr->EndTransaction();
        }
    }

}

///////////////////////////////////////////////////

int Library::GetAllFns(String& items)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;    

    items.clear();

    String query = 
    _T(" SELECT DISTINCT filename FROM songs; ");
    m_ProtectingLibrary.lock();
        int ret = sqlite3_exec(
            m_pDB, utf16to8(query).c_str(),
            LibraryCallbacks::AppendFileStringFromRow, &items, NULL);
    m_ProtectingLibrary.unlock();
    items += _T("\0");

    return ret;
}

///////////////////////////////////////////////////

void Library::QueueTask(Task* ptrTask)
{
	m_Tasks.Push(ptrTask);
}

///////////////////////////////////////////////////

void Library::FlushTaskQueue()
{
	m_Tasks.Flush();
}

///////////////////////////////////////////////////

int Library::Rand(int min, int max)
{
#if 0
    return (int)((double)rand() / ((double)RAND_MAX + 1) * (max - min)) + min;
#endif

    unsigned long randnum;
    sqlite3Randomness((int)(sizeof(long)), &randnum);

    int result = (int)(((double)randnum / (double)0xFFFFFFFF) * (max - min)) + min;
    
    return result;
}

///////////////////////////////////////////////////

const String Library::GetRandom(int field, size_t count)
{
	StringArray target;
    GetAllDistinct(field, target, true);

    if (target.size() < count)
        count = target.size();

    int rnd;
    String ret;
    for (size_t i = 0; i < count; i++)
    {    
        rnd = Rand(0, target.size() - 1);
        if (rnd > (int)target.size() - 1)
        {
            cout << "musikCore::Library::GetRandom() failed\n";
            count--;
            continue;
        }

        if (i == 0)
            ret += target.at(rnd);
        else
        {
            ret += ", ";
            ret += target.at(rnd);
        }
    }

    return ret;
}

///////////////////////////////////////////////////

int Library::GetNextArtist(String& artist_name, Playlist& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    String query;
    target.clear();

    int ret;

    artist_name.Replace(_T("'"), _T("''"));

    query.Format(
            _T(" SELECT songid, duration, filesize")
            _T(" FROM %ls" )
            _T(" WHERE LOWER(artist) = (")
            _T("    SELECT DISTINCT LOWER(artist) as 'artist'")
            _T("    FROM songs ")
            _T("    WHERE LOWER(artist) > LOWER('%ls')")
            _T("    ORDER BY LOWER(artist) LIMIT 1")
            _T(")")
            _T(" ORDER BY album, tracknum;"),
            SONG_TABLE_NAME,
            q_str(artist_name).c_str());

    m_ProtectingLibrary.lock();
        ret = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddSongToPlaylist, 
            &target, 
            NULL);
    m_ProtectingLibrary.unlock();   

    if (!target.size())
    {
        query.Format(
            _T(" SELECT songid, duration, filesize")
            _T(" FROM %ls")
            _T(" WHERE LOWER(artist) = (")
            _T("    SELECT DISTINCT LOWER(artist) as 'artist'")
            _T("    FROM songs ")
            _T("    ORDER BY LOWER(artist) LIMIT 1")
            _T("   )")
            _T(" ORDER BY album, tracknum;"),
            SONG_TABLE_NAME);
        
        m_ProtectingLibrary.lock();
            ret = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                &LibraryCallbacks::AddSongToPlaylist, 
                &target, 
                NULL);
        m_ProtectingLibrary.unlock();   
    }

    Claim(target);

    return ret;
}

///////////////////////////////////////////////////

int Library::GetPrevArtist(String& artist_name, Playlist& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    String query;
    target.clear();

    artist_name.Replace(_T("'"), _T("''"));

    int ret;

    query.Format(
        _T(" SELECT songid, duration, filesize")
        _T(" FROM %ls")
        _T(" WHERE LOWER(artist) = (")
        _T("    SELECT DISTINCT LOWER(artist) as 'artist'")
        _T("    FROM songs ")
        _T("    WHERE LOWER(artist) < LOWER('%ls')")
        _T("    ORDER BY LOWER(artist) DESC LIMIT 1")
        _T("   )")
        _T(" ORDER BY album, tracknum;"),
        SONG_TABLE_NAME,
        q_str(artist_name).c_str());

    m_ProtectingLibrary.lock();
        ret = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddSongToPlaylist, 
            &target, 
            NULL);
    m_ProtectingLibrary.unlock();

    if (!target.size())
    {
        query.Format(
            _T(" SELECT songid, duration, filesize")
            _T(" FROM %ls")
            _T(" WHERE LOWER(artist) = (")
            _T("    SELECT DISTINCT LOWER(artist) as 'artist'")
            _T("    FROM songs ")
            _T("    ORDER BY LOWER(artist) DESC LIMIT 1")
            _T("   )")
            _T(" ORDER BY album, tracknum;"),
            SONG_TABLE_NAME);

        m_ProtectingLibrary.lock();
            ret = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                &LibraryCallbacks::AddSongToPlaylist, 
                &target, 
                NULL);
        m_ProtectingLibrary.unlock();   
    }

    Claim(target);

    return ret;
}

///////////////////////////////////////////////////

int Library::GetNextAlbum(String& artist_name, String& album_name, Playlist& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    String query;
    target.clear();

    artist_name.Replace(_T("'"), _T("''"));
    album_name.Replace(_T("'"), _T("''"));

    int ret;

    query.Format(
            _T(" SELECT songid, duration, filesize ")
            _T(" FROM %ls ")
            _T(" WHERE LOWER(album) = (")
            _T("       SELECT DISTINCT LOWER(album) as 'album' ")
            _T("       FROM %ls ")
            _T("       WHERE ")
            _T("           LOWER(artist) = LOWER('%ls') AND ") 
            _T("           LOWER(album) > LOWER('%ls')")
            _T("       LIMIT 1")
            _T("      )")
            _T(" ORDER BY album, tracknum;"),
            SONG_TABLE_NAME,
            SONG_TABLE_NAME,
            q_str(artist_name).c_str(),
            q_str(album_name).c_str());

    m_ProtectingLibrary.lock();
        ret = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddSongToPlaylist, 
            &target, 
            NULL);
    m_ProtectingLibrary.unlock();   

    if (!target.size())
    {
        query.Format(
            _T(" SELECT songid, duration, filesize")
            _T(" FROM %ls")
            _T(" WHERE LOWER(album) = (")
            _T("    SELECT DISTINCT LOWER(album) as 'album'")
            _T("    FROM %ls ")
            _T("    WHERE LOWER(artist) = LOWER('%ls')")
            _T("    ORDER BY album ASC")
            _T("    LIMIT 1")
            _T("   )")
            _T(" ORDER BY album, tracknum;"),
            SONG_TABLE_NAME,
            SONG_TABLE_NAME,
            q_str(artist_name).c_str());
        
        m_ProtectingLibrary.lock();
            ret = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                &LibraryCallbacks::AddSongToPlaylist, 
                &target, 
                NULL);
        m_ProtectingLibrary.unlock();   
    }

    Claim(target);

    return ret;
}

///////////////////////////////////////////////////

int Library::GetPrevAlbum(String& artist_name, String& album_name, Playlist& target)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    String query;
    target.clear();

    artist_name.Replace(_T("'"), _T("''"));
    album_name.Replace(_T("'"), _T("''"));

    int ret;

    query.Format(
            _T(" SELECT songid, duration, filesize ")
            _T(" FROM %ls ")
            _T(" WHERE LOWER(album) = (")
            _T("       SELECT DISTINCT LOWER(album) as 'album' ")
            _T("       FROM %ls ")
            _T("       WHERE ")
            _T("           LOWER(artist) = LOWER('%ls') AND ")
            _T("           LOWER(album) < LOWER('%ls')")
            _T("       LIMIT 1")
            _T("      )")
            _T(" ORDER BY album, tracknum;"),
            SONG_TABLE_NAME,
            SONG_TABLE_NAME,
            q_str(artist_name).c_str(),
            q_str(album_name).c_str());

    m_ProtectingLibrary.lock();
        ret = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(),
            &LibraryCallbacks::AddSongToPlaylist, 
            &target, 
            NULL);
    m_ProtectingLibrary.unlock();

    if (!target.size())
    {
        query.Format(
            _T(" SELECT songid, duration, filesize")
            _T(" FROM %ls")
            _T(" WHERE LOWER(album) = (")
            _T("    SELECT DISTINCT LOWER(album) as 'album' ")
            _T("    FROM %ls ")
            _T("    WHERE LOWER(artist) = LOWER('%ls')")
            _T("    ORDER BY album DESC")
            _T("    LIMIT 1")
            _T("   )")
            _T(" ORDER BY album, tracknum;"),
            SONG_TABLE_NAME,
            SONG_TABLE_NAME,
            q_str(artist_name).c_str());
        
        m_ProtectingLibrary.lock();
            ret = sqlite3_exec(
                m_pDB, 
                utf16to8(query).c_str(),
                &LibraryCallbacks::AddSongToPlaylist, 
                &target, 
                NULL);
        m_ProtectingLibrary.unlock();
    }

    Claim(target);

    return ret;
}

///////////////////////////////////////////////////

void Library::GetRandom(int field, Playlist& target, bool clear_target, size_t count)
{
	StringArray all_items;
    GetAllDistinct(field, all_items, true);

    if (all_items.size() < count)
        count = all_items.size();

    int rnd;
    String ret = "WHERE ";
    String strfield = GetSongFieldDBStr(field);

    for (size_t i = 0; i < count; i++)
    {    
        rnd = Rand(0, all_items.size() - 1);
        if (rnd > (int)all_items.size() - 1)
        {
            cout << _T("musikCore::Library::GetRandom() failed\n");
            count--;
            continue;
        }

        all_items.at(rnd).Replace(_T("'"), _T("''"));

        if (i == 0)
        {
            ret += strfield;
            ret += _T(" LIKE ");
            ret += _T("'");
            ret += all_items.at(rnd);
            ret += _T("'");
        }
        else
        {
            ret += _T(" OR ");
            ret += strfield;
            ret += _T(" LIKE ");
            ret += _T("'");
            ret += all_items.at(rnd);
            ret += _T("'");
        }
    }

    GetRelatedSongs(ret, field, target, false, clear_target);
}

///////////////////////////////////////////////////

int Library::GetTaskType()
{
	if (m_Tasks.size())
    {
        return m_Tasks.front()->GetType();
    }

    return -1;
}

///////////////////////////////////////////////////

int Library::RemoveFromStdPlaylist(const SongArray& items, int playlistid)
{
	if (!m_DatabaseOpen)
        return MUSIK_LIBRARY_NOT_OPEN;

    musikCore::String in_stmt, temp;

    in_stmt.Format(
        _T(" DELETE FROM %ls ")
        _T(" WHERE std_playlist_id = %d ")
        _T(" AND songid IN ("),
        STD_PLAYLIST_SONGS,
        playlistid);
    for (size_t i = 0; i < items.size(); i++)
    {
        in_stmt += IntToString(items.at(i).GetID());
        if (i < items.size() - 1)
            in_stmt += _T(",");
    }
    in_stmt += _T("); ");

    m_ProtectingLibrary.lock();
        int ret = sqlite3_exec(
            m_pDB, 
            utf16to8(in_stmt).c_str(),
            NULL, NULL, NULL);
    m_ProtectingLibrary.unlock();   

    return ret;
}

///////////////////////////////////////////////////

// musikCore::RemLibrary

///////////////////////////////////////////////////

static int sqlite_RemLibFnUpdate(void *args, int numCols, char **results, char ** columnNames)
{
	SongInfo *pLibItem = new SongInfo();

    pLibItem->SetID(atoi(results[0])); 
    pLibItem->SetFilename(results[1]);

    SongInfoArray* p = (SongInfoArray*)args;
    p->push_back(*pLibItem);

    delete pLibItem;
    return 0;
}

///////////////////////////////////////////////////

RemLibrary::RemLibrary(const String& filename)
: Library(filename)
{
	InitRemPathTable();
}

///////////////////////////////////////////////////

String RemLibrary::GetRemPath()
{
	if (!m_DatabaseOpen)
    {
        return _T("");
    }

    // get the last path
    String query;
    query.Format(
        _T(" SELECT path FROM %ls;"),
        MUSIK_REMLIBRARY_PATH);

    m_ProtectingLibrary.lock();

    String rem_path;
    sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        LibraryCallbacks::GetStringFromRow, 
        &rem_path, 
        NULL);

    m_ProtectingLibrary.unlock();

    return rem_path;
}

void RemLibrary::SetRemPath(const String& path, bool update_all_songs)
{
	if (!m_DatabaseOpen)
    {
        return;
    }

    String last_path = GetRemPath();
    String query;

    // update all songs
    if (update_all_songs && last_path != path)
    {
        // get all the songs, the callback will update the path
        SongInfoArray target;

        query.Format(_T(" SELECT songid,filename FROM songs; "));

        m_ProtectingLibrary.lock();

        sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            sqlite_RemLibFnUpdate, 
            &target, 
            NULL 
       );

        m_ProtectingLibrary.unlock();

        // write the songs back to the database
        BeginTransaction();
        m_ProtectingLibrary.lock();

#if defined (WIN32)
        last_path.ToLower();
#endif

        for (size_t i = 0; i < target.size(); i++)
        {
            String strFn = target[i].GetFilename();
            if (strFn.Replace(last_path, path))
            {
                query.Format(
                    _T(" UPDATE songs ")
                    _T(" SET filename = '%ls' ")
                    _T(" WHERE songid = %d; "),
                    q_str(strFn).c_str(),
                    target.at(i).GetID());

                    sqlite3_exec(
                        m_pDB, 
                        utf16to8(query).c_str(), 
                        NULL, NULL, NULL);
            }
        }
        m_ProtectingLibrary.unlock();
        EndTransaction();
    }

    m_ProtectingLibrary.lock();

    // set the new path
    query.Format(
        _T(" DELETE FROM %ls; ")
        _T(" INSERT INTO %ls ")
        _T(" VALUES ('%ls');"),
        MANAGED_PATH,
        MANAGED_PATH,
        q_str(path).c_str());

    sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        NULL, NULL, NULL);

        // set the new path
    query.Format(
        _T(" DELETE FROM %ls; ")
        _T(" INSERT INTO %ls ")
        _T(" VALUES ('%ls');"),
        MUSIK_REMLIBRARY_PATH,
        MUSIK_REMLIBRARY_PATH,
        q_str(path).c_str());

    sqlite3_exec(
        m_pDB, 
        utf16to8(query).c_str(), 
        NULL, NULL, NULL);

    m_ProtectingLibrary.unlock();
}

///////////////////////////////////////////////////

bool RemLibrary::InitRemPathTable()
{
	if (!m_DatabaseOpen)
        return false;

    bool error = false;

    // construct the table that contains a list of
    // all the standard playlist names
    String szCreateDBQuery;
    szCreateDBQuery.Format(
        _T("CREATE TABLE %ls (")
        _T("path varchar(1024) PRIMARY KEY);"),
        MUSIK_REMLIBRARY_PATH);

    // put a lock on the library and open it up
    char *pErr = NULL;

    m_ProtectingLibrary.lock();

    sqlite3_exec(
        m_pDB, 
        utf16to8(szCreateDBQuery).c_str(), 
        NULL, NULL, &pErr);

    m_ProtectingLibrary.unlock();

    if (pErr)
    {
        error = true;
        sqlite3_free(pErr);
    }

    return error;
}

///////////////////////////////////////////////////

int Library::UpdatePlaylistOrder(const PlaylistInfo& target)
{
	if (!m_DatabaseOpen)
        return -1;

    int nRet = -1;
    m_ProtectingLibrary.lock();
    if (target.GetType() == MUSIK_PLAYLIST_TYPE_STANDARD)
    {
        String query;
        query.Format(
            _T(" UPDATE std_playlist ")
            _T(" SET std_playlist_order = %d ")
            _T(" WHERE std_playlist_id = %d; "),
            target.m_Order,
            target.m_ID);
    
        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);
    }
    else if (target.GetType() == MUSIK_PLAYLIST_TYPE_DYNAMIC)
    {
        String query;
        query.Format(
            _T(" UPDATE dyn_playlist ")
            _T(" SET dyn_playlist_order = %d ")
            _T(" WHERE dyn_playlist_id = %d; "),
            target.m_Order,
            target.m_ID);

        nRet = sqlite3_exec(
            m_pDB, 
            utf16to8(query).c_str(), 
            NULL, NULL, NULL);
    }
    m_ProtectingLibrary.unlock();

    return nRet;
}

///////////////////////////////////////////////////

void Library::Claim(Playlist& playlist)
{
	for (size_t i = 0; i < playlist.size(); i++)
        playlist.at(i).SetLibrary(this);
}

///////////////////////////////////////////////////

void Library::Claim(SongInfoArray& songs)
{
	for (size_t i = 0; i < songs.size(); i++)
        songs.at(i).SetLibrary(this);
}

///////////////////////////////////////////////////

int Library::GetLastSong( int songid, Library *lib )
{
	String query;
	query.Format(
		_T( " SELECT DISTINCT julianday( lastplayed ) " )
		_T( " FROM %ls " )
		_T( " WHERE lastplayed > 0 " )
		_T( " AND songid = %d " )
		_T( " LIMIT 1 " ),
		SONG_TABLE_NAME,
		songid);

	String lastplayed;
	sqlite3_exec(
		lib->m_pDB,
		utf16to8( query ).c_str(),
		&LibraryCallbacks::GetStringFromRow,
		&lastplayed,
		NULL);

	query.clear();
	query.Format(
		_T( " SELECT DISTINCT songid " )
		_T( " FROM %ls " )
		_T( " WHERE julianday( lastplayed ) < %ls " )
		_T( " AND julianday( lastplayed ) > 0 " )
		_T( " ORDER BY julianday( lastplayed ) DESC " )
		_T( " LIMIT 1 " ),
		SONG_TABLE_NAME,
		lastplayed.c_str());

	int newid;
	sqlite3_exec(
		lib->m_pDB,
		utf16to8( query ).c_str(),
		&LibraryCallbacks::GetIntFromRow,
		&newid,
		NULL);

	query.clear();
	query.Format(
		_T( " UPDATE %ls " )
		_T( " SET lastplayed=0 " )
		_T( " WHERE lastplayed > %ls " ),
		SONG_TABLE_NAME,
		lastplayed.c_str());

	sqlite3_exec(
		lib->m_pDB,
		utf16to8( query ).c_str(),
		NULL,
		NULL,
		NULL);

	return newid;
}



