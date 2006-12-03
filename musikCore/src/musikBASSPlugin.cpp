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
#include "../include/musikBASSPlugin.h"

///////////////////////////////////////////////////

#if defined(WIN32) || defined(__MINGW32__) 
#if defined(USE_BASS)

///////////////////////////////////////////////////

musikCore::Plugin::Plugin(const String& dllFilename)
:   LoadFile(0),
    Play(0),
    CanSeek(0),
    Seek(0),
    Pause(0),
    Resume(0),
    Stop(0),
    GetDuration(0),
    GetTime(0),
    IsActive(0),
    CanPlay(0),
    ReadTag(0),
    WriteTag(0),
    Unload(0),
    GetPluginFormats(0),
    GetPluginName(0),
    GetPluginDescription(0)
{
    m_IsValid = false;

    HMODULE mod = LoadLibrary(dllFilename);

    if (mod != NULL)
    {
        LoadFile = (LOADFILE)GetProcAddress(mod, "LoadFile");
        Play = (PLAY)GetProcAddress(mod, "Play");
        CanSeek = (CANSEEK)GetProcAddress(mod, "CanSeek");
        Seek = (SEEK)GetProcAddress(mod, "Seek");
        Pause = (PAUSE)GetProcAddress(mod, "Pause");
        Resume = (RESUME)GetProcAddress(mod, "Resume");
        Stop = (STOP)GetProcAddress(mod, "Stop");
        GetDuration = (GETDURATION)GetProcAddress(mod, "GetDuration");
        GetTime = (GETTIME)GetProcAddress(mod, "GetTime");
        IsActive = (ISACTIVE)GetProcAddress(mod, "IsActive");
        CanPlay = (CANPLAY)GetProcAddress(mod, "CanPlay");
        ReadTag = (READTAG)GetProcAddress(mod, "ReadTag");
        WriteTag = (WRITETAG)GetProcAddress(mod, "WriteTag");
        Unload = (UNLOAD)GetProcAddress(mod, "Unload");

        GetPluginFormats = (GETPLUGINFORMATS)GetProcAddress(mod, "GetPluginFormats");
        GetPluginName = (GETPLUGINNAME)GetProcAddress(mod, "GetPluginName");
        GetPluginDescription = (GETPLUGINDESCRIPTION)GetProcAddress(mod, "GetPluginDescription");

        if (LoadFile && Play && CanSeek && Seek && Pause && Resume && Stop &&
            GetDuration && GetTime && IsActive && CanPlay && GetPluginFormats && 
            GetPluginName && GetPluginDescription && ReadTag && WriteTag && Unload)
        {
            m_IsValid = true;
        }
    }
}

///////////////////////////////////////////////////

musikCore::Plugin::~Plugin()
{
    if (this->Unload != NULL)
    {
        this->Unload();
    }
}

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////