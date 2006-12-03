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

#ifndef C_MUSIK_PLAYER_BASS_H
#define C_MUSIK_PLAYER_BASS_H

///////////////////////////////////////////////////

#include "musikCore.h"
#include "musikPlayer.h"

#if defined(USE_BASS)

#include <windows.h>

#include "musikBASSPlugin.h"
#include "musikBASSStream.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

class MUSIKEXPORT PlayerBASS : public Player
{

public:
    
    PlayerBASS(Functor* functor, Library* library);
    ~PlayerBASS();

    virtual void GetSoundDevices(StringArray& target);
    virtual void GetSoundDrivers(StringArray& target);
    virtual void GetSpectrum(float* target);
    virtual void SetBufferSize(int msecs);

    int GetFileDuration(const String& fn);

    void LoadPlugins();
    void UnloadPlugins();
    void MetaDataChanged();

    static std::vector<Plugin*> s_Plugins;

    HSTREAM GetCurrHandle();

protected:

    void FindPlugins(String dir, StringArray* target);

    // classes we need to implement
    virtual bool PlayStream(Stream* ptrStream, int offset);
    virtual Stream* LoadStream(const String& filename);
    virtual bool SetStreamVolume(Stream* ptrStream, int volume);
    virtual int GetStreamVolume(Stream* ptrStream);
    virtual bool SetStreamPos(Stream* ptrStream, int offset);
    virtual int GetStreamPos(Stream* ptrStream);
    virtual int GetStreamDuration(Stream* ptrStream);
    virtual bool StreamPause(Stream* ptrStream);
    virtual bool StreamResume(Stream* ptrStream);
    virtual void OnDeleteStream(Stream* ptrStream);
    virtual Stream* LoadCDTrack(int track);
    virtual bool IsStreamActive(Stream* pStream);

    virtual void OnEQEnable();
    virtual void OnEQDisable();
    virtual void OnEQUpdate();

    virtual Stream* LoadNetStream();
    virtual void UpdateNetstreamStatus();

    virtual int InitSoundSystem(int device, int driver, int rate);
    virtual void DeinitSoundSystem();

    void InitEQTable(Stream* ptrStream);
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif // defined USE_BASS

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
