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

#ifndef __C_MUSIK_RIP_H__
#define __C_MUSIK_RIP_H__

///////////////////////////////////////////////////

#include <iostream>
#include <musikCore.h>
#include <bass.h>
#include <bassenc.h>
#include <bassflac.h>
#include <basscd.h>

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

enum
{
    MUSIK_RIP_OGG = 0,
    MUSIK_RIP_FLAC,
    MUSIK_RIP_MP3
};

///////////////////////////////////////////////////

struct RipData
{
    String m_Path;
    int m_Format;
    int m_Quality;
    DWORD m_DriveID;
    bool m_SelectAllTracks;
    SongInfoArray* m_Songs;
    Functor* m_Functor;
    bool m_WriteTags;
    int m_Progress;
    int m_CurrTrack;
    int m_NumComplete;
    vector<int> m_Tracks;
    bool m_Error;
    String m_Mask;
    Library* m_Library;
    bool m_ID3V1;
    bool m_ID3V2;
};

///////////////////////////////////////////////////

class Rip : public Task
{
public:

    Rip(RipData* m_Params);
    Rip();
    ~Rip();

    void SetParams(RipData* params){ m_Params = params; }

    virtual void run();
    bool m_Error;
    RipData* m_Params;

private:

    int GetTrackCount();
    void GetAllTracks();   
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

