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
#include "musikDynDspInfo.h"

///////////////////////////////////////////////////

DynDspInfo::DynDspInfo(musikCore::Playlist* songs)
{
    m_Songs = songs;
    m_LastItemRange.Set(-999, -999);
}

///////////////////////////////////////////////////

void DynDspInfo::SetPlaylist(musikCore::Playlist* playlist)
{
    m_Songs = playlist;
}

///////////////////////////////////////////////////

void DynDspInfo::Set(int from, int to, bool force_update)
{ 
    // set the current item range
    m_ItemRange.Set(from, to + 1);

    // see if our new item range is the same as our 
    // last item range... if so, return
    if (!force_update)
    {
        if (m_ItemRange.GetFirst() == m_LastItemRange.GetFirst() &&
            m_ItemRange.GetLast() == m_LastItemRange.GetLast())
            return;
    }

    // we got a 0,0 range, so check to see if there
    // are any songs in the playlist. if there are
    // not then that means nothing has been selected,
    // and this cache hint was sent even though its not
    // needed. just return.
    if (m_ItemRange.GetLast() > (int)m_Songs->GetCount())
        return;

    // grab all the new items
    musikCore::Library::GetInfoArrayFromPlaylist(
        *m_Songs, 
        m_Items,
        (size_t)m_ItemRange.GetFirst(), 
        (size_t)m_ItemRange.GetLast());

    /*
    SongInfo item;
    int nID;
    for (int i = m_ItemRange.GetFirst(); i < m_ItemRange.GetLast(); i++)
    {
        nID = m_Songs->GetSongID(i);

        musikCore::Library* curr_lib = m_Songs->at(i).GetLibrary();
        curr_lib->GetSongInfoFromID(nID, item);

        m_Items.push_back(item);
    }
    */

    
    // store the last known range, so we
    // can later compare it to anew one
    m_LastItemRange = m_ItemRange;
}    

///////////////////////////////////////////////////

musikCore::String DynDspInfo::GetValue(int id, int field)
{
    // this is kludge, but right now i don't have
    // the time to figure out why exactly its happening...


    try
    {
        if (id > (int)m_Items.size() || id < 0)
        {
            #ifdef _DEBUG
                musikCore::String sErr;
                sErr.Format(_T("DynDspInfo failed. Item %d was requested, but there are only %d items\n"), id, m_Items.size()-1);
                cout << sErr.c_str();
            #endif
            
            return _T("[musik.cache]");
        }

        return m_Items.at(id).GetField(field);
    }
    catch (...)
    {
#ifdef WIN32
        cout << _T("DynDspInfo::GetValue: error") << endl; 
#endif
    }

    return _T("[musik.cache]");
}    

///////////////////////////////////////////////////

bool DynDspInfo::ResyncItem(int songid)
{
    if (songid == -1)
        return false;

    for (size_t i = 0; i < m_Items.size(); i++)
    {
        if (m_Items.at(i).GetID() == songid)
        {
            musikCore::SongInfo item;
            musikCore::Library* curr_lib = m_Items.at(i).GetLibrary();
            curr_lib->GetSongInfoFromID(songid, item);

            m_Items.at(i) = item;
            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////    

musikCore::SongInfoArray* DynDspInfo::items()
{ 
    return &m_Items; 
}

///////////////////////////////////////////////////

int DynDspInfo::GetDistance()
{
    return m_ItemRange.GetDistance();
}

///////////////////////////////////////////////////

int DynDspInfo::GetFirst()
{
    return m_ItemRange.GetFirst();
}

///////////////////////////////////////////////////

int DynDspInfo::GetLast()
{
    return m_ItemRange.GetLast();
}

///////////////////////////////////////////////////

