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

#ifndef C_MUSIK_TAGTOFN_H
#define C_MUSIK_TAGTOFN_H

///////////////////////////////////////////////////

#include "musikConfig.h"

#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
 Parameters for a TagToFnTask, which allows batch
 renaming of managed files based on tag information
 and a specified mask.

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT TagToFn
{
public: 

    TagToFn();
    TagToFn(Library* pLibrary, SongArray* songs, Functor* pFunctor, String& mask, bool space_to_under);
    ~TagToFn();

    /**
     * Library to save information back to.
     */
    Library* m_Library;
    /**
     * Functor to call for events
     */
    Functor* m_Functor;
    /**
     * Array of songs that the user wishes to rename
     */    
    SongArray* m_Songs;
    /**
     * Mask used for renaming
     */
    String m_Mask;
    /**
     * Delete the song array upon completion?
     */
    bool m_DeleteSongs;
    /**
     * Call m_Functor->OnThreadEnd() if process is aborted?
     */
    bool m_CallFunctorOnAbort;
    /**
     *
     */
    bool m_SpaceToUndr;
};

///////////////////////////////////////////////////

/**
A task (thread) that batch renames files based on their
filename and a mask.

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT TagToFnTask : public Task
{

public:

    TagToFnTask();
    ~TagToFnTask();

    /**
     * Starts task execution. m_Params must be set prior to calling.
     */
    void run();

    /**
     * Paramters for execution. If uninitialized nothing will happen
     * and the task will simply return.
     */
    TagToFn* m_Params;

private:

    void SpcToUndr(SongInfo& info);
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
