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

#include "stdafx.h"
#include "../include/musikCrossfader.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

Crossfader::Crossfader()
{
    m_NewSong        = 0.0f;
    m_PauseResume    = 0.0f;
    m_Seek            = 0.0f;
    m_Stop            = 0.0f;
    m_Exit            = 0.0f;

    m_ID = -1;
}

///////////////////////////////////////////////////

void Crossfader::Set(int type, float duration)
{
    switch(type)
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

void Crossfader::Set(float newsong, float pauseresume, float seek, float stop, float exit)
{
    m_NewSong        = newsong;
    m_PauseResume    = pauseresume;
    m_Seek            = seek;
    m_Stop            = stop;
    m_Exit            = exit;
}

///////////////////////////////////////////////////

float Crossfader::GetDuration(int type) const
{
    switch(type)
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
