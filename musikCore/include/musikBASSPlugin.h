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

#ifndef __C_MUSIK_BASS_PLUGIN_H__
#define __C_MUSIK_BASS_PLUGIN_H__

///////////////////////////////////////////////////

#if defined(USE_BASS)

#include "musikCore.h"
#include "bass.h"

namespace musikCore {

///////////////////////////////////////////////////

class MUSIKEXPORT Plugin
{
public:

    typedef HSTREAM (*LOADFILE)(const String& file);
    typedef bool (*PLAY)(HSTREAM stream, int offset);
    typedef bool (*CANSEEK)();
    typedef bool (*SEEK)(HSTREAM stream, int ms);
    typedef bool (*PAUSE)(HSTREAM stream);
    typedef bool (*RESUME)(HSTREAM stream);
    typedef bool (*STOP)(HSTREAM stream);
    typedef int (*GETDURATION)(HSTREAM stream);
    typedef int (*GETTIME)(HSTREAM stream);
    typedef bool (*ISACTIVE)(HSTREAM stream);
    typedef bool (*CANPLAY)(const String& file);
    typedef bool (*WRITETAG)(const SongInfo& info);
    typedef bool (*READTAG)(const String& fn, SongInfo& target);
    typedef void (*UNLOAD)();

    typedef const String& (*GETPLUGINFORMATS)();
    typedef const String& (*GETPLUGINNAME)();
    typedef const String& (*GETPLUGINDESCRIPTION)();

public:

    Plugin(const String& dllFilename);
    ~Plugin();

    bool IsValid(){ return m_IsValid; }

    LOADFILE LoadFile;
    PLAY Play;
    CANSEEK CanSeek;
    SEEK Seek;
    PAUSE Pause;
    RESUME Resume;
    STOP Stop;
    GETDURATION GetDuration;
    GETTIME GetTime;
    ISACTIVE IsActive;
    CANPLAY CanPlay;
    WRITETAG WriteTag;
    READTAG ReadTag;
    GETPLUGINFORMATS GetPluginFormats;
    GETPLUGINNAME GetPluginName;
    GETPLUGINDESCRIPTION GetPluginDescription;


private:

    bool m_IsValid;
    UNLOAD Unload;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif  // BASS

///////////////////////////////////////////////////

#endif  // __C_MUSIK_BASS_PLUGIN_H__

///////////////////////////////////////////////////
