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

#ifndef C_MUSIK_SONG_INFO_CACHE
#define C_MUSIK_SONG_INFO_CACHE

///////////////////////////////////////////////////

#include <musikCore.h>
#include <iostream>

///////////////////////////////////////////////////

class DspInfoRange
{
public:

    DspInfoRange(){ m_First = m_Last = 0; }
    ~DspInfoRange(){}

    void Set(int first, int last){ m_First = first; m_Last = last; }

    int GetDistance(){ return m_Last - m_First; }
    int GetFirst(){ return m_First; }
    int GetLast(){ return m_Last; }
    bool IsNull(){ return (GetDistance() == 0 ? true : false); }

    int m_First, m_Last;
};

///////////////////////////////////////////////////

class DynDspInfo
{
public:

    // construct and destruct
    DynDspInfo(musikCore::Playlist* songs);
    ~DynDspInfo(){}

    // sets
    void SetPlaylist(musikCore::Playlist* playlist);
    void Set(int from, int to, bool force_update);

    // gets
    musikCore::String GetValue(int id, int field);    
    musikCore::SongInfoArray* items();
    int GetDistance();
    int GetFirst();
    int GetLast();

    // actions
    bool ResyncItem(int songid);

    DspInfoRange m_LastItemRange;

private:

    DspInfoRange m_ItemRange;
    musikCore::SongInfoArray m_Items;
    musikCore::Playlist* m_Songs;

};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
