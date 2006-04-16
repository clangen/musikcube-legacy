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

#ifndef C_MUSIK_PLAYER_BASS_H
#define C_MUSIK_PLAYER_BASS_H

///////////////////////////////////////////////////

#include "musikCore.h"
#include "musikPlayer.h"

#if defined( USE_BASS )

#include <windows.h>
#include "musikBASSStream.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

extern OpenThreads::Mutex s_DSP_Mutex;

class MUSIKEXPORT PlayerBASS : public Player
{

public:
    
    PlayerBASS( Functor* functor, Library* library );
	~PlayerBASS();

    virtual void GetSoundDevices( StringArray& target );
	virtual void GetSoundDrivers( StringArray& target );
    virtual void GetSpectrum( float* target );
	virtual void SetBufferSize( int msecs );

    int GetFileDuration( const String& fn );

	void LoadPlugins();
	void UnloadPlugins();
	void MetaDataChanged();

	static std::vector<Stream> s_Plugins;

	HSTREAM GetCurrHandle();

protected:

	void FindPlugins( String dir, StringArray* target );
    
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
    virtual Stream* LoadCDTrack( int track );
    virtual bool IsStreamActive( Stream* pStream );

	virtual void OnEQEnable();
	virtual void OnEQDisable();
	virtual void OnEQUpdate();

	virtual Stream* LoadNetStream();
    virtual void UpdateNetstreamStatus();

    virtual int InitSoundSystem( int device, int driver, int rate );   
    virtual void DeinitSoundSystem();

	void InitEQTable( Stream* ptrStream );
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif // defined USE_BASS

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
