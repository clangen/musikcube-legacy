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
#include "../include/musikTaskQueue.h"
#include <iostream>
using namespace musikCore;

///////////////////////////////////////////////////

void TaskQueue::TaskQueueWorker::SetQueue(TaskQueue* queue)
{
    m_TaskQueue = queue;
}

///////////////////////////////////////////////////

void TaskQueue::TaskQueueWorker::run()
{
    m_FlushQueue = false;
    m_Active = true;

    Task* ptrCurr = NULL;

    while (1)
    {       
        if (m_TaskQueue->size())
        {
            ptrCurr = m_TaskQueue->front();
            ptrCurr->start();
            
            while (!ptrCurr->IsFinished())
            {
                if (m_FlushQueue || m_Stop)
                {
                    ptrCurr->StopWait(2);
                    break;
                }

                Sleep(250);
            }

            delete ptrCurr;
            m_TaskQueue->pop();
        }

        // check to see if the queue needs to be flushed.
        // flushing the queue will delete all the
        // respective tasks...
        if (m_FlushQueue)
        {
            while (m_TaskQueue->size() )
            {
                delete m_TaskQueue->front();
                m_TaskQueue->pop();
            }

            m_FlushQueue = false;
        }

        // check termination request
        if (m_Stop)
        {
            m_Finished = true;
            return;
        }

        Sleep(250);
    }

    m_Finished = true;
}

///////////////////////////////////////////////////

TaskQueue::TaskQueueWorker::~TaskQueueWorker()
{
    cout << "musikCore::TaskQueue::TaskQueueWorker: exiting\n";
}

///////////////////////////////////////////////////

TaskQueue::TaskQueue()
{
    m_Worker.SetQueue(this);
    m_Worker.start();
}

///////////////////////////////////////////////////

TaskQueue::~TaskQueue()
{
    Flush();

    cout << "musikCore::TaskQueue: killing task queue worker\n";
    m_Worker.StopWait(1);
}

///////////////////////////////////////////////////

void TaskQueue::Flush()
{
    cout << "musikCore::TaskQueue: flushing queue...";
    // block current context until the
    // queue has been flushed
    if (size())
    {
        m_Worker.Flush();
        while (size())
            Sleep(250);
    }
    cout << "flushed.\n";
}

///////////////////////////////////////////////////

void TaskQueue::Push(Task* ptrThread)
{
    m_Queue.push(ptrThread);
}

///////////////////////////////////////////////////

size_t TaskQueue::size()
{
    return m_Queue.size();
}

///////////////////////////////////////////////////

void TaskQueue::pop()
{
    m_Queue.pop();
}

///////////////////////////////////////////////////

Task* TaskQueue::front()
{
    Task* pRet;
    pRet = m_Queue.front();

    return pRet;
}

///////////////////////////////////////////////////
