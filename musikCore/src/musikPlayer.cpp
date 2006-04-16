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

#include "stdafx.h"
#include <math.h>

#include "../include/musikPlayer.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

#ifdef WIN32
#pragma warning (disable : 4312)		// conversion of int to * of greater size
#endif

///////////////////////////////////////////////////

StringArray musikCore::g_PlayerFormats = StringArray();
const StringArray& musikCore::GetPlayerFormats(){ return g_PlayerFormats; }

#define round(x) floor( x + 0.5f )

////////////////////////////////////////////////////

Player::Player( Functor* functor, Library* library )
{
	m_Functor			= functor;
	m_Library			= library;

	m_Playlist			= new Playlist();
	m_Playlist->SetType( MUSIK_PLAYLIST_TYPE_NOWPLAYING );

	m_IsPlaying			= false;
	m_IsPaused			= false;
	m_IsCrossfaderReady	= false;
	m_CrossfaderEnabled	= false;
	m_IsEQActive		= false;
	m_Exiting			= false;
	m_Finished			= true;
	m_Active			= false;

	m_Crossfader		= NULL;

	m_Handle			= 0;
    m_Duration          = 0;
    m_DiscDrive         = 0;
	m_Index				= -1;
	m_FadeType			= -1;
	m_Rate				= 0;

	m_Volume			= 128;

	m_Playmode			= 0L;
    m_PlayerType        = MUSIK_PLAYER_NORMAL;
	m_State				= MUSIK_PLAYER_INIT_UNINITIALIZED;

	InitEQSettings();
}

///////////////////////////////////////////////////

Player::~Player()
{      
    DeinitEQ();
	DeinitCrossfader();
	DeinitPlaylist();
    
    StopSound();
}

///////////////////////////////////////////////////

void Player::StopWait()
{
	if ( m_Active )
	{
		m_Stop = true;

		while ( !m_Finished )
			Sleep( 50 );

		m_Active = false;
	}
}

///////////////////////////////////////////////////

void Player::AbortCrossfade( bool wait )
{
	if ( !IsCrossfadeActive() )
		return;

	m_AbortCrossfade = true;
/*
	if ( !wait )
		return;

	while ( m_CrossfadeActive )
		Sleep( 100 );
*/
}	

///////////////////////////////////////////////////

void Player::thread_CheckNoCrossfadeStatus()
{
    // intro play mode means we only play 
	// the first 10 seconds of every song...
	if ( GetPlaymode() & MUSIK_PLAYER_PLAYMODE_INTRO )
	{
		t_TimeElapsed = GetTimeNow( MUSIK_TIME_MSECS );

		if ( t_TimeElapsed >= 10000 )
			Next( true );

        return;
	}

	// not intro mode, so check to see if its
	// time to que up the next song...
	else if ( m_Format != MUSIK_LIBRARY_FORMAT_NET )
	{
		t_TimeRemain = GetTimeRemain( MUSIK_TIME_MSECS );

		if ( t_TimeRemain <= 2000 )
		{
			bool started = false;
			while ( !started )
			{               
				// when its time for the next song
				// to fade in, send the player the signal
				// and exit the tighter loop. go along our
				// mary way until the it sends the begin 
				// crossfade flag back.
                t_TimeRemain = GetTimeRemain( MUSIK_TIME_MSECS );
				if ( t_TimeRemain <= 100 || !IsStreamActive( GetCurrStream() ) )
				{
					Next( true );
					started = true;
				}
                
				Sleep( 100 );
			}
		}
    }
}

///////////////////////////////////////////////////

void Player::thread_CheckCrossfadeStatus()
{
    if ( m_IsPlaying && !m_IsPaused )
	{
        // intro play mode means we only play 
		// the first 10 seconds of every song...
        m_ThreadMutex.lock();
		if ( GetPlaymode() & MUSIK_PLAYER_PLAYMODE_INTRO )
		{
            t_TimeElapsed = GetTimeNow( MUSIK_TIME_MSECS );

			if ( t_TimeElapsed >= 10000 )
				Next( true );
		}

		// see if its time to crossfade in the
		// next song
		else if ( m_Format != MUSIK_LIBRARY_FORMAT_NET )
		{                    
			t_TimeRemain = GetTimeRemain( MUSIK_TIME_MSECS );
			t_TimeFade = (int)( GetCrossfader()->GetDuration( MUSIK_CROSSFADER_NEW_SONG ) * 1000.0f );
#if defined(USE_BASS)
			if ( m_PlayerType == MUSIK_PLAYER_CDROM )
            {
                if ( t_TimeRemain <= t_TimeFade )
				    Next( true );
            }
            else
#endif
			if ( t_TimeRemain <= t_TimeFade )
				Next( true );
			else if ( !IsStreamActive( GetCurrStream() ) )
				Next( true );
		}
        m_ThreadMutex.unlock();
    }
}

///////////////////////////////////////////////////

void Player::thread_Crossfade()
{
	cout << "musikCore::Player: Crossfade started\n";

	UnflagCrossfade();

	bool fade_success = true;
	
	// if the song is shorter than twice the fade
	// duration ( fadein + fadeout ), don't do
	// the fade becuase something will fuck up
	if ( GetDuration( MUSIK_TIME_MSECS ) > ( GetCrossfader()->GetDuration( GetFadeType() ) * 1000 ) )
	{
		// fade duration in seconds, not milliseconds 
		int nDuration = (int)( 1000.0f * GetCrossfader()->GetDuration( GetFadeType() ) );

		// total number of steps ( 10 fades per second )
		// with a minimum of 1 step
		float fFadeSecs = (float)nDuration / 1000.0f;
		float fFadeCount = 10.0f * fFadeSecs;
		int nFadeCount = (int) fFadeCount;

		if ( nFadeCount < 1 ) nFadeCount = 1;

		// variables
		float ftemp = 0.0f;
		int ntemp = 0;

		int nFadeType = GetFadeType();
		int nCurrHandle	= GetHandle();
		int nMainStream	= -1;

		size_t nChildCount		= 0;
		IntArray aChildSteps;

		int nFadeStep = 0;

		int nMainVol = 0;
		int nChildVol = 0;

		bool recalc_steps = true;
		bool eq_updated = false;

		m_CrossfadeActive = true;

		// start the main crossfade loop
		for ( int i = 0; i < nFadeCount; i++ )
		{
			if ( m_AbortCrossfade )
			{
				m_AbortCrossfade = false;
				m_CrossfadeActive = false;
				fade_success = false;
				break;
			}

			// if the fade type is pause, stop
			// or exit, we need ALL the streams as
			// children. children always fade out
			if ( ( nFadeType == MUSIK_CROSSFADER_PAUSE_RESUME && IsPlaying() && !IsPaused() ) || nFadeType == MUSIK_CROSSFADER_STOP || nFadeType == MUSIK_CROSSFADER_EXIT )
			{	
				if ( GetStreamCount() != nChildCount )
				{
					nChildCount = GetStreamCount();
					recalc_steps = true;
				}
			}

			// otherwise its a regular fade, so find the
			// main and child streams
			else
			{
				if ( nChildCount != GetStreamCount() - 1 )
				{
					nChildCount = GetStreamCount() - 1;	
					recalc_steps = true;
				}

				if ( nMainStream != (int)nChildCount )
				{
					nMainStream = nChildCount;
					recalc_steps = true;
				}
			}	

			// we will switch the equalizer half way
			// through the crossfade, assuming there are
			// child streams. if there are not child streams
			// just start the new EQ right away
			if ( !eq_updated )
			{
				if ( ( nChildCount > 0 && ( i == ( nFadeCount / 2 ) ) ) || ( nChildCount == 0 && i == 0 ) )
				{		
					if ( nFadeType == MUSIK_CROSSFADER_NEW_SONG || nFadeType == MUSIK_CROSSFADER_STOP )
					{
						if ( IsEqualizerEnabled() )
							UpdateEQSettings();
					}

					eq_updated = true;
				}
			}

			if ( recalc_steps )
			{
				// an array containing the volume steps for
				// all the secondary streams
				aChildSteps.clear();
				for ( size_t i = 0; i < nChildCount; i++ )
				{
					ftemp = (float)GetStreamVolume( m_ActiveStreams.at( i ) ) / fFadeCount;
					ntemp = (int)round( ftemp );

					if ( ntemp < 1 )	
						ntemp = 1;

					aChildSteps.push_back( ntemp );
				}

				// the amount of volume to be changed
				// in the primary stream, which ranges from
				// 0 - 255. minimum of 1 per step.
				nFadeStep = GetMaxVolume() / nFadeCount;

				if ( nFadeStep < 1 ) 
					nFadeStep = 1;

				recalc_steps = false;
			}

			// see if we should abort. 
			if ( GetHandle() != nCurrHandle )
			{
				cout << "musikCore::Player: Crossfade aborted\n";

				fade_success = false;
				break;
			}

			// modify all the child streams
			for ( size_t j = 0; j < nChildCount; j++ )
			{
				nChildVol	= GetStreamVolume( m_ActiveStreams.at( j ) );
				nChildVol   -= aChildSteps.at( j );

				if ( nChildVol < 0 ) nChildVol = 0;
				
				SetStreamVolume( m_ActiveStreams.at( j ), nChildVol );
			}

			// modify primary stream, if one exists
			if ( nMainStream > -1 )
			{
				nMainVol += nFadeStep;
				if ( nMainVol > GetMaxVolume() )
					nMainVol = GetMaxVolume();

				SetStreamVolume( m_ActiveStreams.at( nMainStream ), nMainVol );
			}

			// sleep 100 ms
			Sleep( 100 );
		}

		// if there was a successful fade, then some
		// clean up is in order...
		if ( fade_success )
		{
			if ( GetFadeType() == MUSIK_CROSSFADER_PAUSE_RESUME )
			{
				if ( !IsPaused() )
					FinalizePause();
				else
					FinalizeResume();
			}

			else if ( GetFadeType() == MUSIK_CROSSFADER_STOP )
				FinalizeStop();

			else if ( GetFadeType() == MUSIK_CROSSFADER_NEW_SONG  )
				FinalizeNewSong();

			cout << "musikCore::Player: Crossfade finished\n";
		}		
	}

	// an exit crossfade is forced to finalize, even if
	// fade_success = false.
    if ( GetFadeType() == MUSIK_CROSSFADER_EXIT )
	{
		FinalizeExit();
		t_Exit = true;
	}

    else if ( m_CurrSong.GetFormat() == MUSIK_LIBRARY_FORMAT_NET )
    {
        FinalizeStop();
    }

	m_CrossfadeActive = false;
}

///////////////////////////////////////////////////

void Player::run()
{
	m_Active = true;
	m_Finished = false;
	m_Stop = false;
	m_CrossfadeActive = false;
	m_AbortCrossfade = false;

    t_Exit = false;

	cout << "musikCore::Player: worker function started\n";

	while ( !t_Exit && !m_Stop )
	{

		if ( IsPlaying() && !t_Exit )
		{    
            // if there is no cross fading, check the status
            // because we may need to start up the next song
            if ( !IsCrossfaderEnabled() )
                thread_CheckNoCrossfadeStatus();

			// we have a crossfader, and its ready
			// to begin. send the next song message...
			else
                thread_CheckCrossfadeStatus();

			// othwerwise just monitor for the crossfader flag
			if ( m_IsCrossfaderReady )
                thread_Crossfade();
        }     

        Sleep( 25 );
	}

	cout << "musikCore::Player: worker function finished\n";
	m_Finished = true;
}

///////////////////////////////////////////////////

void Player::InitCrossfader()
{
	if ( m_Crossfader ) 
		delete m_Crossfader;

	m_Crossfader = new Crossfader();
}

///////////////////////////////////////////////////

void Player::DeinitCrossfader()
{
	if ( m_Crossfader ) 
	{
		delete m_Crossfader;
		m_Crossfader = NULL;
	}
}

///////////////////////////////////////////////////

void Player::InitEQSettings()
{
	m_Library->GetDefaultEqualizer( m_EQSettings );
}

///////////////////////////////////////////////////

void Player::UseGlobalEQSettings( bool use_global )
{
    m_UseGlobalEQSettings = use_global;
    UpdateEQSettings();
}

///////////////////////////////////////////////////

int Player::InitSound( int device, int driver, int rate, int mode )
{
	cout << "musikCore::Player: Attempting to initialize sound system\n";

	if ( mode == MUSIK_PLAYER_INIT_RESTART || mode == MUSIK_PLAYER_INIT_STOP )
		StopSound();

	if ( mode == MUSIK_PLAYER_INIT_START || mode == MUSIK_PLAYER_INIT_RESTART )
	{
		m_State = InitSoundSystem( device, driver, rate );

		if ( m_State != MUSIK_PLAYER_INIT_SUCCESS )
		{
			cout << "musikCore::Player: sound system failed to initialize.\n";
			m_Rate = 0;
			return InitSoundSystem( device, driver, rate );
		}
	}

	m_Rate = rate;
	cout << "musikCore::Player: sound system initialized successfully.\n";
	return MUSIK_PLAYER_INIT_SUCCESS;
}

///////////////////////////////////////////////////

void Player::StopSound()
{
	CleanOldStreams( true );
    DeinitSoundSystem();
    m_State = MUSIK_PLAYER_INIT_UNINITIALIZED;
}

///////////////////////////////////////////////////

void Player::SetPlaylist( Playlist* playlist )
{
	if ( playlist == m_Playlist )
		return;

	DeinitPlaylist();
	playlist->SetType( MUSIK_PLAYLIST_TYPE_NOWPLAYING );
	m_Playlist = playlist;
	m_Index = -1;
}

///////////////////////////////////////////////////

void Player::SetPlaylist( const Playlist& playlist )
{
	if ( m_Playlist )
		m_Playlist->clear();
	else
		m_Playlist = new Playlist();

	for ( size_t i = 0; i < playlist.size(); i++ )
		m_Playlist->push_back( playlist.at( i ) );
}

///////////////////////////////////////////////////

bool Player::PlayCD( int track, int start_pos )
{
    if ( (int)m_CDInfo.size() < track )
        return false;

    CleanOldStreams( true );
    Stream* pNewStream = LoadCDTrack( track );
    if ( pNewStream )
    {
        m_CurrSong = m_CDInfo.at( track );
        m_PlayerType = MUSIK_PLAYER_CDROM;
        m_Index = track;

		m_Duration = GetStreamDuration( pNewStream );
		SetStreamVolume( pNewStream, m_Volume );

        // stream is loaded, play it
        PlayStream( pNewStream, 0 );

        m_IsPlaying = true;
        if ( IsPaused() )
            Pause();

        m_ActiveStreams.push_back( pNewStream );
        m_Functor->OnNewSong();

        return true;
    }

    return false;
}

///////////////////////////////////////////////////

bool Player::Play( int index, int start_pos, int advance_type, bool force )
{
	if ( m_State != MUSIK_PLAYER_INIT_SUCCESS )
	    return false;

    if ( force && IsPaused() )
		StreamResume( NULL );

	//
	// verify playlist even exists and that the song index
	// requested falls within the playlist's range
	//

	if ( !m_Playlist )
	{
		cout << "musikCore::Player::Play: Playlist does not exist.\n";
		return false;
	}

	if ( index >= (int)m_Playlist->GetCount() || index < 0 )
	{
		cout << "musikCore::Player::Play: Playlist song out of range or finished.\n";
		Stop();
		return false;
	}

	//
	// save old song's info and equalizer settings 
	//

	UpdateSongHistory( false );	
	SaveSongEq();

	//
	// if old song was a netstream or cd audio, then do
	// clear all the old streams
	//

	if ( m_Format == MUSIK_LIBRARY_FORMAT_NET || m_PlayerType == MUSIK_PLAYER_CDROM )
	{
		CleanOldStreams( true );
		m_IsPlaying = false;
		m_IsPaused = false;
	}

	//
	// load song info for new song
	//

	int songid = m_Playlist->GetSongID( index );

	SongInfo info;
	Library* library;

	library = m_Playlist->at( index ).GetLibrary();
	library->GetSongInfoFromID( songid, info );
	m_Format = info.GetFormat();

	if ( m_Format != MUSIK_LIBRARY_FORMAT_NET )
	{
		if ( !Filename::FileExists( info.GetFilename() ) )
		{
			if ( advance_type == MUSIK_PLAYER_SONG_USER_ADVANCE )
				return false;
			else
				return Next();
		}
	}

	m_CurrSong = info;

	advance_type == MUSIK_PLAYER_SONG_USER_ADVANCE ? m_CurrSong.SetUserChoose() : m_CurrSong.SetAutoChoose();

	Stream* pNewStream = NULL;

	//
	// if the new song is a netstream
	//

	if ( m_Format == MUSIK_LIBRARY_FORMAT_NET )
	{
		cout << _T( "musikCore::Player::Play: netstream connecting...\n" );
		AbortCrossfade( true );
		m_Handle++;
		m_Index = index;
		m_Format = MUSIK_LIBRARY_FORMAT_NET;
		m_CrossfadeActive = false;
		m_IsPlaying = false;
		m_IsPaused = false;

		pNewStream = LoadNetStream();

		if ( !pNewStream )
		{
			if ( advance_type == MUSIK_PLAYER_SONG_USER_ADVANCE )
				return false;
			else
				return Next();
		}

		// if we loaded the stream, then go ahead
		// and clean any current playback
		CleanOldStreams( true );
	}

	//
	// if the new song is NOT a netstream
	//

	else
	{  
		pNewStream = LoadStream( m_CurrSong.GetFilename() );

		// assure the stream was loaded
		if ( !pNewStream )
		{
			cout << "musikCore::Player::Play: Song file failed to load. Invalid filename?\n";
			cout << m_CurrSong.GetFilename().c_str();
			cout << "\n";

			m_IsPlaying = false;
			m_IsPaused = false;

			return false;
		}

		if ( force && IsPaused() )
			m_IsPaused = false;	
	}

	//
	// call OnNextTrack() to flag the crossfader, clean
	// old streams, update the song's history information,
	// and call the functor
	//

    m_PlayerType = MUSIK_PLAYER_NORMAL;
	OnNextTrack( pNewStream, MUSIK_CROSSFADER_NEW_SONG, start_pos );
	m_Index = index;

	return true;
}

///////////////////////////////////////////////////

void Player::OnNextTrack( Stream* pNewStream, int fade_type, int start_pos )
{
    bool no_crossfade = false;
    if ( !IsCrossfaderEnabled() || ( m_Crossfader->GetDuration( fade_type ) < 0 || fade_type == MUSIK_CROSSFADER_NONE ) )
        no_crossfade = true;

    if ( no_crossfade )
        CleanOldStreams( true );

	SetStreamVolume( pNewStream, 0 );
    PlayStream( pNewStream, start_pos );

    if ( IsPaused() )
        StreamPause( pNewStream );

    m_ActiveStreams.push_back( pNewStream );
    m_Duration = GetStreamDuration( pNewStream );

    // if duration discovered by player is different
    // than duration currently in library, update 
    // library...
	if ( m_Format != MUSIK_LIBRARY_FORMAT_NET )
	{
		if ( GetPlaying()->GetDuration() != IntToString( m_Duration ) )
		{
			GetPlaying()->SetDuration( IntToString( m_Duration ) );
			GetPlaying()->GetLibrary()->SetSongInfo( *GetPlaying() );
		}
	}

    SetStreamVolume( pNewStream, 0 );

	m_Handle++;
	m_FadeType = fade_type;

	// toggle the flag
	m_IsPlaying = true;

	// if the current type of crossfade is either
	// disabled or at 0.0 seconds, just cut
	// out any old streams before we start up the next
	if ( no_crossfade || m_Format == MUSIK_LIBRARY_FORMAT_NET )
		SetStreamVolume( pNewStream, m_Volume );

	// if we got a valid crossfade type, flag it
	// so the thread can pick it up when we finish
	else
	{
        if ( IsPlaying() && !IsPaused() )
        {
		    AbortCrossfade();
		    FlagCrossfade();
        }
	}

	UpdateSongHistory( true );
    m_Functor->OnNewSong();
}

///////////////////////////////////////////////////

void Player::UpdateSongHistory( bool begin )
{
	// if begin is true, the song is just beginning
	// if begin is false, the song is ending
	if ( m_CurrSong.GetID() != -1 )
	{
		if ( begin )
		{
			m_CurrSong.GetLibrary()->UpdateTimesPlayed( m_CurrSong );
		}
		else
		{
			m_CurrSong.SetPlayTime( GetTimeNowPer() );
			m_CurrSong.GetLibrary()->UpdatePlayTime( m_CurrSong );
		}
	}
}

///////////////////////////////////////////////////

bool Player::Next( bool expired )
{
	if ( !m_Playlist || !IsPlaying() )
		return false;

	// if repeat single is enabled, and the
	// stream naturally expired (a "next" button
	// was not clicked, for example), we want to
	// start the song over...
	if ( expired && m_Playmode & MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE )
	{
		// m_Index = m_index;
	}

	// if we are repeating a playlist, check to see
	// if the next song is beyond the last song. if
	// it is, start it over from the beginning

    else if ( m_Playmode & MUSIK_PLAYER_PLAYMODE_RANDOM )
    {
        if ( m_PlayerType == MUSIK_PLAYER_NORMAL )     
            m_Index = Library::Rand( 0, (int)m_Playlist->GetCount() );
        else
            m_Index = Library::Rand( 0, (int)m_CDInfo.size() );
    }

	else if ( m_Playmode & MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST )
	{
        if ( m_PlayerType == MUSIK_PLAYER_NORMAL )
        {
		    if ( m_Index + 1 >= (int)m_Playlist->GetCount() )
			    m_Index = 0;
		    else
			    m_Index++;
        }
        else if ( m_PlayerType == MUSIK_PLAYER_CDROM )
        {
		    if ( m_Index + 1 >= (int)m_CDInfo.size() )
			    m_Index = 0;
		    else
			    m_Index++;
        }
	}

	// else nothing special, we're just playing straight
	// through the playlist...
	else
	{
        if ( m_PlayerType == MUSIK_PLAYER_NORMAL )
        {
		    if ( m_Index + 1 >= (int)m_Playlist->GetCount() )
            {
                Stop();
			    return false;
            }
		    else
			    m_Index++;
        }
        else
        {
		    if ( m_Index + 1 >= (int)m_CDInfo.size() )
            {
                Stop();
			    return false;
            }
		    else
			    m_Index++;
        }
	}

    if ( m_PlayerType == MUSIK_PLAYER_NORMAL )
    {
		Play( 
            m_Index, 
            0, 
            expired ? MUSIK_PLAYER_SONG_AUTO_ADVANCE : MUSIK_PLAYER_SONG_USER_ADVANCE,
			false );
    }
    else
        PlayCD( m_Index );
		
	return true;
}

///////////////////////////////////////////////////

bool Player::Prev()
{
	if ( !m_Playlist || !IsPlaying() )
		return false;

	// if the song is under 6000 ms, we want to go
	// to the previous track
	if ( GetTimeNow( MUSIK_TIME_MSECS ) < 6000 || !IsPlaying() )
	{
		m_Index--;

        if ( m_Index < 0 ) 
        {
            if ( m_PlayerType == MUSIK_PLAYER_NORMAL )
            {
                if ( m_Playmode & MUSIK_PLAYER_PLAYMODE_RANDOM )
                    m_Index = Library::Rand( 0, (int)m_Playlist->GetCount() );
			    else if ( m_Playmode & MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST )
				    m_Index = (int)m_Playlist->GetCount() - 1;	
			    else
				    m_Index = 0;
            }
            else if ( m_PlayerType == MUSIK_PLAYER_CDROM )
            {
                if ( m_Playmode & MUSIK_PLAYER_PLAYMODE_RANDOM )
                    m_Index = Library::Rand( 0, (int)m_CDInfo.size() );
                if ( m_Playmode & MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST )
				    m_Index = (int)m_CDInfo.size() - 1;	
			    else
				    m_Index = 0;
            }
        }
	}
	//  else
	//      m_Index = m_Index;

	// song time elapsted is greater than 2000 ms
	// we want to start the song over
	// m_Index == m_Index
    if ( m_PlayerType == MUSIK_PLAYER_NORMAL )
		Play( m_Index, MUSIK_CROSSFADER_NEW_SONG );
    else
        PlayCD( m_Index );

	return true;
}

///////////////////////////////////////////////////

void Player::Stop()
{
	if ( !IsPlaying() )
		return;

	UpdateSongHistory( false );

	m_CurrSong.SetID( -1 );

	if ( m_PlayerType == MUSIK_PLAYER_CDROM || m_Format == MUSIK_LIBRARY_FORMAT_NET )
		FinalizeStop();
	else if ( IsCrossfaderEnabled() && m_Crossfader->GetDuration( MUSIK_CROSSFADER_STOP ) > 0 )
	{
		if ( IsCrossfadeActive() )
			AbortCrossfade();

		m_Handle++;
		m_FadeType = MUSIK_CROSSFADER_STOP;
		FlagCrossfade();
	}
	else
		FinalizeStop();
}

///////////////////////////////////////////////////

void Player::FinalizeStop()
{
	CleanOldStreams( true );

	m_IsPlaying = false;
	m_IsPaused = false;
	m_Index = 0;

	if ( m_Functor )
		m_Functor->OnStop();
}

///////////////////////////////////////////////////

void Player::FinalizeExit()
{
	CleanOldStreams( true );

	m_IsPlaying = false;
	m_IsPaused = false;
}

///////////////////////////////////////////////////

void Player::Exit( bool wait )
{
	m_Exiting = true;

	if ( m_PlayerType == MUSIK_PLAYER_CDROM || m_Format == MUSIK_LIBRARY_FORMAT_NET )
	{
		CleanOldStreams( TRUE );
		FinalizeExit();
	}
	else if ( IsCrossfaderEnabled() && m_Crossfader->GetDuration( MUSIK_CROSSFADER_EXIT ) > 0 && IsPlaying() )
	{
        // wait for any netstream to shut down      
		if ( IsCrossfadeActive() )
			AbortCrossfade();
        
		m_FadeType = MUSIK_CROSSFADER_EXIT;
		FlagCrossfade();
		m_Handle++;
        
        if ( wait )
        {
            while ( !IsFinished() )
                Sleep( 50 );
        }
        
        return;
	}
	else 
	{
		StopWait();
		FinalizeExit();
	}
}

///////////////////////////////////////////////////

Stream* Player::GetCurrStream()
{
	if ( m_ActiveStreams.size() )
		return m_ActiveStreams.at( m_ActiveStreams.size() - 1 );
    
    return NULL;
}

///////////////////////////////////////////////////

void Player::EnableEqualizer( bool enable, bool use_global )
{
    m_UseGlobalEQSettings = use_global;

	if ( enable )
	{
		InitEQSettings();
		InitEQ();
	}
	else
	    DeinitEQ();
}

///////////////////////////////////////////////////

void Player::CleanOldStreams( bool kill_primary )
{
	if ( !m_ActiveStreams.size() )
		return;

	size_t nStreamCount = m_ActiveStreams.size();

	if ( nStreamCount <= 0 )
		return;
	else if ( !kill_primary )
		nStreamCount--;

	for ( size_t i = 0; i < nStreamCount; i++ )
	{	
        OnDeleteStream( m_ActiveStreams.at( 0 ) );
		m_ActiveStreams.erase( m_ActiveStreams.begin() );        
	}	
}

///////////////////////////////////////////////////

bool Player::IsCrossfaderEnabled()
{
	if ( m_Crossfader )
		return true;
	
	return false;
}

///////////////////////////////////////////////////

int Player::GetDuration( int mode )
{
	int nTime = m_Duration;

	if ( mode == MUSIK_TIME_SECS )
		nTime /= 1000;

	return nTime;
}

///////////////////////////////////////////////////

int Player::GetTimeNow( int mode )
{
	if ( !IsPlaying() )
		return -1;

	int nCurr;
    
    if ( GetStreamCount() )
        nCurr = GetStreamPos( GetCurrStream() );
    else
        nCurr = 0;
    
	if ( mode == MUSIK_TIME_SECS )
		nCurr /= 1000;

	return nCurr;
}

///////////////////////////////////////////////////

int Player::GetTimePer ( int percent )
{
	if ( percent == 0 )
		percent = 1;
	else if ( percent > 99 )
		percent = 100;
	
	int nDuration = GetDuration( MUSIK_TIME_MSECS );
	float fFinal = (float)nDuration * ( (float)percent / 100.0f );

	return (int)fFinal;
}

///////////////////////////////////////////////////

String Player::GetTimePerStr( int percent )
{
	return GetTimeStr( GetTimePer( percent ) );
}

///////////////////////////////////////////////////

int Player::GetTimeNowPer()
{
	int nCurr = GetTimeNow( MUSIK_TIME_SECS );
	int nTotal = GetDuration( MUSIK_TIME_SECS );

	if ( nTotal == 0 )
		return 0;

	return ( ( nCurr * 100 ) / nTotal );
}

///////////////////////////////////////////////////

void Player::SetTimeNowPer( int percent )
{
    if ( !IsPlaying() )
        return;

	if ( percent > 99 )
		percent = 99;

	int nPos = GetDuration( MUSIK_TIME_MSECS );
	float fPos = ( (float)percent / 100.0f ) * nPos;
   
    m_ThreadMutex.lock();
	    
		SetStreamPos( GetCurrStream(), (int)fPos );
		
		if ( m_Functor )
			m_Functor->OnSeek();

    m_ThreadMutex.unlock();
}

///////////////////////////////////////////////////

void Player::SetTimeNow( int time, int mode )
{
    if ( !IsPlaying() )
        return;

	if ( time > GetDuration( mode ) )
	{
		time = GetDuration( mode );
	}

	if ( mode == MUSIK_TIME_SECS )
	{
		time *= 1000;
	}	
 
    m_ThreadMutex.lock();
	    
		SetStreamPos( GetCurrStream(), (int)time );
		
		if ( m_Functor )
			m_Functor->OnSeek();

    m_ThreadMutex.unlock();
}

///////////////////////////////////////////////////

int Player::GetTimeRemain( int mode )
{
	int nLeft = GetDuration( mode ) - GetTimeNow( mode );

	if ( mode == MUSIK_TIME_SECS )
		nLeft /= 1000;

	return nLeft;
}

///////////////////////////////////////////////////

void Player::FinalizeNewSong()
{
	CleanOldStreams();
}

///////////////////////////////////////////////////

size_t Player::GetStreamCount()
{
	return m_ActiveStreams.size();
}

///////////////////////////////////////////////////

void Player::SetMaxVolume( int n, bool set_now )
{
	m_Volume = n;

	if ( set_now && GetStreamCount() )
        SetStreamVolume( GetCurrStream(), n );
}

///////////////////////////////////////////////////

void Player::EnableCrossfader( bool enable )
{
	m_CrossfaderEnabled = enable;

	if ( enable )
	{
		DeinitCrossfader();
		InitCrossfader(); 
	}
	else
		DeinitCrossfader();
}

///////////////////////////////////////////////////

bool Player::SetCrossfader( const Crossfader& fader )
{
	if ( m_CrossfaderEnabled )
	{
		if ( !m_Crossfader )
			InitCrossfader();

		*m_Crossfader = fader;

		return true;
	}

	return false;
}

///////////////////////////////////////////////////

void Player::DeinitPlaylist()
{
	if ( m_Playlist )
	{
		delete m_Playlist;
		m_Playlist = NULL;
	}
}

///////////////////////////////////////////////////

String Player::GetTimeStr( int time_ms )
{
	String sTime;

	int ms = time_ms;
	int hours = ms / 1000 / 60 / 60;
	ms -= hours * 1000 * 60 * 60;
	int minutes = ms / 1000 / 60;
	ms -= minutes * 1000 * 60;
	int seconds = ms / 1000;

	if ( hours > 0 )
		sTime.Format( _T( "%d:%02d:%02d" ), hours, minutes, seconds );
	else
		sTime.Format( _T( "%d:%02d" ), minutes, seconds );
	
	return sTime;
}

///////////////////////////////////////////////////

bool Player::Pause()
{
	if ( IsPlaying() && !IsPaused() )
	{
		if ( IsCrossfadeActive() )
			AbortCrossfade();

		if ( IsCrossfaderEnabled() && m_Crossfader->GetDuration ( MUSIK_CROSSFADER_PAUSE_RESUME ) > 0 )
		{
			m_FadeType = MUSIK_CROSSFADER_PAUSE_RESUME;
			FlagCrossfade();
		}
		else
			FinalizePause();
	}

	return true;
}

///////////////////////////////////////////////////

void Player::FinalizePause()
{
	StreamPause( NULL );
	SetStreamVolume( GetCurrStream(), 0 );

	CleanOldStreams();
	m_IsPaused = true;
	m_IsPlaying = true;

	if ( m_Functor )
		m_Functor->OnPause();
}

///////////////////////////////////////////////////

bool Player::Resume()
{
	if ( IsPlaying() && IsPaused() )
	{
		if ( IsCrossfadeActive() )
			AbortCrossfade();

		CleanOldStreams();
        StreamResume( NULL );

		if ( IsCrossfaderEnabled() && m_Crossfader->GetDuration ( MUSIK_CROSSFADER_PAUSE_RESUME ) > 0 )
		{
			m_FadeType = MUSIK_CROSSFADER_PAUSE_RESUME;
			FlagCrossfade();
		}
		else
			FinalizeResume();
	}

	return true;
}

///////////////////////////////////////////////////

void Player::FinalizeResume()
{
    SetStreamVolume( GetCurrStream(), m_Volume );
	CleanOldStreams();

	m_IsPaused = false;
	m_IsPlaying = true;

	if ( m_Functor )
		m_Functor->OnResume();
}

///////////////////////////////////////////////////

void Player::FinalizeNetstream()
{
    StreamResume( GetCurrStream() );

	if ( IsCrossfaderEnabled() )
	{
        SetStreamVolume( GetCurrStream(), 0 );
		m_FadeType = MUSIK_CROSSFADER_NEW_SONG;
		FlagCrossfade();
	}
	else
	{
        SetStreamVolume( GetCurrStream(), m_Volume );
		CleanOldStreams();		
	}
	m_IsPlaying = true;
	m_IsPaused = false;
		
	if ( m_Functor )
		m_Functor->OnNewSong();
}

///////////////////////////////////////////////////

void Player::ModifyPlaymode( unsigned long add, unsigned long remove, bool remove_first )
{
	if ( remove_first )
	{
		if ( remove )
			m_Playmode &= ~remove;

		if ( add )
			m_Playmode |= add;

		return;	
	}

	if ( add )
		m_Playmode |= add;

	if ( remove )
		m_Playmode &= ~remove;
}

///////////////////////////////////////////////////

void Player::UpdatePlaylist()
{
	// check if all songs in the playlist are still in the library:
	// (we do this backwards as we might delete entries and the indices might change)

	// cl: added the ability to support multiple libraries
	musikCore::Library* curr;
	for ( int i = m_Playlist->size() - 1; i >= 0; i-- )
	{
		curr = m_Playlist->at( i ).GetLibrary();
		if ( !curr->IsSongInLibrary( m_Playlist->GetSongID( i ) ) )
			m_Playlist->DeleteAt( i );
	}
}

///////////////////////////////////////////////////

void Player::UpdateEQSettings( bool refreshonly, bool callfunctor )
{
    if ( refreshonly )
    {
		if ( IsEqualizerEnabled() )
			UpdateEQTable();

        if ( callfunctor )
            m_Functor->OnNewEqualizer();

        return;
    }

	if ( IsEqualizerEnabled() && m_Playlist && m_Index > -1 )
	{	
        if ( m_UseGlobalEQSettings )
            m_Library->GetDefaultEqualizer( m_EQSettings );
        else
        {
		    if ( m_Index < (int)m_Playlist->GetCount() )
		    {
			    int songid = m_Playlist->GetSongID( m_Index );
			    if ( songid != -1 )
				{
					int eqid = StringToInt( m_Playlist->at( m_Index ).GetField( MUSIK_LIBRARY_TYPE_EQUALIZER ) );
					
					// the song has a custom equalizer
					if ( eqid != -1 )				
						m_Playlist->at( m_Index ).m_Library->GetEqualizer( eqid, m_EQSettings );

					// doesn't have a custom equalizer, load default
					else
						m_Library->GetDefaultEqualizer( m_EQSettings );
				}
		    }
        }

		UpdateEQTable();
        if ( callfunctor )
            m_Functor->OnNewEqualizer();
	}
}

///////////////////////////////////////////////////

bool Player::FindNewIndex( int songid )
{	
	bool found = false;

	if ( !m_Playlist )
		return false;

	if ( songid == m_Playlist->GetSongID( m_Index ) && songid != -1 && (int)m_Playlist->GetCount() > m_Index )
		return true;

	if ( songid == -1 )
		songid = GetPlaying()->GetID();

	for ( size_t i = 0; i < m_Playlist->GetCount(); i++ )
	{
		if ( m_Playlist->GetSongID( i ) == songid )
		{	
			m_Index = i;
			found = true;
			break;
		}
	}

	return found;
}

///////////////////////////////////////////////////

void Player::Shuffle()
{
	int songid = GetPlaying()->GetID();
	std::random_shuffle( m_Playlist->begin(), m_Playlist->end() );
	FindNewIndex( songid );
}

///////////////////////////////////////////////////

int Player::GetFileLengthMS( const String& fn )
{
    /*
    Stream* pStream = LoadStream( fn );
    int ret = GetStreamDuration( pStream );
    OnDeleteStream( pStream );

    return ret;
    */
    return -1;
}

///////////////////////////////////////////////////

void Player::SetCDInfo( SongInfoArray info, int drive )
{ 
    m_CDInfo = info; 
    m_DiscDrive = drive; 

    if ( m_PlayerType == MUSIK_PLAYER_CDROM && IsPlaying() )
    {
        if ( (int)info.size() > m_Index )
        {
            m_CurrSong = info.at( m_Index );
            if ( m_Functor )
                m_Functor->OnNewSong();
        }
    }
}

///////////////////////////////////////////////////

void Player::LibraryDetach( Library* detach )
{


	for ( size_t i = 0; i < m_Playlist->size(); i++ )
	{
		if ( m_Playlist->at( i ).GetLibrary() == detach )
		{
			m_Playlist->erase( m_Playlist->begin() + i );
			i--;
		}
	}

	bool playing_detached = false;
	if ( this->GetPlaying()->GetLibrary() == detach )
	{
		this->Next();
	}
}

///////////////////////////////////////////////////

void Player::InitEQ()
{
	m_IsEQActive = true;
	OnEQEnable();	// call to virtual class
}


///////////////////////////////////////////////////

void Player::DeinitEQ()
{
	OnEQDisable();	// call to virtual class
	m_IsEQActive = false;

}

///////////////////////////////////////////////////

void Player::UpdateEQTable()
{
	OnEQUpdate();	// call to virtual class
}

///////////////////////////////////////////////////

void Player::SaveSongEq()
{
	if ( IsPlaying() && !m_UseGlobalEQSettings )
	{
		// no stored equalizer, so create one
		if ( StringToInt( m_CurrSong.GetEqualizer().c_str() ) == -1 )
		{
			m_CurrSong.m_Library->CreateEqualizer( m_EQSettings, false );
			m_CurrSong.m_Library->SetSongEqualizer( m_CurrSong.GetID(), m_EQSettings.m_ID );
		}

		// equalizer is already stored in the song's library,
		// so just update it.
		else
			m_CurrSong.m_Library->UpdateEqualizer( m_EQSettings.m_ID, m_EQSettings );
	}
}

///////////////////////////////////////////////////


