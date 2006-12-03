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

#ifndef C_MUSIK_TAGFROMFN_H
#define C_MUSIK_TAGFROMFN_H

///////////////////////////////////////////////////

#include "musikConfig.h"

#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
 Parameters for a TagFromFnTask

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT TagFromFn
{
public: 

    TagFromFn();
    TagFromFn(Library* pLibrary, SongArray* songs, Functor* pFunctor, String& mask, bool undr_to_spc);
    ~TagFromFn();

    /**
     * Required. Library to save information back to.
     */
    Library* m_Library;
    /**
     * Optional. Functor to call for events
     */
    Functor* m_Functor;
    /**
     * Required. Array of songs that the user wishes to rename
     */    
    SongArray* m_Songs;
    /**
     * Required. Mask used for renaming
     */
    String m_Mask;
    /**
     * Optional. Delete the song array upon completion? Defaults true.
     */
    bool m_DeleteSongs;
    /**
     * Optinal. Call m_Functor->OnThreadEnd() if process is aborted? 
     * defaults false.
     */
    bool m_CallFunctorOnAbort;
    /**
     * Convert underscores to spaces.
     */
    bool m_UndrToSpace;
};

///////////////////////////////////////////////////

/**
A task (thread) that batch parses filenames, and, using a mask,
retrieves song information.

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT TagFromFnTask : public Task
{

public:

    TagFromFnTask();
    ~TagFromFnTask();

    /**
     * Start execution. m_Params must be initialized prior to calling.
     */
    void run();

    /**
     * Required. Paramters used for task execution. Must be initialized 
     * before  calling run() or the task will simply return.
     */
    TagFromFn* m_Params;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
