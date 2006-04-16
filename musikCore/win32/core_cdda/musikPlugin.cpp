///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPlugin.h"

#include <windows.h>
#include <basscd.h>

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***

musikCore::String g_Name = _T( "core_cdda" );
musikCore::String g_Description = _T( "CD Digital Audio" );
musikCore::String g_Formats = _T( "cdda" );
bool g_CanConfigure = true;
bool g_CanAbout = true;

///////////////////////////////////////////////////

// these will get initialized automatically by musikCube

HMODULE g_Instance = NULL;
HSYNC g_Sync = NULL;

///////////////////////////////////////////////////

int g_CDDrive = 0;
int g_CDTrack = 0;

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
	// we handle netstreams and cda...
	if ( filename.Left( 4 ) == _T( "cdda" ) )
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
	{
		Seek( stream, offset );
	}

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
	DWORD length = BASS_CD_GetTrackLength( g_CDDrive, g_CDTrack );
	DWORD secs = length / 176400;
	return (int)secs * 1000;
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

	musikCore::StringArray target;
	musikCore::Filename::DelimitStr( filename, _T( "," ), target, false );

	if ( target.size() != 3 )
	{
		return NULL; // malformed 
	}

	g_CDDrive = musikCore::StringToInt( target.at( 1 ) );
	g_CDTrack = musikCore::StringToInt( target.at( 2 ) );

	load = BASS_CD_StreamCreate(
		g_CDDrive,	
		g_CDTrack,	
		NULL );

	return load;
}

///////////////////////////////////////////////////

bool WriteTag( const musikCore::SongInfo& info )
{
	return false;
}

///////////////////////////////////////////////////

bool ReadTag( const musikCore::String& fn, musikCore::SongInfo& target )
{
	return false;
}

///////////////////////////////////////////////////