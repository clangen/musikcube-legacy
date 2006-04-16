///////////////////////////////////////////////////

#ifndef __C_MUSIKPLUGIN_H__
#define __C_MUSIKPLUGIN_H__

///////////////////////////////////////////////////

#include <bass.h>
#include <musikCore.h>
#include <iostream>
#include <vector>
#include <All.h>
#include <MACLib.h>

#ifdef MUSIKPLUGIN_EXPORTS
#define MUSIKPLUGIN_API __declspec(dllexport)
#else
#define MUSIKPLUGIN_API __declspec(dllimport)
#endif

///////////////////////////////////////////////////

extern "C" {

///////////////////////////////////////////////////

// these can be implemented and/or initialized 

extern bool g_CanConfigure;
extern bool g_CanAbout;
extern musikCore::String g_Name;
extern musikCore::String g_Description;	
extern musikCore::String g_Formats;
MUSIKPLUGIN_API void Configure();
MUSIKPLUGIN_API void About();
MUSIKPLUGIN_API void OnSources();
void OnPluginAttach();
void OnPluginDetach();

///////////////////////////////////////////////////

// stream prototype

///////////////////////////////////////////////////

// *** YOU DO NOT WANT TO MODIFY ANY OF THESE ***
// musikCube uses them for communication between the
// plugin and the main UI.

extern HMODULE g_Instance;
extern musikCore::Library* g_Library;
extern musikCore::Player* g_Player;
extern HWND g_MainWnd;

MUSIKPLUGIN_API	const musikCore::String& GetPluginFormats();
MUSIKPLUGIN_API	const musikCore::String& GetPluginName();
MUSIKPLUGIN_API	const musikCore::String& GetPluginDescription();
MUSIKPLUGIN_API void Unload();

///////////////////////////////////////////////////

// all of these must be implemented

MUSIKPLUGIN_API bool CanPlay( const musikCore::String& filename );
MUSIKPLUGIN_API bool Play( HSTREAM stream, int offset = 0 );
MUSIKPLUGIN_API bool CanSeek();
MUSIKPLUGIN_API bool Seek( HSTREAM stream, int ms );
MUSIKPLUGIN_API bool Pause( HSTREAM stream );
MUSIKPLUGIN_API bool Resume( HSTREAM stream );
MUSIKPLUGIN_API bool Stop( HSTREAM stream ); 
MUSIKPLUGIN_API int  GetDuration( HSTREAM stream );
MUSIKPLUGIN_API int  GetTime( HSTREAM stream );
MUSIKPLUGIN_API bool IsActive( HSTREAM stream );
MUSIKPLUGIN_API HSTREAM LoadFile( const musikCore::String& file );
MUSIKPLUGIN_API bool WriteTag( const musikCore::SongInfo& info );
MUSIKPLUGIN_API bool ReadTag( const musikCore::String& fn, musikCore::SongInfo& target );

///////////////////////////////////////////////////

}	// extern "C"

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

