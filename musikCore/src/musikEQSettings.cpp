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
#include "../include/musikEQSettings.h"

///////////////////////////////////////////////////

namespace musikCore {
    float s_PreampLevel = 48.0;
    float GetEQPreampLevel(){ return musikCore::s_PreampLevel; }
    void SetEQPreampLevel(float level){ musikCore::s_PreampLevel = level; }


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

    for (size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++)
    {
        m_Left[i] = 48.0f;
        m_Right[i] = 48.0f;
    }

    musikCore::SetEQPreampLevel(96.0);
}

///////////////////////////////////////////////////

void EQSettings::Set(int which_band, float values[])
{
    // left band
    if (which_band == MUSIK_EQ_SETTINGS_LEFT_BAND)
    {
        for (size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++)
            m_Left[i] = values[i];
        return;
    }

    // right band
    else if (which_band == MUSIK_EQ_SETTINGS_RIGHT_BAND)
    {
        for (size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++)
            m_Right[i] = values[i];
        return;
    }

    // both bands
    else if (which_band == MUSIK_EQ_SETTINGS_BOTH_BANDS)
    {
        for (size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++)
        {
            m_Left[i] = values[i];
            m_Right[i] = values[i];
        }
    }
}

///////////////////////////////////////////////////

