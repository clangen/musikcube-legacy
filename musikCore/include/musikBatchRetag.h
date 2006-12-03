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

#ifndef C_MUSIK_BATCHRETAG_H
#define C_MUSIK_BATCHRETAG_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
A struct that contains information that will be passed
to a BatchRetagTask. Contained information includes a pointer
to a Library to store data to, a functor to post events to, and
a SongInfoArray containing the information to be written to file.

@author Casey Langen
@version alpha 0.7
 */
struct MUSIKEXPORT BatchRetag
{
    /**
     * Default constructor.
     */
    BatchRetag();
    /**
     * \param pLibrary library used for transactions. Must not be NULL.
     * \param pFunctor functor to receive events. Must not be NULL.
     * \param pUpdatedTags tags to be updated. Must not be NULL.
     */
    BatchRetag(Library* pLibrary, Functor* pFunctor, SongInfoArray* pUpdatedTags);
    ~BatchRetag();

    /**
     * Required. Library that the updated data will be stored to.
     */
    Library* m_Library;
    /**
     * Optional. Functor that will recieve events. 
     */
    Functor* m_Functor;
    /**
     * Required. The array of updated tags.
     */
    SongInfoArray* m_UpdatedTags;
    /**
     * Optional. Call m_Functor->OnThreadEnd() even if aborted. 
     * Defaults false.
     */
    bool m_CallFunctorOnAbort;
    /**
     * Optional. Delete m_UpdatedTags on completion. Defaults
     * to true.
     */
    bool m_DeleteUpdatedTags;
    /**
     * Optional. Write the tags to file as well. Defaults to false.
     */
    bool m_WriteToFile;
};

///////////////////////////////////////////////////

/**
A task (thread) that batch updates a list of songs based
on a respective SongInfoArray. 

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT BatchRetagTask : public Task
{

public:

    BatchRetagTask();
    ~BatchRetagTask();

    /**
     * Start task execution. m_Params is requird to be initialized
     * before calling this function. If m_Params is uninitalized
     * the task will simply return.
     */
    void run();

    /**
     * Required. If uninitialized the task will fail to run and
     * return before any processing is done. 
     */
    BatchRetag* m_Params;

};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
