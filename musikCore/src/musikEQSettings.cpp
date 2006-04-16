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
#include "../include/musikEQSettings.h"

///////////////////////////////////////////////////

namespace musikCore {
	float s_PreampLevel = 48.0;
	float GetEQPreampLevel(){ return musikCore::s_PreampLevel; }
	void SetEQPreampLevel( float level ){ musikCore::s_PreampLevel = level; }


	int g_EQFreqs[18] =
		{ 55, 77, 110, 156, 220, 311, 
		440, 622, 880, 1244, 1760, 2489, 
		3520, 4978, 7040, 10056, 14080, 19912 };

	int* GetEQFreqs(){ return g_EQFreqs; }
}

///////////////////////////////////////////////////

using namespace musikCore;

float s_PreAmpLevel = 48.0;

///////////////////////////////////////////////////

EQSettings::EQSettings()
{
	m_ID = -1;

	for ( size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++ )
	{
		m_Left[i] = 48.0f;
		m_Right[i] = 48.0f;
	}

	musikCore::SetEQPreampLevel( 96.0 );
}

///////////////////////////////////////////////////

void EQSettings::Set( int which_band, float values[] )
{
	// left band
	if ( which_band == MUSIK_EQ_SETTINGS_LEFT_BAND )
	{
		for ( size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++ )
			m_Left[i] = values[i];
		return;
	}

	// right band
	else if ( which_band == MUSIK_EQ_SETTINGS_RIGHT_BAND )
	{
		for ( size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++ )
			m_Right[i] = values[i];
		return;
	}

	// both bands
	else if ( which_band == MUSIK_EQ_SETTINGS_BOTH_BANDS )
	{
		for ( size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++ )
		{
			m_Left[i] = values[i];
			m_Right[i] = values[i];
		}
	}
}

///////////////////////////////////////////////////

