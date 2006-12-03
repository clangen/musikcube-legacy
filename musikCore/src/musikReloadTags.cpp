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
#include "../include/musikReloadTags.h"
#include "../include/musikPlaylist.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// ReloadTags

///////////////////////////////////////////////////

ReloadTags::ReloadTags(Functor* functor, Playlist* playlist, Library* library)
{
    m_Playlist = playlist;
    m_Library = library;
    m_Functor = functor;
    m_CallFunctorOnAbort = true;
}

///////////////////////////////////////////////////

ReloadTags::~ReloadTags()
{
    if (m_Playlist)
        delete m_Playlist;
}

///////////////////////////////////////////////////

// ReloadTagsTask

///////////////////////////////////////////////////

ReloadTagsTask::ReloadTagsTask()
    : Task()
{
    m_Params = NULL;
    m_Type = MUSIK_TASK_TYPE_RELOAD_TAGS;
}

///////////////////////////////////////////////////

ReloadTagsTask::~ReloadTagsTask()
{
    if (m_Params)
    {
        delete m_Params;
    }

    cout << "musikCore::BatchAddTask: exiting...\n";
}

///////////////////////////////////////////////////

void ReloadTagsTask::run()
{
    m_Stop = false;
    m_Finished = false;
    m_Active = true;

    if (m_Params)
    {      
        if (m_Params->m_Functor)
            m_Params->m_Functor->OnTaskStart();
        
        size_t last_prog = 0;
        bool verify_failed = false;

        Song song;

        m_Params->m_Library->BeginTransaction();
        for (size_t i = 0; i < m_Params->m_Playlist->size(); i++)
        {
            m_Params->m_Library->ReloadTags(m_Params->m_Playlist->at(i));

            // post progress to the functor
            m_Progress = (100 * i) / m_Params->m_Playlist->size();
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
    
    Functor* fnct = m_Params->m_Functor;
    bool call_on_abort = m_Params->m_CallFunctorOnAbort;
    
    if (fnct && ((m_Stop && call_on_abort) || !m_Stop))
        fnct->OnTaskEnd((Task*)this);
}

///////////////////////////////////////////////////
