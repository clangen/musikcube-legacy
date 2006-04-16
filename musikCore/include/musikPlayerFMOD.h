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

#ifndef C_MUSIK_PLAYER_FMOD_H
#define C_MUSIK_PLAYER_FMOD_H

///////////////////////////////////////////////////

#include "musikCore.h"
#include "musikPlayer.h"

#if defined( USE_FMOD )

#include "fmod.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////
    
class MUSIKEXPORT Stream
{
public:
    FSOUND_STREAM* m_Stream;
    int m_Channel;
};    
    
///////////////////////////////////////////////////

class MUSIKEXPORT PlayerFMOD : public Player
{
    
	// trust the equalizer + stream metainfo callbacks and the main thread.
	static void* F_CALLBACKAPI musikEQCallback( void* originalbuffer, void *newbuffer, int length, void* param );
    static signed char F_CALLBACKAPI musikNetstreamMetaCallback( char *name, char *value, void* userdata );
    
public:
    
    PlayerFMOD( Functor* functor, Library* library );

	void SetNetStreamBuffer( int network_buffer, int prebuffer, int rebuffer );
	void SetNetStreamProxy( const String& host, const String& user, const String& pass, int port = 80 );
	virtual void GetSoundDevices( StringArray& target );
	virtual void GetSoundDrivers( StringArray& target );
    virtual void GetSpectrum( float* target );

    static int GetFileLengthMS( String& fn );

protected:
    
    // classes we need to implement
    virtual int PlayStream( Stream* ptrStream, int offset );
    virtual Stream* LoadStream( const String& filename );
    virtual int SetStreamVolume( Stream* ptrStream, int volume );
    virtual int GetStreamVolume( Stream* ptrStream );
    virtual int SetStreamPos( Stream* ptrStream, int offset );
    virtual int GetStreamPos( Stream* ptrStream );
    virtual int GetStreamDuration( Stream* ptrStream );
    virtual int StreamPause( Stream* ptrStream );
    virtual int StreamResume( Stream* ptrStream );
    virtual void OnDeleteStream( Stream* ptrStream );    
    virtual int InitEQ();
    virtual int DeinitEQ();
    virtual int StartNetstream();
    virtual void UpdateNetstreamStatus();
    virtual bool IsStreamActive( Stream* pStream ){ return true; }
        
    virtual int InitSoundSystem( int device, int driver, int rate );   
    virtual void DeinitSoundSystem();
    
	FSOUND_DSPUNIT *m_EQ_DSP;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif // defined USE_FMOD

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
