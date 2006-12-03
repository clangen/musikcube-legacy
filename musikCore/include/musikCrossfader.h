///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
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
    void Set(int type, float duration);
    /**
     * Sets all the crossfade settings.
     *
     * \param newsong crossfade duration when a new song starts
     * \param pauseresume duration while pausing or resuming
     * \param seek duration to use while mixing
     * \param stop duration to use while stopping
     * \param exit duration to use while program exists
     */
    void Set(float newsong, float pauseresume, float seek, float stop, float exit);
    /**
     * Sets a string name for the crossfader
     * \param name name for the crossfader
     */
    void SetName(const String& name){ m_Name = name; }


    /** 
     * \param type type of crossfader to be queryed
     * \return returns the duration of the specified crossfader
     */
    float GetDuration(int type) const;
    /**
     * \return the string name of the crossfader
     */
    String GetName() const { return m_Name;    }

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
