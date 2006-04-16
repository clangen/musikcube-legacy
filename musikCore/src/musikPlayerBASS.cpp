	
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikPlayerBASS.h"

using namespace musikCore;

///////////////////////////////////////////////////

#if defined( USE_BASS )

std::vector<Stream> PlayerBASS::s_Plugins = std::vector<Stream>();
OpenThreads::Mutex musikCore::s_DSP_Mutex;

///////////////////////////////////////////////////

namespace musikCore 
{
	static void CALLBACK BASS_EQ_Callback( HDSP handle, DWORD channel, void *buffer, DWORD length, DWORD user )
	{
		Stream* stream = (Stream*)user;
		if ( stream )
		{
			BASS_CHANNELINFO info;
			BASS_ChannelGetInfo( channel, &info );
			float seconds = BASS_ChannelBytes2Seconds( channel, length );	// number of seconds this buffer is
			float total_samples = seconds * (float)info.freq;
			
			char*buf = (char*)buffer;
			stream->m_Shibatch.equ_modifySamples( (char*)buffer, (int)total_samples, info.chans, 16 );
		}
	}
}

///////////////////////////////////////////////////

// returns true if the metadata gotten from the stream actually made anything change
bool GetMetaData( char* meta_tags, PlayerBASS* player, HSTREAM channel )
{
	if ( !player )
		return false;

	// store the previous values
	musikCore::String prev_artist = player->GetPlaying()->GetArtist();
	musikCore::String prev_album = player->GetPlaying()->GetAlbum();
	musikCore::String prev_title = player->GetPlaying()->GetTitle();

	// its a good idea to check for ogg tags first.
	char* tags = BASS_StreamGetTags( channel, BASS_TAG_OGG );
	if ( tags )
	{
		bool artist = false, album = false, title = false;
		while ( *tags ) 
		{
			musikCore::String tag = tags;
			if ( tag.Left( 7 ).ToLower() == _T( "artist=" ) )
			{
				player->GetPlaying()->SetArtist( tag.Right( tag.GetLength() - 7 ) );
				artist = true;
			}
			else if ( tag.Left( 6 ).ToLower() == _T( "title=" ) )
			{
				player->GetPlaying()->SetTitle( tag.Right( tag.GetLength() - 6 ) );
				title = true;
			}
			else if ( tag.Left( 6 ).ToLower() == _T( "album=" ) )
			{
				player->GetPlaying()->SetAlbum( tag.Right( tag.GetLength() - 6 ) );
				album = true;
			}

			tags += strlen( tags ) + 1 ; // move on to next tag
		}

		if ( !artist )
			player->GetPlaying()->SetArtist( _T( "netstream" ) );
		if ( !album )
			player->GetPlaying()->SetAlbum( _T( "" ) );
		if ( !title )
			player->GetPlaying()->SetArtist( player->GetPlaying()->GetFilename() );
	}
	else if ( meta_tags )
	{
		bool title = false, artist = false;
		while ( *meta_tags ) 
		{
			musikCore::String tag = meta_tags;
			if ( tag.Left( 13 ).ToLower() == _T( "streamtitle='" ) )
			{
				int pos = tag.Find( wchar_t(';' ) );
				tag = tag.Left( pos - 1 );
				tag = tag.Right( tag.GetLength() - 13 );
				player->GetPlaying()->SetTitle( tag );
				title = true;
			}
			else if ( tag.Left( 9 ).ToLower() == _T( "icy-name:" ) )
			{
				if ( title )
				{
					artist = true;
					player->GetPlaying()->SetArtist( tag.Right( tag.GetLength() - 9 ) );
				}
				else
				{
					title = true;
					player->GetPlaying()->SetTitle( tag.Right( tag.GetLength() - 9 ) );
				}
			}

			meta_tags += strlen( meta_tags ) + 1; // move on to next tag
		}	

		player->GetPlaying()->SetAlbum( _T( "" ) );
		if ( !artist )
			player->GetPlaying()->SetArtist( _T( "" ) );
		if ( !title )
			player->GetPlaying()->SetTitle( player->GetPlaying()->GetFilename() );
	}

	// check to see if the strings actually changed
	if ( !prev_title.Equals(player->GetPlaying()->GetTitle()) )
		return true;
	if ( !prev_artist.Equals(player->GetPlaying()->GetArtist()) )
		return true;
	if ( !prev_album.Equals(player->GetPlaying()->GetAlbum()) )
		return true;

	return false;
}

///////////////////////////////////////////////////

void CALLBACK BASS_NetStream_Tag( HSYNC handle, DWORD channel, DWORD data, DWORD user )
{
	PlayerBASS* player = (PlayerBASS*)user;

	if ( !user )
		return;

	if ( GetMetaData((char*)data, player, channel) ) 
		player->MetaDataChanged();
}

///////////////////////////////////////////////////

PlayerBASS::PlayerBASS( Functor* functor, Library* library )
: Player( functor, library )
{
	LoadPlugins();

	// create a static list of known formats
	for ( size_t i = 0; i < s_Plugins.size(); i++ )
	{
		String formats = s_Plugins.at( i ).GetPluginFormats();
		StringArray target;
		Filename::DelimitStr( formats, _T( "," ), target, false, true, true );

		for ( size_t j = 0; j < target.size(); j++ )
			musikCore::g_PlayerFormats.push_back( target.at( j ).ToLower() );
	}
}

///////////////////////////////////////////////////

PlayerBASS::~PlayerBASS()
{
	UnloadPlugins();
}

///////////////////////////////////////////////////

HSTREAM PlayerBASS::GetCurrHandle()
{
	if ( GetCurrStream() )
	{
		return GetCurrStream()->m_Stream;
	}

	return 0;
}

///////////////////////////////////////////////////

void PlayerBASS::GetSpectrum( float* target )
{
    Stream* ptrStr = GetCurrStream();
    if ( ptrStr )
    {
        BASS_ChannelGetData(
            ptrStr->GetStream(),
            (void*)target,
            BASS_DATA_FFT1024 );
    }
}

///////////////////////////////////////////////////

Stream* PlayerBASS::LoadNetStream()
{
	Stream* pNewStream = new Stream();
	for ( size_t i = 0; i < s_Plugins.size(); i++ )
	{
		if ( s_Plugins.at( i ).CanPlay( m_CurrSong.GetFilename() ) )
		{
			*pNewStream = s_Plugins.at( i );		// copy the stream. this basically makes
													// a new stream with all the correct
													// addresses for the right dll

			pNewStream->m_Stream = pNewStream->LoadFile( m_CurrSong.GetFilename() );

			if ( pNewStream->m_Stream )
			{
				GetMetaData( 
					BASS_StreamGetTags( 
						pNewStream->m_Stream, 
						BASS_SYNC_META 
					),
					this,
					pNewStream->m_Stream
				);

				BASS_ChannelSetSync(
					pNewStream->m_Stream,
					BASS_SYNC_META,
					0,
					(SYNCPROC*)&BASS_NetStream_Tag,
					(DWORD)this );

				return pNewStream;
			}
		}
	}

	// not loaded
	delete pNewStream;
	return NULL;
}

///////////////////////////////////////////////////

bool PlayerBASS::PlayStream( Stream* ptrStream, int offset )
{
	if ( !ptrStream )
		return false;

	return ptrStream->Play( ptrStream->m_Stream, offset );
}

///////////////////////////////////////////////////

Stream* PlayerBASS::LoadStream( const String& filename )
{
	Stream* pNewStream = new Stream();
	for ( size_t i = 0; i < s_Plugins.size(); i++ )
	{
		if ( s_Plugins.at( i ).CanPlay( filename ) )
		{
			*pNewStream = s_Plugins.at( i );		// copy the stream. this basically makes
													// a new stream with all the correct
													// addresses for the right dll

			pNewStream->m_Stream = pNewStream->LoadFile( filename );

			if ( pNewStream->m_Stream )
			{
				if ( IsEqualizerEnabled() )
				{
					pNewStream->m_Shibatch.equ_init( 14 );	// 14 is the magic number
					InitEQTable( pNewStream );
					pNewStream->m_DSP = BASS_ChannelSetDSP(
						pNewStream->m_Stream,
						(DSPPROC*)musikCore::BASS_EQ_Callback,
						(DWORD)pNewStream,
						0 );
				}

				return pNewStream;
			}
		}
	}

	// not loaded
	delete pNewStream;
	return NULL;
}

///////////////////////////////////////////////////

void PlayerBASS::InitEQTable( Stream* ptrStream )
{
	float curr_preamp = musikCore::GetEQPreampLevel();
	float preamp = ( 96 - curr_preamp ) / 96;

	float cleft, cright;
	float left[18], right[18];
	for( int i = 0; i < 18; i++ )
	{
		cleft = m_EQSettings.m_Left[i];
		cright = m_EQSettings.m_Right[i];
		left[i] = preamp * ( ( 96 - cleft ) / 96 ) * 1.58;
		right[i] = preamp * ( ( 96 - cright ) / 96 ) * 1.58;
	}

	ptrStream->m_Shibatch.equ_makeTable( left, right, &ptrStream->m_Paramlist, m_Rate );
}

///////////////////////////////////////////////////

void PlayerBASS::OnEQEnable()
{
	if ( IsPlaying() )
	{
		GetCurrStream()->m_Shibatch.equ_init( 14 );	// 14 is the magic number
		InitEQTable( GetCurrStream() );
		GetCurrStream()->m_DSP = BASS_ChannelSetDSP(
			GetCurrStream()->m_Stream,
			(DSPPROC*)musikCore::BASS_EQ_Callback,
			(DWORD)GetCurrStream(),
			0 );

	}
}

///////////////////////////////////////////////////

void PlayerBASS::OnEQUpdate()
{
	for ( size_t i = 0; i < m_ActiveStreams.size();i ++ )
		InitEQTable( m_ActiveStreams.at( i ) );
}

///////////////////////////////////////////////////

void PlayerBASS::OnEQDisable()
{
	if ( IsPlaying() )
	{
		for ( size_t i = 0; i < m_ActiveStreams.size(); i++ )
		{
			if ( m_ActiveStreams.at( i )->m_DSP )
			{
				BASS_ChannelRemoveDSP(
					m_ActiveStreams.at( i )->m_Stream,
					m_ActiveStreams.at( i )->m_DSP );
				m_ActiveStreams.at( i )->m_Shibatch.equ_quit();
				m_ActiveStreams.at( i )->m_DSP = 0;
			}
		}
	}
}

///////////////////////////////////////////////////

int PlayerBASS::GetStreamDuration( Stream* ptrStream )
{
	if ( !ptrStream )
		return 0;

	return ptrStream->GetDuration( ptrStream->m_Stream );
}

///////////////////////////////////////////////////

bool PlayerBASS::SetStreamVolume( Stream* ptrStream, int volume )
{
    BASS_ChannelSetAttributes( 
        ptrStream->GetStream(),
        -1,
        (int)( (float)volume / 2.5f ),
        -101 );

    return true;
}
 
///////////////////////////////////////////////////

int PlayerBASS::GetStreamVolume( Stream* ptrStream )
{
    DWORD volume;
    BASS_ChannelGetAttributes(
        ptrStream->GetStream(),
        NULL,
        &volume,
        NULL );

    return (int)( (float)volume * 2.5f );
}

///////////////////////////////////////////////////

bool PlayerBASS::SetStreamPos( Stream* ptrStream, int offset )
{
	if ( !ptrStream || !ptrStream->CanSeek() )
		return false;

	return ptrStream->Seek( ptrStream->m_Stream, offset );
}

///////////////////////////////////////////////////

int PlayerBASS::GetStreamPos( Stream* ptrStream )
{
	if ( !ptrStream )
		return 0;

	return ptrStream->GetTime( ptrStream->m_Stream );
}

///////////////////////////////////////////////////

bool PlayerBASS::StreamPause( Stream* ptrStream )
{
    if ( !ptrStream )
	{
		for ( size_t i = 0; i < m_ActiveStreams.size(); i++ )
			m_ActiveStreams.at( i )->Pause( m_ActiveStreams.at( i )->m_Stream );
	}
	else
		return ptrStream->Pause( ptrStream->m_Stream );

    return true;
}

///////////////////////////////////////////////////

bool PlayerBASS::StreamResume( Stream* ptrStream )
{
    if ( !ptrStream )
		if ( GetCurrStream() )
			return GetCurrStream()->Resume( GetCurrStream()->m_Stream );

	return ptrStream->Resume( ptrStream->m_Stream );
}

///////////////////////////////////////////////////

void PlayerBASS::OnDeleteStream( Stream* ptrStream )
{
	if ( ptrStream->m_DSP )
	{
		BASS_ChannelRemoveDSP( ptrStream->m_Stream, ptrStream->m_DSP );
		ptrStream->m_Shibatch.equ_quit();
		ptrStream->m_DSP = 0;
	}
	ptrStream->Stop( ptrStream->m_Stream );

    delete ptrStream;
}

///////////////////////////////////////////////////

int PlayerBASS::InitSoundSystem( int device, int driver, int rate )
{
    driver += 1;

    if ( BASS_Init( driver, rate, 0, NULL, NULL ) )
	    return MUSIK_PLAYER_INIT_SUCCESS;

    return MUSIK_PLAYER_INIT_ERROR;
}

///////////////////////////////////////////////////

void PlayerBASS::UpdateNetstreamStatus()
{

}

///////////////////////////////////////////////////

void PlayerBASS::DeinitSoundSystem()
{
    BASS_Free();
}

///////////////////////////////////////////////////

void PlayerBASS::GetSoundDevices( StringArray& target )
{
}

///////////////////////////////////////////////////

void PlayerBASS::GetSoundDrivers( StringArray& target )
{
    int count = 1;
    char* ptrDevice = BASS_GetDeviceDescription( 0 );
    
    while ( ptrDevice )
    {
        ptrDevice = BASS_GetDeviceDescription( count );
        if ( ptrDevice )
            target.push_back( ptrDevice );
        count++;
    }
}

///////////////////////////////////////////////////

Stream* PlayerBASS::LoadCDTrack( int track )
{
	String info;
	info.Format( 
		_T( "cdda, %d, %d" ) ,
		m_DiscDrive,
		track );					// this is how we pass to our internal plugin
									// core_Internal.dll will decompose the string
									// and load the given track from the specified drive

	Stream* pNewStream = new Stream();
	for ( size_t i = 0; i < s_Plugins.size(); i++ )
	{
		if ( s_Plugins.at( i ).CanPlay( info ) )
		{
			*pNewStream = s_Plugins.at( i );		// copy the stream. this basically makes
													// a new stream with all the correct
													// addresses for the right dll

			pNewStream->m_Stream = pNewStream->LoadFile( info );
			if ( pNewStream->m_Stream )
				return pNewStream;	
		}
	}

	// not loaded
	delete pNewStream;
	return NULL;
}

///////////////////////////////////////////////////

bool PlayerBASS::IsStreamActive( Stream* pStream )
{
    if ( !pStream )
        return false;

	return pStream->IsActive( pStream->m_Stream );
}   

///////////////////////////////////////////////////

// a static function to use the audio decoder
// to retrieve the song's length

int PlayerBASS::GetFileDuration( const String& filename )
{
    Stream* str = LoadStream( filename );
	int secs = str->GetDuration( str->m_Stream );
	delete str;

	return secs;
}

///////////////////////////////////////////////////

void PlayerBASS::SetBufferSize( int msecs )
{
	BASS_SetConfig(
		BASS_CONFIG_BUFFER,
		(DWORD)msecs );
}

///////////////////////////////////////////////////

void PlayerBASS::LoadPlugins()
{
	wchar_t buffer[2000];
	GetModuleFileName( NULL, buffer, sizeof( buffer ) );
	String path = buffer;

	path = path.Left( path.ReverseFind( '\\' ) );	// remove filename, we only want the directory
	path += _T( "\\formats\\*.*" );
	
	StringArray dllfiles;
	FindPlugins( path, &dllfiles );

	for( size_t i = 0; i < dllfiles.size(); i++ )
	{
		Stream curr( dllfiles.at( i ) );
		if ( curr.IsLoaded() )
			s_Plugins.push_back( curr );
	}
}

///////////////////////////////////////////////////

void PlayerBASS::UnloadPlugins()
{
	for( size_t i = 0; i < s_Plugins.size(); i++ )
		s_Plugins.at( i ).Unload();

	s_Plugins.clear();
}

///////////////////////////////////////////////////

void PlayerBASS::FindPlugins( String dir, StringArray* target )
{
	// stopping conditions
	if ( dir.IsEmpty() || !target )
		return;

	// find the first file in the directory...
	WIN32_FIND_DATA lpFindFileData;
	HANDLE hDir = FindFirstFile( dir.c_str(), &lpFindFileData );

	// assure the first file exists...
	if ( hDir != INVALID_HANDLE_VALUE )
	{
		musikCore::String fn, temp;
		musikCore::Filename MFN;
		do
		{
			if ( lpFindFileData.cFileName[0] != '.' )
			{
				// file is a directory
				if ( lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					fn = dir.Left( dir.GetLength() - 3 );	// remove *.* from full path
					fn += lpFindFileData.cFileName;			// append to make new path

					if ( fn.Left( 0 ) != _T( "\\" ) )
						fn += _T( "\\*.*" );					// add \\*.* to the end of the new path
					else
						fn += _T( "*.*" );						// add *.* to the end of the new path

					FindPlugins( fn, target );
				}

				// file is a file...
				else
				{
					fn = dir.Left( dir.GetLength() - 3 );	// remove *.* from full path...
					fn += lpFindFileData.cFileName;			// append filename

					MFN.SetFilename( fn );
					if ( MFN.GetExtension() == _T( "dll" ) )
						target->push_back( fn );
				}
			}

		}
		while ( FindNextFile( hDir, &lpFindFileData ) );

		FindClose( hDir );
	}
}

///////////////////////////////////////////////////

void PlayerBASS::MetaDataChanged()
{
	m_Functor->OnNewSong();	
}

///////////////////////////////////////////////////

#endif // USE_BASS

///////////////////////////////////////////////////
