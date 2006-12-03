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
#include "../include/musikBatchAdd.h"
#include "../include/musikPlaylist.h"
#include "../include/musikDir.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// BatchAdd

///////////////////////////////////////////////////

BatchAdd::BatchAdd()
{
    m_Files                    = NULL;
    m_Playlist                = NULL;
    m_Functor                = NULL;
    m_UpdatePlaylist        = false;
    m_DeleteFilelist        = true;
    m_AddToPlayer            = false;
    m_Library                = NULL;
    m_Player                = NULL;
    m_CallFunctorOnAbort    = false;
    m_Directory             = "";
}

///////////////////////////////////////////////////

BatchAdd::BatchAdd(StringArray* pFiles, const String& path, Playlist* pPlaylist, Library* pLibrary, Player* pPlayer, 
    Functor* pFunctor, bool bUpdatePlaylist, bool bAddToPlayer, bool bDeleteFilelist)
{
    m_Files                    = pFiles;
    m_Playlist                = pPlaylist;
    m_Functor                = pFunctor;
    m_Library                = pLibrary;
    m_UpdatePlaylist        = bUpdatePlaylist;
    m_DeleteFilelist        = bDeleteFilelist;
    m_Player                = pPlayer;
    m_AddToPlayer            = bAddToPlayer;
    m_CallFunctorOnAbort    = false;
    m_Directory             = path;
}

///////////////////////////////////////////////////

BatchAdd::~BatchAdd()
{
    if (m_DeleteFilelist && m_Files)
        delete m_Files;
}

///////////////////////////////////////////////////

// BatchAddTask

///////////////////////////////////////////////////

BatchAddTask::BatchAddTask()
    : Task()
{
    m_Params = NULL;
    m_Type = MUSIK_TASK_TYPE_BATCHADD;
}

///////////////////////////////////////////////////

BatchAddTask::~BatchAddTask()
{
    if (m_Params)
    {
        delete m_Params;
    }

    cout << "musikCore::BatchAddTask: exiting...\n";
}

///////////////////////////////////////////////////

void BatchAddTask::run()
{
    m_Stop = false;
    m_Finished = false;
    m_Active = true;

    if (m_Params)
    {
        if (!m_Params->m_Files)
        {
            m_Params->m_Files = new StringArray();
            m_Params->m_DeleteFilelist = true;
        }

        // parse directories. they are delimited by "||"
        if (m_Params->m_Directory.IsEmpty() == false)
        {
            musikCore::StringArray dirs;
            musikCore::Filename::DelimitStr(m_Params->m_Directory, _T("||"), dirs, false, true, true);
            for(size_t i = 0; i < dirs.size(); i++)
                   musikCore::Dir::OpenDir(dirs.at(i), m_Params->m_Files);
        }
        
        if (m_Params->m_Functor)
            m_Params->m_Functor->OnTaskStart();
        
        size_t last_prog = 0;
        bool verify_failed = false;

        Song song;

        m_Params->m_Library->BeginTransaction();
        for(size_t i = 0; i < m_Params->m_Files->size(); i++)
        {
            if (m_Stop)
            {
                cout << "musikCore::BatchAddTask: worker function aborted\n";
                break;
            }

            // add the song
            if (m_Params->m_Library->AddSong(m_Params->m_Files->at(i)))
            {
                // adding to now playing
                if (m_Params->m_AddToPlayer && m_Params->m_Player && m_Params->m_Player->GetPlaylist())
                {
                    m_Params->m_Library->GetSongAttrFromFilename(m_Params->m_Files->at(i), song);
                    song.SetLibrary(m_Params->m_Library);
                    m_Params->m_Player->GetPlaylist()->Add(song);
                }

                // adding to current playlist
                if (m_Params->m_UpdatePlaylist && m_Params->m_Playlist)
                {
                    if (!verify_failed)
                    {
                        if (m_Params->m_Functor)
                        {
                            if (m_Params->m_Functor->VerifyPlaylist((void*)m_Params->m_Playlist))
                            {
                                m_Params->m_Library->GetSongAttrFromFilename(m_Params->m_Files->at(i), song);
                                song.SetLibrary(m_Params->m_Library);
                                m_Params->m_Playlist->Add(song);
                            }
                        }
                    }
                    else
                    {
                        verify_failed = true;
                        String s;
                        s.Format(_T("musikCore::BatchAddTask: failed to add song to playlist at address %d becuase it couldn't be verified\n"), m_Params->m_Playlist);
                        cout << s.c_str();
                    }
                }
            }

            // post progress to the functor
            m_Progress = (100 * i) / m_Params->m_Files->size();
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
