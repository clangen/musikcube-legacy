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

#ifndef C_MUSIK_RELOADTAGS_H
#define C_MUSIK_RELOADTAGS_H

///////////////////////////////////////////////////

#include "musikLibrary.h"
#include "musikPlayer.h"
#include "musikFunctor.h"
#include "musikTask.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

struct MUSIKEXPORT ReloadTags
{
    ReloadTags(Functor* functor = NULL, Playlist* playlist = NULL, Library* library = NULL);
    ~ReloadTags();

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
    * Required. Functor to post events to
    */
    Functor* m_Functor;

    bool m_CallFunctorOnAbort;
};

///////////////////////////////////////////////////

class MUSIKEXPORT ReloadTagsTask : public Task
{

public:

    ReloadTagsTask();
    ~ReloadTagsTask();

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
    ReloadTags* m_Params;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
