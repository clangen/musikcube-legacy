///////////////////////////////////////////////////

#include "../include/musikPlayerFMOD.h"
using namespace musikCore;

///////////////////////////////////////////////////

#if defined( USE_FMOD )

///////////////////////////////////////////////////

void* F_CALLBACKAPI PlayerFMOD::musikEQCallback( void* originalbuffer, void *newbuffer, int length, void *param )
{
	// the param value is really the address
	// to the Equalizer that is gonna
	// be used to process the sample
	if ( param )
	{	
		Equalizer* ptrEQ = (Equalizer*)param;

		// two channel ( stereo ), 16 bit sound
		ptrEQ->ProcessDSP( newbuffer, length, 2, 16 );
	}
	return newbuffer;
}

///////////////////////////////////////////////////

signed char F_CALLBACKAPI PlayerFMOD::musikNetstreamMetaCallback( char *name, char *value, void *userdata )
{
	musikCore::String str( value );
    Player* pPlayer = (Player*)userdata;
    
    if ( !strcmp("ARTIST", name) )
    {
		if ( pPlayer )
		{
			pPlayer->m_CurrSong.SetArtist( str );
			pPlayer->TriggerOnNewSong();
		}
        return TRUE;
    }

    if (!strcmp("TITLE", name))
    {
        if ( pPlayer )
		{
			pPlayer->m_CurrSong.SetTitle( str );
			pPlayer->TriggerOnNewSong();
		}
        return TRUE;
    }

    return TRUE;
}

///////////////////////////////////////////////////

PlayerFMOD::PlayerFMOD( Functor* functor, Library* library )
: Player( functor, library )
{
    m_EQ_DSP = NULL;
}

///////////////////////////////////////////////////

void PlayerFMOD::GetSpectrum( float* target )
{
    target = FSOUND_DSP_GetSpectrum();
}

///////////////////////////////////////////////////

int PlayerFMOD::StartNetstream()
{              
    String fn = m_CurrSong.GetFilename();
    FSOUND_STREAM* pNewStream = NULL;

    // FSOUND_NONBLOCKING for netstream because the connect/buffer process might take a while.
    // From fmod sample
    //
    // streams filenames ending with ".pls" must be truncated
    if ( fn.Right( 3 ).ToLower() == "pls" )
    {
        musikCore::Filename MFN( fn );               
        pNewStream = FSOUND_Stream_Open(
            MFN.GetNetstreamPath(),
            FSOUND_NORMAL | FSOUND_NONBLOCKING, 
            0, 0 );
    }
    else
    {
        pNewStream = FSOUND_Stream_Open(
            fn.c_str(),
            FSOUND_NORMAL | FSOUND_NONBLOCKING, 
            0, 0 );                    
    }

    if ( pNewStream )
    {
        cout << "musikCore::Player: netstream established\n";

        int net_channel = -1;
        int handle = m_Handle;
        
        // waiting for connection to establish
        while ( net_channel == -1 && m_Format == MUSIK_LIBRARY_FORMAT_NET && !m_Exiting )
        {
            net_channel = FSOUND_Stream_PlayEx( FSOUND_FREE, pNewStream, NULL, 1 );
            Sleep( 50 );	

            // install the handler for metainfo - this has to be done before
            // we start playing so we can capture the first song info
            FSOUND_Stream_Net_SetMetadataCallback( pNewStream, PlayerFMOD::musikNetstreamMetaCallback, this );

            if ( handle != m_Handle )
                break;
        }

        if ( net_channel > 0 )
        {
            cout << "musikCore::Player: netstream started\n";
            
            Stream* ptrStream = new Stream();
            ptrStream->m_Stream = pNewStream;
            ptrStream->m_Channel = net_channel;
            
            m_ActiveStreams.push_back( ptrStream );

            UpdateNetstreamStatus();
            FinalizeNetstream();
            
        }
        else
        {
            FSOUND_Stream_Stop( pNewStream );
            FSOUND_Stream_Close( pNewStream );

            cout << "musikCore::Player: netstream failed\n";
        }
    }
    
    return 1;
}

///////////////////////////////////////////////////

int PlayerFMOD::PlayStream( Stream* ptrStream, int offset )
{
	ptrStream->m_Channel = FSOUND_Stream_PlayEx( FSOUND_FREE, ptrStream->m_Stream, NULL, FALSE );
    FSOUND_Stream_SetTime( ptrStream->m_Stream, offset );
    
    return ptrStream->m_Channel;
}

///////////////////////////////////////////////////

Stream* PlayerFMOD::LoadStream( const String& filename )
{
    Stream* ptrStream = new Stream();
	ptrStream->m_Stream = FSOUND_Stream_Open( 
		m_CurrSong.GetFilename().c_str(), 
		FSOUND_NORMAL | FSOUND_2D | FSOUND_MPEGACCURATE, 
		0, 0 );  

	if ( ptrStream->m_Stream )
	    return ptrStream;      

	FSOUND_Stream_Stop	( ptrStream->m_Stream );
	FSOUND_Stream_Close	( ptrStream->m_Stream ); 
	return NULL;
}

///////////////////////////////////////////////////

int PlayerFMOD::GetStreamDuration( Stream* ptrStream )
{
    return FSOUND_Stream_GetLengthMs( ptrStream->m_Stream );
}

///////////////////////////////////////////////////

int PlayerFMOD::SetStreamVolume( Stream* ptrStream, int volume )
{
    FSOUND_SetVolume( ptrStream->m_Channel, volume );
    return 0;
}

///////////////////////////////////////////////////

int PlayerFMOD::GetStreamVolume( Stream* ptrStream )
{
    return FSOUND_GetVolume( ptrStream->m_Channel );
}

///////////////////////////////////////////////////

int PlayerFMOD::SetStreamPos( Stream* ptrStream, int offset )
{
    return FSOUND_Stream_SetTime( ptrStream->m_Stream, offset );
}

///////////////////////////////////////////////////

int PlayerFMOD::GetStreamPos( Stream* ptrStream )
{
    return FSOUND_Stream_GetTime( ptrStream->m_Stream );
}

///////////////////////////////////////////////////

int PlayerFMOD::StreamPause( Stream* ptrStream )
{
    if ( ptrStream == NULL )
        FSOUND_SetPaused( FSOUND_ALL, true );
    else
        FSOUND_SetPaused( ptrStream->m_Channel, true );    
    
    return 0;
}

///////////////////////////////////////////////////

int PlayerFMOD::StreamResume( Stream* ptrStream )
{
    if ( ptrStream == NULL )
        FSOUND_SetPaused( FSOUND_ALL, false );
    else
        FSOUND_SetPaused( ptrStream->m_Channel, false );    
    
    return 0;
}

///////////////////////////////////////////////////

void PlayerFMOD::OnDeleteStream( Stream* ptrStream )
{
    FSOUND_Stream_Stop	( ptrStream->m_Stream );
	FSOUND_Stream_Close	( ptrStream->m_Stream );  

    delete ptrStream;    
}

///////////////////////////////////////////////////

int PlayerFMOD::InitEQ()
{
	DeinitEQ();

	if ( !m_EQ_DSP )
	{
		if ( !m_EQ )
			InitEqualizer();

		void* ptrEQ = m_EQ;
		m_EQ_DSP = FSOUND_DSP_Create( &musikEQCallback, FSOUND_DSP_DEFAULTPRIORITY_USER, ptrEQ );
		FSOUND_DSP_SetActive( m_EQ_DSP, true );
        return 0;
	}

	cout << "musikCore::Player::InitEQ_DSP: Equalizer failed to initialize becuase the DSP callback is already active.\n";  
    return 1;
}

///////////////////////////////////////////////////

int PlayerFMOD::DeinitEQ()
{
	if ( m_EQ_DSP )
	{
		FSOUND_DSP_Free( m_EQ_DSP );
		m_EQ_DSP = NULL;
	}   
    
    return m_EQ_DSP ? 1 : 0;
}

///////////////////////////////////////////////////

int PlayerFMOD::InitSoundSystem( int device, int driver, int rate )
{
	int nOSDriverID = -1;

#ifdef WIN32
	switch( driver )
	{
	case 0:
		nOSDriverID = FSOUND_OUTPUT_DSOUND;
		break;
	case 1:
		nOSDriverID = FSOUND_OUTPUT_WINMM;
		break;
	case 2:
		nOSDriverID = FSOUND_OUTPUT_ASIO;
		break;
	}
#else
	switch( driver )
	{
	case 0:
		nOSDriverID = FSOUND_OUTPUT_OSS;
		break;
	case 1:
		nOSDriverID = FSOUND_OUTPUT_ESD;
		break;
	case 2:
		nOSDriverID = FSOUND_OUTPUT_ALSA;
		break;
	}
#endif

	if ( nOSDriverID == -1 )
		return MUSIK_PLAYER_INIT_ERROR;

	// initialize the driver that will be used
	// for the device.
	//
	// windows supports the following:
	//   - direct sound
	//   - windows waveform
	//   - asio
	//
	// linux supports the following:
	//   - open sound system (OSS)
	//   - enlightenment sound daemon (ESD)
	//   - advanced linux sound architecture (ALSA)
	if( !FSOUND_SetOutput( nOSDriverID ) )
		return MUSIK_PLAYER_INIT_ERROR;

	// initialize the device itself.
	if ( !FSOUND_SetDriver( device ) )
		return MUSIK_PLAYER_INIT_ERROR;

	// now initialize the whole system
	FSOUND_SetBufferSize( 100 );
	if ( !FSOUND_Init( rate, 16, 0 ) )
		return MUSIK_PLAYER_INIT_ERROR;

	SetNetStreamBuffer( 64000, 60, 80 );

	// if we get here we're home free
    FSOUND_DSP_SetActive( FSOUND_DSP_GetFFTUnit(), TRUE );
	return MUSIK_PLAYER_INIT_SUCCESS;
}

///////////////////////////////////////////////////

void PlayerFMOD::SetNetStreamBuffer( int network_buffer, int prebuffer, int rebuffer )
{
	FSOUND_Stream_Net_SetBufferProperties( network_buffer, prebuffer, rebuffer);
}

///////////////////////////////////////////////////

void PlayerFMOD::SetNetStreamProxy( const String& host, const String& user, const String& pass, int port )
{
	String str;
	
	if ( user.IsEmpty() && pass.IsEmpty() )
		str.Format( "%s:%d", host.c_str(), port );
	else
		str.Format( "%s:%s@%s:%d", user.c_str(), pass.c_str(), host.c_str(), port );
	
	FSOUND_Stream_Net_SetProxy( str.c_str() );
	
	return;
}

///////////////////////////////////////////////////

void PlayerFMOD::UpdateNetstreamStatus()
{
	int read_percent = 0, status = 0, openstate, bitrate;
	unsigned int flags;
	    
	openstate = FSOUND_Stream_GetOpenState( GetCurrStream()->m_Stream );        
	FSOUND_Stream_Net_GetStatus( GetCurrStream()->m_Stream, &status, &read_percent, &bitrate, &flags);

	m_CurrSong.SetBitrate( IntToString( bitrate ) );
	m_CurrSong.SetFilesize( String( _T("-1") ) );
	
}

///////////////////////////////////////////////////

void PlayerFMOD::DeinitSoundSystem()
{
	FSOUND_Close();
}

///////////////////////////////////////////////////

void PlayerFMOD::GetSoundDevices( StringArray& target )
{
	target.clear();

	int num_drivers = FSOUND_GetNumDrivers();

	for ( int i = 0; i < num_drivers; i++ )
		target.push_back( FSOUND_GetDriverName( i ) );

	if ( !target.size() )
		target.push_back( _T( "(no devices found)" ) );
}

///////////////////////////////////////////////////

void PlayerFMOD::GetSoundDrivers( StringArray& target )
{
#ifdef WIN32
	target.push_back( _T( "Direct Sound" ) );
	target.push_back( _T( "Windows Waveform" ) );
	target.push_back( _T( "ASIO" ) );
#else
	target.push_back( _T( "OSS" ) );
	target.push_back( _T( "ESD" ) );
	target.push_back( _T( "ALSA" ) );
#endif
}

///////////////////////////////////////////////////

#endif // USE_FMOD

///////////////////////////////////////////////////
