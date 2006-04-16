///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://www.musikcube.com
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2002-2004
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey at musikcube dot com
//   Dustin Carter  : Developer
//   Simon Windmill : Developer
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_PLAYER_XINE_H
#define C_MUSIK_PLAYER_XINE_H

///////////////////////////////////////////////////

#include "musikCore.h"
#include "musikPlayer.h"

///////////////////////////////////////////////////

#if defined( USE_XINE )

#include <xine.h>
#define XINE_CONFIG "~/.musikproject/xine.conf"

///////////////////////////////////////////////////

namespace musikCore {
    
///////////////////////////////////////////////////    
    
class Stream
{
public:
    xine_stream_t* m_Stream;
    xine_audio_port_t* m_AudioPort; 
};

///////////////////////////////////////////////////

class PlayerXINE : public Player
{
public:
    
    PlayerXINE( Functor* functor, Library* library );

protected:
    
    // classes we need to implement
    virtual bool PlayStream( Stream* ptrStream, int offset );
    virtual Stream* LoadStream( const String& filename );
    virtual bool SetStreamVolume( Stream* ptrStream, int volume );
    virtual int GetStreamVolume( Stream* ptrStream );
    virtual bool SetStreamPos( Stream* ptrStream, int offset );
    virtual int GetStreamPos( Stream* ptrStream );
    virtual int GetStreamDuration( Stream* ptrStream );
    virtual bool StreamPause( Stream* ptrStream );
    virtual bool StreamResume( Stream* ptrStream );
    virtual void OnDeleteStream( Stream* ptrStream );    
    virtual bool StartNetstream();
    virtual void UpdateNetstreamStatus();
    virtual bool IsStreamActive( Stream* pStream ){ return true; }

    virtual int InitSoundSystem( int device, int driver, int rate );   
    virtual void DeinitSoundSystem();

    virtual void OnEQEnable(){}
    virtual void OnEQDisable(){}
    virtual void OnEQUpdate(){}
    
    virtual void GetSoundDevices( StringArray& target ){ }
    virtual void GetSoundDrivers( StringArray& target ){ }
    
    xine_t* m_Xine;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif // USE_XINE

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
