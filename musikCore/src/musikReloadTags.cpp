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
#include "../include/musikReloadTags.h"
#include "../include/musikPlaylist.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// ReloadTags

///////////////////////////////////////////////////

ReloadTags::ReloadTags( Functor* functor, Playlist* playlist, Library* library )
{
	m_Playlist = playlist;
	m_Library = library;
	m_Functor = functor;
	m_CallFunctorOnAbort = true;
}

///////////////////////////////////////////////////

ReloadTags::~ReloadTags()
{
	if ( m_Playlist )
		delete m_Playlist;
}

///////////////////////////////////////////////////

// ReloadTagsTask

///////////////////////////////////////////////////

ReloadTagsTask::ReloadTagsTask()
	: Task()
{
	m_Params = NULL;
	m_Type = MUSIK_TASK_TYPE_RELOAD_TAGS;
}

///////////////////////////////////////////////////

ReloadTagsTask::~ReloadTagsTask()
{
    if ( m_Params )
	{
		delete m_Params;
	}

	cout << "musikCore::BatchAddTask: exiting...\n";
}

///////////////////////////////////////////////////

void ReloadTagsTask::run()
{
	m_Stop = false;
	m_Finished = false;
	m_Active = true;

	if ( m_Params )
	{      
		if ( m_Params->m_Functor )
            m_Params->m_Functor->OnTaskStart();
        
		size_t last_prog = 0;
		bool verify_failed = false;

		Song song;

		m_Params->m_Library->BeginTransaction();
		for ( size_t i = 0; i < m_Params->m_Playlist->size(); i++ )
		{
			m_Params->m_Library->ReloadTags( m_Params->m_Playlist->at( i ) );

			// post progress to the functor
			m_Progress = ( 100 * i ) / m_Params->m_Playlist->size();
			if ( m_Progress != last_prog )
			{
				if ( m_Params->m_Functor )
					m_Params->m_Functor->OnTaskProgress( m_Progress, this );
				last_prog = m_Progress;
			}
		}
		m_Params->m_Library->EndTransaction();
	}

	m_Finished = true;
    m_Progress = 100;
    
    Functor* fnct = m_Params->m_Functor;
    bool call_on_abort = m_Params->m_CallFunctorOnAbort;
    
	if ( fnct && ( ( m_Stop && call_on_abort ) || !m_Stop ) )
        fnct->OnTaskEnd( ( Task* )this );
}

///////////////////////////////////////////////////
