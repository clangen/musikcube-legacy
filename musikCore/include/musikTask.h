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

#ifndef C_MUSIK_TASK
#define C_MUSIK_TASK

///////////////////////////////////////////////////

#include "musikConfig.h"

#include <OpenThreads/Thread>
using namespace OpenThreads;

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

enum
{
    MUSIK_TASK_TYPE_UNKNOWN = -1,
    MUSIK_TASK_TYPE_BATCHADD = 0,
    MUSIK_TASK_TYPE_BATCHRETAG,
    MUSIK_TASK_TYPE_REMOVEOLD,
    MUSIK_TASK_TYPE_BATCHCHGCASE,
    MUSIK_TASK_TYPE_TAGFROMFN,
    MUSIK_TASK_TYPE_TAGTOFN,
    MUSIK_TASK_TYPE_PLAYER_WORKER,
    MUSIK_TASK_TYPE_RELOAD_TAGS,
    MUSIK_TASK_TYPE_LAST
};

///////////////////////////////////////////////////

/**
An object oriented thread. Any class wishing to become a thread
may inherit from Task. Override the virtual "run()" function
and implement your own operation.

@author Casey Langen
*/
class MUSIKEXPORT Task : public Thread
{
public:

    /**
     * Default constructor.
     */
    Task();
    virtual ~Task();

    /**
     * Returns the type of task we are. The m_Type paramater
     * should be set by the inherited class.
     *
     * \return type of task we are.
     */
    int GetType() const { return m_Type; }
    /**
     * Stop the thread and wait timeout_secs seconds before 
     * forcing the m_Active flag false and returning. Once
     * returned the object should be safe to delete.
     *
     * \param timeout_secs number of seconds to wait.
     */
    bool StopWait(int timeout_secs);
    /**
     * Gets the current percentage complete.
     *\return the percentage complete.
     */
    size_t GetProgress() const { return m_Progress; }

    /**
     * Returns true if the task is finished.
     */
    bool IsFinished() const { return m_Finished; }
    /**
     * Returns true if the task is active
     */
    bool IsActive() const { return m_Active; }
    /**
     * Sets the stop flag then returns without waiting to assure
     * the thread terminates.
     */
    void Stop(){ m_Stop = true; }

    void Start();

protected:

    /**
     * Is the thread currently active?
     */
    bool m_Active;
    /**
     * Has the thread set itself as finished? It is up to the developer
     * to set this to true once his void run() has completed.
     */
    bool m_Finished;
    /**
     * The abort flag
     */
    bool m_Stop;
    /**
     * Type of task. It is up to the developer to set this. 
     */
    int m_Type;
    /**
     * Current progress in percent (0-100)
     */
    size_t m_Progress;
};

typedef std::vector<Task*> TaskPtrArray;

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
