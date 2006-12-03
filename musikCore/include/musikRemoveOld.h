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

#ifndef C_MUSIK_REMOVEOLD_H
#define C_MUSIK_REMOVEOLD_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
a Struct that contains information that will be passed
to a RemoveOldTask. Contained information includes a pointer
to a Library used to retrieve and store data and a functor
to post events to.

@author Casey Langen
@version alpha 0.7
 */
struct MUSIKEXPORT RemoveOld
{
    RemoveOld();
    RemoveOld(Library* pLibrary, Functor* pFunctor);
    ~RemoveOld(){}

    /**
     * Required. The Library used for transactions.
     */
    Library* m_Library;
    /**
     * Optional. The functor used to post events to.
     */
    Functor* m_Functor;
    /**
     * Optional. If true m_Functor->OnThreadEnd() will be called
     * even if the task is aborted. Defaults false.
     */
    bool m_CallFunctorOnAbort;
};

///////////////////////////////////////////////////

class MUSIKEXPORT RemoveOldTask : public Task
{

public:

    RemoveOldTask();
    ~RemoveOldTask();

    /**
     * Start task execution. m_Params must be initialized.
     */
    void run();

    /**
     * Required. If uninitialized the task will return immediately.
     */
    RemoveOld* m_Params;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
