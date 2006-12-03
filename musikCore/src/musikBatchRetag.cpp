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
#include "../include/musikBatchRetag.h"
#include "../include/musikPlaylist.h"

#if defined (USE_BASS)
#include "../include/musikPlayerBASS.h"    // use plugins to write tags
#endif

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// BatchRetag

///////////////////////////////////////////////////

BatchRetag::BatchRetag()
{
    m_Functor = NULL;
    m_Library = NULL;
    m_UpdatedTags = NULL;
    m_CallFunctorOnAbort = false;
    m_DeleteUpdatedTags = true;
    m_WriteToFile = false;
}

///////////////////////////////////////////////////

BatchRetag::BatchRetag(Library* pLibrary, Functor* pFunctor, SongInfoArray* pUpdatedTags)
{
    m_Functor = pFunctor;
    m_Library = pLibrary;
    m_UpdatedTags = pUpdatedTags;
    m_CallFunctorOnAbort = false;
    m_DeleteUpdatedTags = true;
    m_WriteToFile = false;
}

///////////////////////////////////////////////////

BatchRetag::~BatchRetag()
{
    if (m_DeleteUpdatedTags)
        delete m_UpdatedTags;
}

///////////////////////////////////////////////////

// BatchRetagTask

///////////////////////////////////////////////////

BatchRetagTask::BatchRetagTask()
    : Task()
{
    m_Params = NULL;
    m_Type = MUSIK_TASK_TYPE_BATCHRETAG;
}

///////////////////////////////////////////////////

BatchRetagTask::~BatchRetagTask()
{
    if (m_Params)
    {
        delete m_Params;
    }

    cout << "musikCore::BatchRetagTask: deleted\n";
}

///////////////////////////////////////////////////

void BatchRetagTask::run()
{
    cout << "musikCore::BatchRetagTask: started\n";
    
    m_Stop = false;
    m_Finished = false;
    m_Active = true;

    if (m_Params)
    {
        if (m_Params->m_Functor)
            m_Params->m_Functor->OnTaskStart();
        
        size_t last_prog = 0;

        m_Params->m_Library->BeginTransaction();
        String sFormat;
        for(size_t i = 0; i < m_Params->m_UpdatedTags->size(); i++)
        {
            // check abort flag
            if (m_Stop)
            {
                cout <<"musikCore::BatchRetagTask: worker function aborted\n";
                break;
            }

            if (m_Params->m_WriteToFile)
            {
                m_Params->m_Library->GetSongFormatFromID(m_Params->m_UpdatedTags->at(i).GetID(), sFormat);
                if (sFormat != _T("net")   )    // can't tag a netstream
                {

#if defined (USE_BASS)
                    const String fn = m_Params->m_UpdatedTags->at(i).GetFilename();
                    for (size_t j = 0; j < PlayerBASS::s_Plugins.size(); j++)
                    {
                        if (PlayerBASS::s_Plugins[j]->CanPlay(fn))
                        {
                            if (PlayerBASS::s_Plugins[j]->WriteTag(m_Params->m_UpdatedTags->at(i)))
                                m_Params->m_UpdatedTags->at(i).SetDirtyFlag("0");    // no longer dirty
                        }
                    }
#else
                    if (m_Params->m_UpdatedTags->at(i).WriteInfo() == true)
                        m_Params->m_UpdatedTags->at(i).SetDirtyFlag("0");    // no longer dirty
#endif

                }
            }
            else
                m_Params->m_UpdatedTags->at(i).SetDirtyFlag("1");

            m_Params->m_Library->SetSongInfo(m_Params->m_UpdatedTags->at(i));

            // post progress to the functor
            m_Progress = (100 * i) / m_Params->m_UpdatedTags->size();
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
        if (!m_Stop || (m_Stop && m_Params->m_CallFunctorOnAbort))
            m_Params->m_Functor->OnTaskEnd(this);
    }
}

///////////////////////////////////////////////////
