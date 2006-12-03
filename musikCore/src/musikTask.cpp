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
#include <iostream>
#include "../include/musikTask.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

Task::~Task()
{
}

///////////////////////////////////////////////////

Task::Task()
{
    m_Stop = false;
    m_Finished = false;
    m_Active = false;
    m_Progress = 0;
}

///////////////////////////////////////////////////

void Task::Start()
{
    start();
}

///////////////////////////////////////////////////

bool Task::StopWait(int timeout_secs)
{
    if (m_Active)
    {
        m_Stop = true;
        int timeout = 0;
        while (!m_Finished)
        {
            timeout++;
            Sleep(100);

            if (timeout * 100 >= (timeout_secs * 1000))
            {
                cout << "musikCore::Task: I appear to be deadlocked. Terminating\n";
                m_Finished = true;
                m_Active = false;
                return false;
            }
        }
    }

    m_Finished = true;
    m_Active = false;
    return true;
}

///////////////////////////////////////////////////
