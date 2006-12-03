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
#include "../include/musikCore.h"
#include "../include/musikBASSStream.h"

///////////////////////////////////////////////////

#if defined(WIN32) || defined(__MINGW32__) 
#if defined(USE_BASS)

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

Stream::Stream(const Plugin* plugin, const HSTREAM streamID)
{
    this->m_Plugin = plugin;
    this->m_StreamID = streamID;
}

///////////////////////////////////////////////////

Stream::~Stream()
{
    if (this->IsActive())
    {
        this->m_Plugin->Stop(m_StreamID);
    }
}

///////////////////////////////////////////////////

bool Stream::Play(int offset)
{
    return this->m_Plugin->Play(this->m_StreamID, offset);
}

///////////////////////////////////////////////////

bool Stream::CanSeek()
{
    return this->m_Plugin->CanSeek();
}

///////////////////////////////////////////////////

bool Stream::Seek(int ms)
{
    return this->m_Plugin->Seek(this->m_StreamID, ms);
}

///////////////////////////////////////////////////

bool Stream::Pause()
{
    return this->m_Plugin->Pause(this->m_StreamID);
}

///////////////////////////////////////////////////

bool Stream::Resume()
{
    return this->m_Plugin->Resume(this->m_StreamID);
}

///////////////////////////////////////////////////

bool Stream::Stop()
{
    return this->m_Plugin->Stop(this->m_StreamID);
}

///////////////////////////////////////////////////

int Stream::GetDuration()
{
    return this->m_Plugin->GetDuration(this->m_StreamID);
}

///////////////////////////////////////////////////

int Stream::GetTime()
{
    return this->m_Plugin->GetTime(this->m_StreamID);
}

///////////////////////////////////////////////////

bool Stream::IsActive()
{
    return this->m_Plugin->IsActive(this->m_StreamID);
}

///////////////////////////////////////////////////

#endif  // USE_BASS
#endif    // WIN32  


///////////////////////////////////////////////////
