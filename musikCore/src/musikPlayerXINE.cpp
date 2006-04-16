///////////////////////////////////////////////////

#include "../include/musikPlayerXINE.h"
using namespace musikCore;

///////////////////////////////////////////////////

#if defined( USE_XINE )

///////////////////////////////////////////////////

PlayerXINE::PlayerXINE( Functor* functor, Library* library )
: Player( functor, library )
{
    m_Xine = NULL;
}

///////////////////////////////////////////////////

bool PlayerXINE::StartNetstream()
{              
    // nothing
    return false;
}

///////////////////////////////////////////////////

bool PlayerXINE::PlayStream( Stream* ptrStream, int offset )
{
    return (bool)xine_play( ptrStream->m_Stream, 0, offset );
}

///////////////////////////////////////////////////

Stream* PlayerXINE::LoadStream( const String& filename )
{
    Stream* ptrStream = new Stream();
    
    // setup audio port
    ptrStream->m_AudioPort = xine_open_audio_driver( m_Xine, NULL, NULL );
    if ( !ptrStream->m_AudioPort )
    {
        printf( "musikCore::Player::StartSound_XINE: couldn't open audio driver\n" );
        delete ptrStream;
        return NULL;
    }

    // setup physical stream
    ptrStream->m_Stream = xine_stream_new( m_Xine, ptrStream->m_AudioPort, NULL );
    if ( ptrStream->m_Stream )
        xine_open( ptrStream->m_Stream, utf16to8( filename ).c_str() );

    xine_set_param( ptrStream->m_Stream, XINE_PARAM_METRONOM_PREBUFFER, 6000 );
    xine_set_param( ptrStream->m_Stream, XINE_PARAM_IGNORE_VIDEO, 1 );
    
    return ptrStream;   
}

///////////////////////////////////////////////////

bool PlayerXINE::SetStreamVolume( Stream* ptrStream, int volume )
{
    int mod_vol = (int)( (float)volume / 2.5f );
    xine_set_param( ptrStream->m_Stream, XINE_PARAM_AUDIO_AMP_LEVEL, mod_vol );
    return true;
}

///////////////////////////////////////////////////

int PlayerXINE::GetStreamVolume( Stream* ptrStream )
{
    int volume = xine_get_param( ptrStream->m_Stream, XINE_PARAM_AUDIO_AMP_LEVEL );
    volume = (int)( (float)volume * 2.5f );
    return volume;
}

///////////////////////////////////////////////////

bool PlayerXINE::SetStreamPos( Stream* ptrStream, int offset )
{
   xine_play( ptrStream->m_Stream, 0, offset ); 
   return true;
}

///////////////////////////////////////////////////

int PlayerXINE::GetStreamPos( Stream* ptrStream )
{
    int pos = 0;
    int pos_time_ms =  0;
    int length_ms = 0;
    
    xine_get_pos_length( ptrStream->m_Stream, &pos, &pos_time_ms, &length_ms );    
    return pos_time_ms;
}

///////////////////////////////////////////////////

int PlayerXINE::GetStreamDuration( Stream* ptrStream )
{
    int pos = 0;
    int pos_time_ms =  0;
    int length_ms = 0;
    
    xine_get_pos_length( ptrStream->m_Stream, &pos, &pos_time_ms, &length_ms );    
    return length_ms;    
}

///////////////////////////////////////////////////

bool PlayerXINE::StreamPause( Stream* ptrStream )
{
    if ( ptrStream == NULL )
    {
        for ( size_t i = 0; i < m_ActiveStreams.size(); i++ )
           xine_set_param( m_ActiveStreams.at( i )->m_Stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE ); 
    }
    else
        xine_set_param( ptrStream->m_Stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE );  
    
    return true;
}

///////////////////////////////////////////////////

bool PlayerXINE::StreamResume( Stream* ptrStream )
{
    if ( ptrStream == NULL )
    {
        for ( size_t i = 0; i < m_ActiveStreams.size(); i++ )
           xine_set_param( m_ActiveStreams.at( i )->m_Stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL ); 
    }
    else
        xine_set_param( ptrStream->m_Stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL );

    return true;
}

///////////////////////////////////////////////////

void PlayerXINE::OnDeleteStream( Stream* ptrStream )
{
    xine_stop( ptrStream->m_Stream );
    xine_close( ptrStream->m_Stream );
    xine_dispose( ptrStream->m_Stream );
    xine_close_audio_driver( m_Xine, ptrStream->m_AudioPort );
    delete ptrStream;    
}

///////////////////////////////////////////////////

int PlayerXINE::InitSoundSystem( int device, int driver, int rate )
{
    m_Xine = xine_new();

    if ( !m_Xine )
    {
        printf( "musikCore::Player::StartSound_XINE: failed to initialize xine\n" );
        return -1;
    }
    
    xine_config_load( m_Xine, XINE_CONFIG );
    xine_init( m_Xine );
    
    return 1;
}

///////////////////////////////////////////////////

void PlayerXINE::UpdateNetstreamStatus()
{
	// nothing
}

///////////////////////////////////////////////////

void PlayerXINE::DeinitSoundSystem()
{
    xine_config_save( m_Xine, XINE_CONFIG );
    xine_exit( m_Xine );
    m_Xine = NULL;
}

///////////////////////////////////////////////////

#endif // USE_XINE

///////////////////////////////////////////////////
