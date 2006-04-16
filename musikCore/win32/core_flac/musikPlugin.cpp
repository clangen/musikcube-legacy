///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPlugin.h"

#include <windows.h>
#include <bassflac.h>

#include <taglib/fileref.h>
#include <taglib/tag.h>

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***

musikCore::String g_Name = _T( "core_flac" );
musikCore::String g_Description = _T( "Free Lossless Audio Codec (FLAC)" );
musikCore::String g_Formats = _T( "flac" );
bool g_CanConfigure = true;
bool g_CanAbout = true;

///////////////////////////////////////////////////

// these will get initialized automatically by musikCube

HMODULE g_Instance = NULL;
HSYNC g_Sync = NULL;

///////////////////////////////////////////////////

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved	 )
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

bool CanPlay( const musikCore::String& filename )
{
	musikCore::Filename MFN( filename );
	if ( MFN.GetExtension() == _T( "flac" ) )
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////

bool Play( HSTREAM stream, int offset )
{
	BASS_ChannelPlay(
		stream,
		true
		);

	if ( offset )
		Seek( stream, offset );

	return true;
}

///////////////////////////////////////////////////

bool CanSeek()
{ 
	return true; 
}

///////////////////////////////////////////////////

bool Seek( HSTREAM stream, int ms )
{
	float secs = (float)ms / 1000.0f;
	QWORD pos = BASS_ChannelSeconds2Bytes( stream, secs );
	BASS_ChannelSetPosition( stream, pos );

	return true;
}

///////////////////////////////////////////////////

bool Pause( HSTREAM stream )
{
	BASS_ChannelPause( stream );
	return true;
}

///////////////////////////////////////////////////

bool Resume( HSTREAM stream )
{
	BASS_ChannelPlay( stream, false );
	return true;
}

///////////////////////////////////////////////////

bool Stop( HSTREAM stream )
{	
	BASS_ChannelStop( stream );
	BASS_StreamFree( stream );
	return true;
}

///////////////////////////////////////////////////

int GetDuration( HSTREAM stream )
{
	QWORD bytelength = BASS_ChannelGetLength( stream );
	float secs = BASS_ChannelBytes2Seconds( stream, bytelength ); 
	secs *= 1000; // to ms
	return (int)secs;
}

///////////////////////////////////////////////////

int GetTime( HSTREAM stream )
{
	QWORD pos = BASS_ChannelGetPosition( stream );
	float secs = BASS_ChannelBytes2Seconds( stream, pos );
	secs *= 1000.0f;
	return (int)secs;
}

///////////////////////////////////////////////////

bool IsActive( HSTREAM stream )
{
	DWORD state = BASS_ChannelIsActive( stream );

	if ( state == BASS_ACTIVE_PLAYING || state == BASS_ACTIVE_PAUSED )
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////

HSTREAM LoadFile( const musikCore::String& filename )
{
	HSTREAM load = NULL;

	// flac
	musikCore::Filename fn( filename );

	load = BASS_FLAC_StreamCreateFile(
		FALSE,
		filename.c_str(),
		0,
		0,
		BASS_STREAM_AUTOFREE | BASS_UNICODE );

	if ( load )
	{
		BASS_ChannelPreBuf( load, 500 );
	}
	
	return load;
}

///////////////////////////////////////////////////

bool WriteTag( const musikCore::SongInfo& info )
{
	bool ret = true;

	try
	{
#if defined (WIN32)
		TagLib::FileRef tag_file( info.GetFilename().c_str() );
#else
		TagLib::FileRef tag_file( utf16to8( info.GetFilename(), true ).c_str() );
#endif
		if ( !tag_file.isNull() )
		{
			TagLib::Tag *tag = tag_file.tag();

			tag->setArtist( info.GetArtist().c_str() );
			tag->setAlbum( info.GetAlbum().c_str() );
			tag->setTitle( info.GetTitle().c_str() );
			tag->setGenre( info.GetGenre().c_str() );
			tag->setYear( musikCore::StringToInt( info.GetYear() ) );
			tag->setTrack( musikCore::StringToInt( info.GetTrackNum() ) );
			tag->setComment( info.GetNotes().c_str() );

			tag_file.save();
		}
	}
	catch ( ... )
	{
		ret = false;
		cout << "taglib crashed trying to write: " << info.GetFilename().c_str() << endl;
	}

	return ret;
}

///////////////////////////////////////////////////

bool ReadTag( const musikCore::String& fn, musikCore::SongInfo& target )
{
	bool ret = true;

	musikCore::Filename mfn( fn );

	target.SetFilename( fn );
	target.SetFormat( mfn.GetExtension() );

	try
	{
#if defined (WIN32)
		TagLib::FileRef tag_file( fn.c_str() );
#else	
		TagLib::FileRef tag_file( utf16to8( fn, true ).c_str() );
#endif
		if ( !tag_file.isNull() )
		{
			if ( tag_file.tag() )
			{
				TagLib::Tag *tag = tag_file.tag();        

				target.SetArtist( musikCore::utf8to16( tag->artist().to8Bit( true ) ) );
				target.SetAlbum( musikCore::utf8to16( tag->album().to8Bit( true ) ) );      
				target.SetTitle( musikCore::utf8to16( tag->title().to8Bit( true ) ) );
				target.SetGenre( musikCore::utf8to16( tag->genre().to8Bit( true ) ) );
				target.SetNotes( musikCore::utf8to16( tag->comment().to8Bit( true ) ) );

				target.SetYear( musikCore::IntToString( tag->year() ) );
				target.SetTrackNum( musikCore::IntToString( tag->track() ) );
			}

			if ( tag_file.audioProperties() )
			{
				TagLib::AudioProperties *properties = tag_file.audioProperties();
				int duration = properties->length() * 1000;
				target.SetBitrate( musikCore::IntToString( properties->bitrate() ) );
				target.SetDuration( musikCore::IntToString( duration ) );
			}
		}

		// if the title is empty, then use the
		// filename...
		if ( target.GetTitle().IsEmpty() )
		{
			musikCore::Filename MFN( fn );
			target.SetTitle( MFN.GetJustFilename() );
		}
	}
	catch ( ... )
	{
		ret = false;
		cout << "taglib crashed reading: " << fn.c_str() << endl;
	}

	return ret;
}

///////////////////////////////////////////////////

