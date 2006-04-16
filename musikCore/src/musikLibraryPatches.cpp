///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://www.musikcube.com
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2002-2004
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey at musikcube dot com
//   Dustin Carter  : Developer
//   Simon Windmill : Developer
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikLibrary.h"
#include "../include/musikLibraryCallbacks.h"
#include <stack>

using namespace musikCore;

///////////////////////////////////////////////////

void Library::PatchLibrary()
{
	int prev_version = -1;

	m_ProtectingLibrary.lock();

	sqlite3_exec(
		m_pDB,
		"SELECT version FROM library_version;",
		sqlite_GetIntFromRow,
		&prev_version,
		NULL );
	m_ProtectingLibrary.unlock();

	if ( prev_version < MUSIK_LIBRARY_VERSION )
	{
		// 
		// prior to version 1, no stock dynamic playlists
		// existed, so create them....
		// 
		if ( prev_version < 1 )
		{
			InitDefaultDynPlaylists();

			// set netstream durations to 0
			m_ProtectingLibrary.lock();
			sqlite3_exec(
				m_pDB,
				"UPDATE songs SET duration = 0 WHERE format = 4;",
				NULL, NULL, NULL );
			m_ProtectingLibrary.unlock();
		}

		//
		// version 2 removed sub libraries and converted
		// them to standard playlists
		//
		if ( prev_version < 2 )
		{
			m_ProtectingLibrary.lock();
			sqlite3_exec(
				m_pDB,
				"UPDATE std_playlist SET std_playlist_type = 1;",
				NULL, NULL, NULL );
			m_ProtectingLibrary.unlock();
		}

		// version 3 skipped

		//
		// temp song table is also changed to only contain song
		// ids... this makes things much faster
		//
		if ( prev_version < 4 )
		{
			// drop old temp song table, it will be re-created
			// after this function returns.
			m_ProtectingLibrary.lock();
			sqlite3_exec(
				m_pDB,
				"DROP TABLE temp_songs;",
				NULL, NULL, NULL );
			m_ProtectingLibrary.unlock();

			// convert std playlists
			PlaylistInfoArray plinfo;
			vector<Playlist> playlists;
			GetAllStdPlaylists( plinfo );
			for ( size_t i = 0; i < plinfo.size(); i++ )
			{
				Playlist pl;
				playlists.push_back( pl );
				Patch4GetStdPlaylist( plinfo.at( i ).GetID(), playlists.at( i ) );
			}

			// drop old playlist song table
			m_ProtectingLibrary.lock();
			sqlite3_exec(
				m_pDB,
				"DROP TABLE std_playlist_song;",
				NULL, NULL, NULL );
			m_ProtectingLibrary.unlock();

			// create new playlist song table
			InitStdTables();

			// add the songs again
			for ( size_t i = 0; i < plinfo.size(); i++ )
			{
				playlists.at( i ).SetID( plinfo.at( i ).GetID() );
				RewriteStdPlaylist( playlists.at( i ) );
			}
		}

		//
		// add a playlist column "order" to dynamic and standard playlist. it also
		// removes a column "type". 
		//

		if ( prev_version < 5 )
		{
			// convert std playlists
			PlaylistInfoArray plinfo;
			vector<Playlist> playlists;
			Patch5GetAllStdPlaylists( plinfo );
			for ( size_t i = 0; i < plinfo.size(); i++ )
			{
				Playlist pl;
				playlists.push_back( pl );
				GetStdPlaylist( plinfo.at( i ).GetID(), playlists.at( i ) );
				plinfo.at( i ).m_Order = i;	// correct order
			}

			// drop old std playlist tables.
			m_ProtectingLibrary.lock();
			sqlite3_exec(
				m_pDB,
				"DROP TABLE std_playlist_song; DROP TABLE std_playlist;",
				NULL, NULL, NULL );
			m_ProtectingLibrary.unlock();

			// create new playlist song table
			InitStdTables();

			// add the songs again
			for ( size_t i = 0; i < plinfo.size(); i++ )
			{
				playlists.at( i ).SetID( plinfo.at( i ).GetID() );
				CreateStdPlaylist( plinfo.at( i ).m_Name, playlists.at( i ), plinfo.at( i ).m_Order );
			}

			// now we need to do dynamic playlists
			vector<String> queries;
			String temp;
			Patch5GetAllDynPlaylists( plinfo );
			for ( size_t i = 0; i < plinfo.size(); i++ )
			{
				String query;
				GetDynPlaylistInfo( plinfo.at( i ).m_ID, temp, query );
				queries.push_back( query );
			}

			m_ProtectingLibrary.lock();
			sqlite3_exec(
				m_pDB,
				"DROP TABLE dyn_playlist;",
				NULL, NULL, NULL );
			m_ProtectingLibrary.unlock();

			InitDynTable( false );

			for ( size_t i = 0; i < plinfo.size(); i++ )
				CreateDynPlaylist( plinfo.at( i ).m_Name, queries.at( i ), i );
		}
	}

	//
	// assures all filenames are stored lowercase 
	//

	if ( prev_version < 6 )
	{
#if defined (WIN32) || defined(__MINGW32__)
		m_ProtectingLibrary.lock();
		sqlite3_exec(
			m_pDB,
			"UPDATE songs SET filename = LOWER(filename);",
			NULL, NULL, NULL );
		m_ProtectingLibrary.unlock();
#endif
	}

	//
	// makes the "format" column a string instead of an int.
	// this is so plugins can register their format easily
	//

	if ( prev_version < 7 )
	{
		musikCore::Playlist allsongids;
		GetAllSongs( allsongids );

		musikCore::SongInfoArray allsonginfo;
		GetInfoArrayFromPlaylist( allsongids, allsonginfo );

		for ( size_t i = 0; i < allsonginfo.size(); i++ )
		{
			SongInfo& curr = allsonginfo.at( i );
		
			if ( curr.GetFormat() == _T( "0" ) )
				curr.SetFormat( _T( "mp3" ) );
			else if ( curr.GetFormat() == _T( "1" ) )
				curr.SetFormat( _T( "ogg" ) );
			else if ( curr.GetFormat() == _T( "2" ) )
				curr.SetFormat( _T( "flac" ) );
			else if ( curr.GetFormat() == _T( "4" ) )
				curr.SetFormat( _T( "net" ) );
			else 
				curr.SetFormat( _T( "mp3" ) );
		}

		m_ProtectingLibrary.lock();
		sqlite3_exec(
			m_pDB,
			"DROP TABLE songs;",
			NULL, NULL, NULL );
		m_ProtectingLibrary.unlock();

		InitLibTable();

		BeginTransaction();
		for ( size_t i = 0; i < allsonginfo.size(); i++ )
			AddFile( allsonginfo.at( i ) );
		EndTransaction();
	}

	//
	// fixes standard and dynamic playlist ordering
	//

	if ( prev_version < 8 )
	{
		PlaylistInfoArray target;

		GetAllStdPlaylists( target );
		for ( size_t i = 0; i < target.size(); i++ )
		{
			target.at( i ).m_Order = i;
			UpdatePlaylistOrder( target.at( i ) );
		}

		GetAllDynPlaylists( target );
		for ( size_t i = 0; i < target.size(); i++ )
		{
			target.at( i ).m_Order = i;
			UpdatePlaylistOrder( target.at( i ) );
		}
	}

	//
	// WIN32 only... adds an extra column in the database to store a 
	// filename with preserved case. done for query speeds. 
	//
	// ended up REMOVING this because i thought it was too much of
	// a gross hack. 
	//

	if ( prev_version < 9 )
	{
		;
	}

	// save new version
	m_ProtectingLibrary.lock();
	musikCore::String query;
	query.Format(
		_T( "UPDATE library_version SET version = %d;" ),
		MUSIK_LIBRARY_VERSION );
	sqlite3_exec(
		m_pDB,
		utf16to8( query ).c_str(),
		NULL, NULL, NULL );
	m_ProtectingLibrary.unlock();
}

///////////////////////////////////////////////////

static int sqlite_Patch5AddStdPlaylistInfoArray( void *args, int numCols, char **results, char ** columnNames )
{
	PlaylistInfoArray* p = (PlaylistInfoArray*)args;
	PlaylistInfo info ( utf8to16( results[0] ), MUSIK_PLAYLIST_TYPE_STANDARD, atoi( results[1] ) );
	p->push_back( info ); 
	return 0;
}

///////////////////////////////////////////////////

int Library::Patch5GetAllStdPlaylists( PlaylistInfoArray& target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( clear_target )
		target.clear();

	// do it
	int nRet;
	m_ProtectingLibrary.lock();

	String query;

	query.Format( 
		_T( " SELECT std_playlist_name,std_playlist_id " )
		_T( " FROM %ls; " ),
		STD_PLAYLIST_TABLE_NAME );

	nRet = sqlite3_exec( 
		m_pDB, 
		utf16to8( query ).c_str(), 
		&sqlite_Patch5AddStdPlaylistInfoArray, 
		&target, 
		NULL );

	m_ProtectingLibrary.unlock();

	return nRet;
}


///////////////////////////////////////////////////

static int sqlite_Patch5AddDynPlaylistInfoArray( void *args, int numCols, char **results, char ** columnNames )
{
	PlaylistInfoArray* p = (PlaylistInfoArray*)args;

	PlaylistInfo info ( utf8to16( results[0] ), MUSIK_PLAYLIST_TYPE_DYNAMIC, atoi( results[1] ) );
	//info.m_Order = atoi( results[2] );
	p->push_back( info ); 

	return 0;
}

///////////////////////////////////////////////////

int Library::Patch5GetAllDynPlaylists( PlaylistInfoArray& target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( clear_target )
		target.clear();

	// do it
	int nRet;
	m_ProtectingLibrary.lock();

	String query;
	query.Format(
		_T( " SELECT dyn_playlist_name,dyn_playlist_id " )
		_T( " FROM %ls " )
		_T( " WHERE dyn_playlist_name <> ''; " ),
		DYN_PLAYLIST_TABLE_NAME );

	nRet = sqlite3_exec( 
		m_pDB, 
		utf16to8( query ).c_str(),
		&sqlite_Patch5AddDynPlaylistInfoArray, 
		&target, 
		NULL );

	m_ProtectingLibrary.unlock();

	for ( size_t i = 0; i < target.size(); i++ )
		target.at( i ).m_Order = i;

	return nRet;
}

///////////////////////////////////////////////////

int Library::Patch4GetStdPlaylist( int id, Playlist& target, bool clear_target )
{
	if ( !m_DatabaseOpen )
		return MUSIK_LIBRARY_NOT_OPEN;

	if ( clear_target )
		target.Clear();

	StringArray* items = new StringArray;
	int playlist_type = MUSIK_PLAYLIST_TYPE_UNKNOWN;

	int nRet1, nRet2;
	m_ProtectingLibrary.lock();

	String query;

	// get the type

	query.Format( 
		_T( " SELECT std_playlist_type " )
		_T( " FROM %ls " )
		_T( " WHERE std_playlist_id = %d " ),
		STD_PLAYLIST_TABLE_NAME,
		id );

	nRet2 = sqlite3_exec( 
		m_pDB, 
		utf16to8( query ).c_str(), 
		&sqlite_GetIntFromRow, &playlist_type, NULL );

	query.Format(
		_T( " SELECT DISTINCT songfn " )
		_T( " FROM %ls " )
		_T( " WHERE std_playlist_id = %d; " ),
		STD_PLAYLIST_SONGS,
		id );

	nRet1 = sqlite3_exec( 
		m_pDB, 
		utf16to8( query ).c_str(), 
		&sqlite_AddSongToStringArray, 
		items, 
		NULL );

	m_ProtectingLibrary.unlock();	

	// add all the items to the playlist
	Song song;
	for ( size_t i = 0; i < items->size(); i++ )
	{
		GetSongAttrFromFilename( items->at( i ), song );
		target.Add( song );
	}

	// assure correct type and id are set
	target.SetType( playlist_type );
	target.SetID( id );

	delete items;

	if ( !nRet1 )
		return nRet2;

	return nRet1;
}

///////////////////////////////////////////////////




