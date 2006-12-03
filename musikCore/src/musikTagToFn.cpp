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
#include "../include/musikTagToFn.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// TagToFn

///////////////////////////////////////////////////

TagToFn::TagToFn()
{
    m_Library = NULL;
    m_Functor = NULL;
    m_Songs = NULL;

    m_DeleteSongs = true;
    m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

TagToFn::TagToFn(Library* pLibrary, SongArray* songs, Functor* pFunctor, String& mask, bool space_to_under)
{
    m_Library = pLibrary;
    m_Functor = pFunctor;
    m_Songs = songs;
    m_Mask = mask;
    m_SpaceToUndr = space_to_under;

    m_DeleteSongs = true;
    m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

TagToFn::~TagToFn()
{
    if (m_DeleteSongs && m_Songs)
        delete m_Songs;
}

///////////////////////////////////////////////////

// TagToFnTask

///////////////////////////////////////////////////

TagToFnTask::TagToFnTask()
    : Task()
{
    m_Params = NULL;
    m_Type = MUSIK_TASK_TYPE_TAGTOFN;    
}

///////////////////////////////////////////////////

TagToFnTask::~TagToFnTask()
{
    if (m_Params)
    {
        delete m_Params;
    }
    
    cout << "musikCore::TagToFnTask: deleted\n";
}

///////////////////////////////////////////////////

void TagToFnTask::run()
{
    m_Stop = false;
    m_Finished = false;
    m_Active = true;

    if (m_Params)
    {
        if (m_Params->m_Functor)
            m_Params->m_Functor->OnTaskStart();
        
        size_t last_prog = 0;

        SongInfo info;
        m_Params->m_Library->BeginTransaction();
        for(size_t i = 0; i < m_Params->m_Songs->size(); i++)
        {
            // check abort flag
            if (m_Stop)
            {
                cout << "musikCore::TagToFnTask: worker function aborted\n";
                break;
            }

            m_Params->m_Library->GetSongInfoFromID(m_Params->m_Songs->at(i).GetID(), info);
            Filename fn_rename(info.GetFilename());

            // convert spaces to underscores
            SongInfo temp = info;
            if (m_Params->m_SpaceToUndr)
                SpcToUndr(temp);

            if (fn_rename.RenameFromSongInfo(m_Params->m_Mask, temp))
            {
                info.SetFilename(temp.GetFilename());
                m_Params->m_Library->SetSongInfo(info);
            }

            // post progress to the functor
            m_Progress = (100 * i) / m_Params->m_Songs->size();
            if (m_Progress != last_prog)
            {
                if (m_Params->m_Functor)
                    m_Params->m_Functor->OnTaskProgress(m_Progress, this);
                last_prog = m_Progress;
            }
        }
        m_Params->m_Library->EndTransaction();
    }

    m_Finished = true;
    m_Progress = 100;
    
    if (m_Params->m_Functor)
    {
        if (!m_Stop || m_Params->m_CallFunctorOnAbort)
            m_Params->m_Functor->OnTaskEnd(this);
    }
}

///////////////////////////////////////////////////

void TagToFnTask::SpcToUndr(SongInfo& info)
{
    String strTemp;

    strTemp = info.GetTitle();
    strTemp.Replace(_T(" "), _T("_"));
    info.SetTitle(strTemp);

    strTemp = info.GetArtist();
    strTemp.Replace(_T(" "), _T("_"));
    info.SetArtist(strTemp);

    strTemp = info.GetAlbum();
    strTemp.Replace(_T(" "), _T("_"));
    info.SetAlbum(strTemp);

    strTemp = info.GetGenre();
    strTemp.Replace(_T(" "), _T("_"));
    info.SetGenre(strTemp);

    strTemp = info.GetYear();
    strTemp.Replace(_T(" "), _T("_"));
    info.SetYear(strTemp);
}

///////////////////////////////////////////////////
