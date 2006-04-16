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

#ifndef C_MUSIK_EQ_SETTINGS
#define C_MUSIK_EQ_SETTINGS

///////////////////////////////////////////////////

#include "musikConfig.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

MUSIKEXPORT enum 
{
	MUSIK_EQ_SETTINGS_LEFT_BAND = 0,
	MUSIK_EQ_SETTINGS_RIGHT_BAND,
	MUSIK_EQ_SETTINGS_BOTH_BANDS
};

///////////////////////////////////////////////////

#define MUSIK_EQ_SETTINGS_NUM_BANDS 18

///////////////////////////////////////////////////

extern int g_EQFreqs[18];
MUSIKEXPORT int* GetEQFreqs();
extern float s_PreampLevel;
MUSIKEXPORT float GetEQPreampLevel();
MUSIKEXPORT void SetEQPreampLevel( float level );

///////////////////////////////////////////////////

/**
Similar to Crossfader, EQSettings is an object that 
contains equalizer settings. Library can be used to manage 
presets of them. Player uses them for settings.

@version alpha 0.7
*/
class MUSIKEXPORT EQSettings
{
public:

	/**
	 * Default constructor.
	 */
	EQSettings();
	~EQSettings(){}

	/**
	 * Sets the equalizer band settings.
	 *
	 * \param which MUSIK_EQ_SETTINGS_LEFT_BAND, MUSIK_EQ_SETTINGS_RIGHT_BAND, or MUSIK_EQ_SETTINGS_BOTH_BANDS
	 * \param values[MUSIK_EQ_SETTINGS_NUM_BANDS] the band settings, specified as an array of floats
	 */
	void Set( int which_band, float values[MUSIK_EQ_SETTINGS_NUM_BANDS] );
	/**
	 * Left channel settings.
	 */
	float m_Left[MUSIK_EQ_SETTINGS_NUM_BANDS];
	/**
	 * Right channel settings.
	 */
	float m_Right[MUSIK_EQ_SETTINGS_NUM_BANDS];
	/**
	 * String name of the preset
	 */
	String m_Name;
	/**
	 * ID of the equalizer according to Library
	 */
	int m_ID;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
