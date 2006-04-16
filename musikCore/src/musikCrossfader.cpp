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
// Version: alpha 6, March 28, 2004
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikCrossfader.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

Crossfader::Crossfader()
{
	m_NewSong		= 0.0f;
	m_PauseResume	= 0.0f;
	m_Seek			= 0.0f;
	m_Stop			= 0.0f;
	m_Exit			= 0.0f;

	m_ID = -1;
}

///////////////////////////////////////////////////

void Crossfader::Set( int type, float duration )
{
	switch( type )
	{
	case MUSIK_CROSSFADER_NEW_SONG:
		m_NewSong = duration;
		return;

	case MUSIK_CROSSFADER_PAUSE_RESUME:
		m_PauseResume = duration;
		return;

	case MUSIK_CROSSFADER_SEEK:
		m_Seek = duration;
		return;

	case MUSIK_CROSSFADER_STOP:
		m_Stop = duration;
		return;

	case MUSIK_CROSSFADER_EXIT:
		m_Exit = duration;
		return;
	}
}

///////////////////////////////////////////////////

void Crossfader::Set( float newsong, float pauseresume, float seek, float stop, float exit )
{
	m_NewSong		= newsong;
	m_PauseResume	= pauseresume;
	m_Seek			= seek;
	m_Stop			= stop;
	m_Exit			= exit;
}

///////////////////////////////////////////////////

float Crossfader::GetDuration( int type ) const
{
	switch( type )
	{
	case MUSIK_CROSSFADER_NONE:
		return 0.0f;

	case MUSIK_CROSSFADER_NEW_SONG:
		return m_NewSong;

	case MUSIK_CROSSFADER_PAUSE_RESUME:
		return m_PauseResume;

	case MUSIK_CROSSFADER_SEEK:
		return m_Seek;

	case MUSIK_CROSSFADER_STOP:
		return m_Stop;

	case MUSIK_CROSSFADER_EXIT:
		return m_Exit;
	}

	return 0.0f;
}

///////////////////////////////////////////////////
