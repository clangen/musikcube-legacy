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
#include "../include/musikRemoveOld.h"
#include "../include/musikPlaylist.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// RemoveOld

///////////////////////////////////////////////////

RemoveOld::RemoveOld()
{
    m_Functor = NULL;
    m_Library = NULL;
    m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

RemoveOld::RemoveOld(Library* pLibrary, Functor* pFunctor)
{
    m_Functor = pFunctor;
    m_Library = pLibrary;
    m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

// RemoveOldTask

///////////////////////////////////////////////////

RemoveOldTask::RemoveOldTask()
    : Task()
{
    m_Params = NULL;
    m_Type = MUSIK_TASK_TYPE_REMOVEOLD;
}

///////////////////////////////////////////////////

RemoveOldTask::~RemoveOldTask()
{    
    if (m_Params)
    {
        delete m_Params;
    }

    cout << "musikCore::RemoveOldTask: deleted\n";
}

///////////////////////////////////////////////////

void RemoveOldTask::run()
{
    m_Stop = false;
    m_Finished = false;
    m_Active = true;

    cout << "musikCore::RemoveOldTask: started\n";
    
    if (m_Params)
    {
        if (m_Params->m_Functor)
            m_Params->m_Functor->OnTaskStart();
        
        size_t last_prog = 0;

        StringArray all_files;
        m_Params->m_Library->GetAllDistinct(MUSIK_LIBRARY_TYPE_FILENAME, all_files, false);

        m_Params->m_Library->BeginTransaction();
        for(size_t i = 0; i < all_files.size(); i++)
        {
            // check abort flag
            if (m_Stop)
            {
                cout << "musikCore::RemoveOldTask: worker function aborted\n";
                break;
            }

            // see if the file exists
            if (all_files.at(i).Left(7).ToLower() == _T("http://"))
                continue;
            
            if (!Filename::FileExists(all_files.at (i)))
                m_Params->m_Library->RemoveSong(all_files.at (i));

            // figure out percentage done, then post the 
            // progress event to the functor, if it exists.
            m_Progress = (100 * i) / all_files.size();
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
