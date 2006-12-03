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

#ifndef C_MUSIK_BATCHADD_H
#define C_MUSIK_BATCHADD_H

///////////////////////////////////////////////////

#include "musikLibrary.h"
#include "musikPlayer.h"
#include "musikFunctor.h"
#include "musikTask.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
A struct that contains information that will be passed
to a BatchAddTask. Contained information includes a list
of files to add, a playlist to add the files to, a library to
store information to, a player, and a functor to call for
various events.

@author Casey Langen
@version alpha 0.7
 */
struct MUSIKEXPORT BatchAdd
{
    BatchAdd();
    BatchAdd(StringArray* pFiles, const String& path, Playlist* pPlaylist, Library* pLibrary, Player* pPlayer, 
        Functor* pFunctor, bool bUpdatePlaylist = false, bool bAddToPlayer = false, 
        bool bDeleteFilelist = true);

    ~BatchAdd();

    /**
     * Required. List of files to be added to the library.
     */
    StringArray* m_Files;
    /**
     * Optional. Playlist to add files to once verified and
     * added to the library. m_UpdatePlaylist must be true.
     */
    Playlist* m_Playlist;
    /**
     * Required. Library to add files to
     */
    Library* m_Library;
    /**
     * Optional. If m_AddToPlayer is true, songs will be added
     * to the m_Player's current playlist once verified.
     */
    Player* m_Player;
    /**
     * Optional. Functor to post events to. Defaults to NULL.
     */
    Functor* m_Functor;
    /**
     * Optional. Once a song has been added to the library, append it to
     * m_Playlist as well. Defaults to false.
     */
    bool m_UpdatePlaylist;
    /**
     * Optional. Delete the passed file list once thread execution completes.
     * Defaults to true.
     */
    bool m_DeleteFilelist;
    /**
     * Optional. If true, files will be added to m_Player's Playlist once
     * verified and added to the library. Defaults to false.
     */
    bool m_AddToPlayer;            
    /**
     * Optional. Call m_Functor->OnThreadEnd() if process is aborted? Defaults
     * to false.
     */
    bool m_CallFunctorOnAbort;
    /**
     * Optional. Will add files in this directory.
     */
    String m_Directory;

    bool m_Clean;
};

///////////////////////////////////////////////////

/**
A task (thread) that batch adds a list of files to a Library
as specified in a BatchAddTask

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT BatchAddTask : public Task
{

public:

    BatchAddTask();
    ~BatchAddTask();

    /**
     * Call this to start the task going. m_Params must be set
     * prior to calling run().
     */
    void run();

    /**
     * If true the thread will call "delete this" once its run() has completed.
     * This only has an effect in non-win32 (linux) environments. In win32 the
     * thread cannot delete itself. If true in a win32 environment an assertion 
     * will be thrown.
     */
    BatchAdd* m_Params;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
