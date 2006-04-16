///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikCore.h"
#include "../include/musikBASSStream.h"

///////////////////////////////////////////////////

#if defined(WIN32) || defined(__MINGW32__) 
#if defined(USE_BASS)

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

Stream::Stream()
{
	m_IsLoaded = false;
	LoadFile = NULL;
	Play = NULL;
	CanSeek = NULL;
	Seek = NULL;
	Pause = NULL;
	Resume = NULL;
	Stop = NULL;
	GetDuration = NULL;
	GetTime = NULL;
	IsActive = NULL;
	CanPlay = NULL;
	ReadTag = NULL;
	WriteTag = NULL;
	Unload = NULL;
	GetPluginFormats = NULL;
	GetPluginName = NULL;
	GetPluginDescription = NULL;
	m_DSP = 0;
}

///////////////////////////////////////////////////

Stream::~Stream()
{
	if ( Stop )
		Stop( m_Stream );
}

///////////////////////////////////////////////////

Stream::Stream( const String& filename )
{
	m_IsLoaded = false;

	HMODULE mod = LoadLibrary( filename );

	LoadFile = (LOADFILE)GetProcAddress( mod, "LoadFile" );
	Play = (PLAY)GetProcAddress( mod, "Play" );
	CanSeek = (CANSEEK)GetProcAddress( mod, "CanSeek" );
	Seek = (SEEK)GetProcAddress( mod, "Seek" );
	Pause = (PAUSE)GetProcAddress( mod, "Pause" );
	Resume = (RESUME)GetProcAddress( mod, "Resume" );
	Stop = (STOP)GetProcAddress( mod, "Stop" );
	GetDuration = (GETDURATION)GetProcAddress( mod, "GetDuration" );
	GetTime = (GETTIME)GetProcAddress( mod, "GetTime" );
	IsActive = (ISACTIVE)GetProcAddress( mod, "IsActive" );
	CanPlay = (CANPLAY)GetProcAddress( mod, "CanPlay" );
	ReadTag = (READTAG)GetProcAddress( mod, "ReadTag" );
	WriteTag = (WRITETAG)GetProcAddress( mod, "WriteTag" );
	Unload = (UNLOAD)GetProcAddress( mod, "Unload" );			// never call manually. this is used
																// internally by musikCore::Player

	GetPluginFormats = (GETPLUGINFORMATS)GetProcAddress( mod, "GetPluginFormats" );
	GetPluginName = (GETPLUGINNAME)GetProcAddress( mod, "GetPluginName" );
	GetPluginDescription = (GETPLUGINDESCRIPTION)GetProcAddress( mod, "GetPluginDescription" );

	if ( LoadFile && Play && CanSeek && Seek && Pause && Resume && Stop &&
		 GetDuration && GetTime && IsActive && CanPlay && GetPluginFormats && 
		 GetPluginName && GetPluginDescription && ReadTag && WriteTag && Unload )
	{
		m_IsLoaded = true;
	}
}

///////////////////////////////////////////////////

#endif  // USE_BASS
#endif	// WIN32  


///////////////////////////////////////////////////
