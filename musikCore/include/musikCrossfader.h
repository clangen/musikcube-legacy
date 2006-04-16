
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

#ifndef C_MUSIK_CROSSFADER
#define C_MUSIK_CROSSFADER

///////////////////////////////////////////////////

#include "musikConfig.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

MUSIKEXPORT enum 
{
	MUSIK_CROSSFADER_NONE = -1,
	MUSIK_CROSSFADER_NEW_SONG,
	MUSIK_CROSSFADER_PAUSE_RESUME,
	MUSIK_CROSSFADER_SEEK,
	MUSIK_CROSSFADER_STOP,
	MUSIK_CROSSFADER_EXIT,
	MUSIK_CROSSFADER_CDROM
};

///////////////////////////////////////////////////

/**
An object that contains crossfader settings. Library can be used
to manage presets of them. Player uses them for settings.

@author Casey Langen
@version alpha 0.7
*/
class MUSIKEXPORT Crossfader
{

public:

	/**
	 * Default constructor.
	 */
	Crossfader();
	~Crossfader(){}

	/**
	 * Sets the specified type of crossfader with a duration.
	 *
	 * \param type type of crossfader setting to modify
	 * \param duration duration in seconds
	 */
	void Set( int type, float duration );
	/**
	 * Sets all the crossfade settings.
	 *
	 * \param newsong crossfade duration when a new song starts
	 * \param pauseresume duration while pausing or resuming
	 * \param seek duration to use while mixing
	 * \param stop duration to use while stopping
	 * \param exit duration to use while program exists
	 */
	void Set( float newsong, float pauseresume, float seek, float stop, float exit );
	/**
	 * Sets a string name for the crossfader
	 * \param name name for the crossfader
	 */
	void SetName( const String& name ){ m_Name = name; }


	/** 
	 * \param type type of crossfader to be queryed
	 * \return returns the duration of the specified crossfader
	 */
	float GetDuration( int type ) const;
	/**
	 * \return the string name of the crossfader
	 */
	String GetName() const { return m_Name;	}

	/**
	 * Crossfader duration when the player starts a new song
	 */
	float m_NewSong;
	/**
	 * Crossfader duration when the player pauses playback
	 */
	float m_PauseResume;
	/**
	 * Crossfader duration when the player seeks the currently
	 * playint track.
	 */
	float m_Seek;
	/**
	 * Crossfader duration when the player stops playback
	 */
	float m_Stop;
	/**
	 * Crossfader duration when the program exits. 
	 */
	float m_Exit;
	/**
	 * Unique ID of the crossfader as determined by Library
	 */
	int m_ID;
	/**
	 * String identifier of the crossfader settings.
	 */
	String m_Name;
};


///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
