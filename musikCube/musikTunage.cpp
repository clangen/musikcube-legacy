///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2002-2004
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey at musikcube dot com
//   Dustin Carter  : Developer
//   Simon Windmill : Developer
//	 Rajiv Makhijani: Website Manager, Added $RATING and $GENRE
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikTunage
//
// Filename(s): 
//
//   musikTunage.h, musikTunage.cpp
//
// Information:
//
//   Does something related to current song info, such as post to a website,
//   write to a text file, etc.
// 
// Usage: 
//
//	 Called when a song changes or is stopped
//
///////////////////////////////////////////////////

#include "StdAfx.h"

#include <afxinet.h>

#include "musikCube.h"
#include "musikTunage.h"

///////////////////////////////////////////////////

UINT TunageWorker( LPVOID pParam )
{
	CInternetSession m_Session;

	if ( musikCube::g_Prefs->GetTunageURL() != "" ) 
	{
		musikCore::String url( musikCube::g_Prefs->GetTunageURL() );
		CmusikTunage::ParseTags( url, true );

		try
		{
			delete m_Session.OpenURL( url );
		}
		catch (...)
		{
			
		}

		m_Session.Close();
	}

	return (UINT)1;
}

///////////////////////////////////////////////////

CmusikTunage::CmusikTunage()
{
}

///////////////////////////////////////////////////

CmusikTunage::~CmusikTunage(void)
{
}	

///////////////////////////////////////////////////

void CmusikTunage::Execute()
{
	if ( musikCube::g_Prefs->GetTunagePostURL() )
		Execute_PostURL();

	if ( musikCube::g_Prefs->GetTunageRunApp() )
		Execute_RunApp();

	if ( musikCube::g_Prefs->GetTunageWriteFile() )
		Execute_WriteFile();
}

///////////////////////////////////////////////////

void CmusikTunage::Execute_PostURL()
{	
	AfxBeginThread( ::TunageWorker, (LPVOID)NULL );
}

///////////////////////////////////////////////////

void CmusikTunage::Execute_RunApp()
{
	// bail if empty
	if ( musikCube::g_Prefs->GetTunageCmdLine() == "" )
		return;

	musikCore::String str( musikCube::g_Prefs->GetTunageCmdLine() );
	ParseTags( str, false );

    WinExec( musikCore::utf16to8( str ).c_str(), SW_NORMAL );
}

///////////////////////////////////////////////////

void CmusikTunage::Execute_WriteFile()
{
	// bail if empty
	if ( ( musikCube::g_Prefs->GetTunageFilename() == "" ) || ( musikCube::g_Prefs->GetTunageFileLine() == "" ) )
		return;

	if ( !musikCube::g_Prefs->GetTunageAppendFile() )
	{
		if (  musikCore::Filename::FileExists( musikCube::g_Prefs->GetTunageFilename() ) )
			remove( musikCube::g_Prefs->GetTunageFilename().c_str() );
	}

	CStdioFile out;
    CString fn = musikCore::utf8to16( musikCube::g_Prefs->GetTunageFilename() ).c_str();
    if ( !out.Open( fn, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText ) )
		return;


	musikCore::String str( musikCube::g_Prefs->GetTunageFileLine() );
	ParseTags( str, false );

	if ( musikCube::g_Prefs->GetTunageAppendFile() )
	{
		out.SeekToEnd();
		out.WriteString( str + "\n" );
	}
	else
		out.WriteString( str );

	out.Close();
}

///////////////////////////////////////////////////

void CmusikTunage::ParseTags( musikCore::String& str, bool encode )
{	
	musikCore::String artist = musikCube::g_Player->GetPlaying()->GetArtist();
	if ( encode )
		URLEncode( artist );
	str.Replace( _T( "$ARTIST" ), artist );

	musikCore::String playlistpos;
	playlistpos.Format( _T( "%d of %d" ), musikCube::g_Player->GetIndex(), musikCube::g_Player->GetPlaylist()->GetCount() );
	str.Replace( _T( "$PLAYLISTPOS" ), playlistpos );

	musikCore::String album = musikCube::g_Player->GetPlaying()->GetAlbum();
	if ( encode )
		URLEncode( album );
	str.Replace( _T( "$ALBUM" ), album );
	
	musikCore::String title = musikCube::g_Player->GetPlaying()->GetTitle();
	if ( encode )
		URLEncode( title );
	str.Replace( _T( "$TITLE" ), title );

	musikCore::String genre = musikCube::g_Player->GetPlaying()->GetGenre();
	if ( encode )
		URLEncode( genre );
	str.Replace( _T( "$GENRE" ), genre );

	musikCore::String name;
	name.Format( _T( "%s - %s" ), artist.c_str(), title.c_str() );
	str.Replace( _T( "$NAME" ), name );

	int time = StoI( musikCube::g_Player->GetPlaying()->GetDuration() );
	musikCore::String duration = musikCore::Player::GetTimeStr( time );
	str.Replace( _T( "$DURATION" ), duration );

	musikCore::String filename = musikCube::g_Player->GetPlaying()->GetFilename();
	if ( encode )
		URLEncode( filename );
	str.Replace( _T( "$FILENAME" ), filename );

	double filesize = StoF( musikCube::g_Player->GetPlaying()->GetFilesize().c_str() );
	musikCore::String strsize( _T( "0.0 mb" ) );
	if ( filesize < 0 )
		strsize.Format( _T( "net radio" ) );
	else if ( filesize < 1024.0 )
		strsize.Format( _T( "%.2f b" ), filesize );
	else if ( filesize < ( 1024.0 * 1024.0 ) )
		strsize.Format( _T( "%.2f kb" ), filesize / 1024.0 );
	else if ( filesize < ( 1024.0 * 1024.0 * 1024.0 ) )
		strsize.Format( _T( "%.2f mb" ), filesize / 1024.0 / 1024.0 );
	else if ( filesize < ( 1024.0 * 1024.0 * 1024.0 * 1024.0 ) )
		strsize.Format( _T( "%.2f gb" ), filesize / 1024.0 / 1024.0 / 1024.0 );

	str.Replace( _T( "$FILESIZE" ), strsize );
	str.Replace( _T( "$BITRATE" ), musikCube::g_Player->GetPlaying()->GetBitrate().c_str() );
	str.Replace( _T( "$TIMESPLAYED" ), musikCube::g_Player->GetPlaying()->GetTimesPlayed().c_str() );
	str.Replace( _T( "$TRACKNUM" ), musikCube::g_Player->GetPlaying()->GetTrackNum().c_str() );
	str.Replace( _T( "$RATING" ), musikCube::g_Player->GetPlaying()->GetRating().c_str() );
}

///////////////////////////////////////////////////

// this is pretty much taken from Oddsock's Do Something plugin:
// http://www.oddsock.org/tools/dosomething/
void CmusikTunage::URLEncode( musikCore::String& str )
{
	str.Replace( _T( "%" ), _T( "%25" ) );
	str.Replace( _T( ";" ), _T( "%3B" ) );
	str.Replace( _T( "/" ), _T( "%2F" ) );
	str.Replace( _T( "?" ), _T( "%3F" ) );
	str.Replace( _T( ":" ), _T( "%3A" ) );
	str.Replace( _T( "@" ), _T( "%40" ) );
	str.Replace( _T( "&" ), _T( "%26" ) );
	str.Replace( _T( "=" ), _T( "%3D" ) );
	str.Replace( _T( "+" ), _T( "%2B" ) );
	str.Replace( _T( " " ), _T( "%20" ) );
	str.Replace( _T( "\"" ), _T( "%22" ) );
	str.Replace( _T( "#" ), _T( "%23" ) );
	str.Replace( _T( "<" ), _T( "%3C" ) );
	str.Replace( _T( ">" ), _T( "%3E" ) );
	str.Replace( _T( "!" ), _T( "%21" ) );
	str.Replace( _T( "*" ), _T( "%2A" ) );
	str.Replace( _T( "'" ), _T( "%27" ) );
	str.Replace( _T( "(" ), _T( "%28" ) );
	str.Replace( _T( ")" ), _T( "%29" ) );
	str.Replace( _T( "," ), _T( "%2C" ) );
}