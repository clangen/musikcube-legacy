//////////////////////////////////////////////////
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
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer
//   Simon Windmill : Developer
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////

#include "../include/musikAlarm.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

AlarmInfo::AlarmInfo()
{
    m_Hours = 12;
    m_Minutes = 00;    
    m_MinVol = 130;
    m_Day = 1;
    m_Month = 1;
    m_Year = 2004;    
    m_SnoozeTime = 5;
	m_ID = -1;
	m_Type = MUSIK_ALARM_TYPE_DAILY;
}

///////////////////////////////////////////////////

AlarmMonitor::AlarmMonitor()
{
	m_Player = NULL;
	m_Library = NULL;
}

///////////////////////////////////////////////////

AlarmMonitor::AlarmMonitor( Player* player, Library* library )
{  
    m_Player = player;
    m_Library = library;
}

///////////////////////////////////////////////////

AlarmMonitor::~AlarmMonitor()
{
	cout << "musikCore::AlarmMonitor: exiting\n";
}

///////////////////////////////////////////////////

bool AlarmMonitor::AlarmTriggered( AlarmInfo& info )
{
    time( &curr_time );     
    ptm = localtime( &curr_time );

	for ( size_t i = 0; i < m_Alarms.size(); i++ )
	{
        if ( m_Alarms.at( i ).m_Hours == ptm->tm_hour && m_Alarms.at( i ).m_Minutes == ptm->tm_min )
		{
			if ( m_Alarms.at( i ).m_Type == MUSIK_ALARM_TYPE_DAILY )
			{
				info = m_Alarms.at( i );
				return true;
			}

			if ( m_Alarms.at( i ).m_Day == ptm->tm_mday && 
				 m_Alarms.at( i ).m_Month == ( ptm->tm_mon + 1 ) && 
				 m_Alarms.at( i ).m_Year == ( ptm->tm_year + 1900 ) )
			{
				info = m_Alarms.at( i );
				return true;
			}
 
		}
	}

	return false;
}

///////////////////////////////////////////////////

void AlarmMonitor::run()
{
    m_Active = true;
    m_State = MUSIK_ALARM_STATE_NORMAL;
	AlarmInfo curr_info;

    while( 1 )
    {
		if ( AlarmTriggered( curr_info ) )
		{
			if ( m_Player && m_Library )
			{
				Playlist* playlist = new Playlist();
				m_Library->GetStdPlaylist( curr_info.m_PlaylistID, *playlist );
    
				if ( !playlist->size() )                
					m_Library->GetAllSongs( *playlist );
                    
				// if SetPlaylist() is called using a pointer, Player
				// knows to delete the playlist once its done with it.
				// no manual deallocation is needed.
				m_Player->SetPlaylist( playlist );
                    
				state_mutex.lock();
				m_State = MUSIK_ALARM_STATE_BEEPING;
				    
				while ( m_State != MUSIK_ALARM_STATE_NORMAL )
				{
					// sleep 10 minutes
					if ( m_State == MUSIK_ALARM_STATE_SNOOZE )
					{
						m_Player->Pause();

						state_mutex.unlock();
						int sleep_sec = curr_info.m_SnoozeTime * 60;
						for ( int i = 0; i < sleep_sec; i++ )
						{
							Sleep( 1000 );

							// sometime in here we may need to return, if the
							// stop signal was caught...
							if ( m_Stop )
							{
								cout << "musikCore::Alarm: finishing from sleep state" << endl;
								m_Active = false;
								m_Finished = true;
								return;
							}
						}

						Sleep( curr_info.m_SnoozeTime * 1000 );
						state_mutex.lock();

						if ( m_State == MUSIK_ALARM_STATE_SNOOZE )
						{
							if ( m_Player->GetMaxVolume() < curr_info.m_MinVol )
								m_Player->SetMaxVolume( curr_info.m_MinVol );
						    
							m_Player->Resume();
						}
					}
					else
					{
						if ( m_Player->GetMaxVolume() < curr_info.m_MinVol )
							m_Player->SetMaxVolume( curr_info.m_MinVol );

						m_Player->Play( 0 );
					}

                        
					m_State = MUSIK_ALARM_STATE_BEEPING;
					state_mutex.unlock();

					m_Player->TriggerAlarm();

					// sleep until we have a new state, or the user changes 
					// the playlist, or playback stops...
					while ( m_State == MUSIK_ALARM_STATE_BEEPING && !m_Stop && m_Player->IsPlaying() && m_Player->GetPlaylist() == playlist )
						Sleep( 500 );

					// if playback stopped or the playlist has changed, then
					// assume the user is now awake. this should only stop if
					// the programmer was sloppy and didn't call Snooze() or
					// StopBeeping()
					if ( !m_Player->IsPlaying() || m_Player->GetPlaylist() != playlist )
						break;
                        
					// we may get here and need to stop
					if ( m_Stop )
					{
						cout << "musikCore::Alarm: finishing from awaiting new state" << endl;
						m_Active = false;
						m_Finished = true;
						return;
					}
						            
					if ( m_State != MUSIK_ALARM_STATE_NORMAL )
						state_mutex.lock();
				}
                    
				state_mutex.lock();
				m_State = MUSIK_ALARM_STATE_NORMAL;
				state_mutex.unlock();

			}  
			else 
				cout << "musikCore::Alarm: error, invalid player or library." << endl;

			// wait for for the time to change so the alarm doesn't
			// get triggered again...
			cout << "musikCore::Alarm: polling will continue in a minute." << endl;
			while ( curr_info.m_Minutes == ptm->tm_min )
			{
				Sleep( 10000 );
				time( &curr_time );     
				ptm = localtime( &curr_time );                  
			}
			cout << "musikCore::Alarm: polling reset." << endl;
		}
        
		if ( m_Stop )
		{
			cout << "musikCore::Alarm: finishing..." << endl;
			m_Active = false;
			m_Finished = true;
			return;
		}
        
        if ( !m_Alarms.size() )
            Sleep( 60000 );
        else        
		    Sleep( 5000 );
	}   
}

///////////////////////////////////////////////////

void AlarmMonitor::RequeryAlarms()
{
	if ( m_Library )
	{
		
	}
}

///////////////////////////////////////////////////

void AlarmMonitor::Snooze()
{
    state_mutex.lock();
    if ( m_State == MUSIK_ALARM_STATE_BEEPING )
        m_State = MUSIK_ALARM_STATE_SNOOZE;  
    state_mutex.unlock();
}

///////////////////////////////////////////////////

void AlarmMonitor::StopBeeping()
{
    state_mutex.lock();
    m_State = MUSIK_ALARM_STATE_NORMAL;     
    state_mutex.unlock();    
}

///////////////////////////////////////////////////
