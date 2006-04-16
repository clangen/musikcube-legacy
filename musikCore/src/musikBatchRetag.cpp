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
#include "../include/musikBatchRetag.h"
#include "../include/musikPlaylist.h"

#if defined (USE_BASS)
#include "../include/musikPlayerBASS.h"	// use plugins to write tags
#endif

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// BatchRetag

///////////////////////////////////////////////////

BatchRetag::BatchRetag()
{
	m_Functor = NULL;
	m_Library = NULL;
	m_UpdatedTags = NULL;
	m_CallFunctorOnAbort = false;
	m_DeleteUpdatedTags = true;
	m_WriteToFile = false;
}

///////////////////////////////////////////////////

BatchRetag::BatchRetag( Library* pLibrary, Functor* pFunctor, SongInfoArray* pUpdatedTags )
{
	m_Functor = pFunctor;
	m_Library = pLibrary;
	m_UpdatedTags = pUpdatedTags;
	m_CallFunctorOnAbort = false;
	m_DeleteUpdatedTags = true;
	m_WriteToFile = false;
}

///////////////////////////////////////////////////

BatchRetag::~BatchRetag()
{
	if ( m_DeleteUpdatedTags )
		delete m_UpdatedTags;
}

///////////////////////////////////////////////////

// BatchRetagTask

///////////////////////////////////////////////////

BatchRetagTask::BatchRetagTask()
	: Task()
{
	m_Params = NULL;
	m_Type = MUSIK_TASK_TYPE_BATCHRETAG;
}

///////////////////////////////////////////////////

BatchRetagTask::~BatchRetagTask()
{
	if ( m_Params )
	{
		delete m_Params;
	}

	cout << "musikCore::BatchRetagTask: deleted\n";
}

///////////////////////////////////////////////////

void BatchRetagTask::run()
{
    cout << "musikCore::BatchRetagTask: started\n";
    
	m_Stop = false;
	m_Finished = false;
	m_Active = true;

	if ( m_Params )
	{
        if ( m_Params->m_Functor )
            m_Params->m_Functor->OnTaskStart();
        
		size_t last_prog = 0;

		m_Params->m_Library->BeginTransaction();
		String sFormat;
		for( size_t i = 0; i < m_Params->m_UpdatedTags->size(); i++ )
		{
			// check abort flag
			if ( m_Stop )
			{
				cout <<"musikCore::BatchRetagTask: worker function aborted\n";
				break;
			}

			if ( m_Params->m_WriteToFile )
			{
				m_Params->m_Library->GetSongFormatFromID( m_Params->m_UpdatedTags->at( i ).GetID(), sFormat );
                if ( sFormat != _T( "net" )	)	// can't tag a netstream
				{

#if defined (USE_BASS)
					const String fn = m_Params->m_UpdatedTags->at( i ).GetFilename();
					for ( size_t j = 0; j < PlayerBASS::s_Plugins.size(); j++ )
					{
						if ( PlayerBASS::s_Plugins.at( j ).CanPlay( fn ) )
						{
							if ( PlayerBASS::s_Plugins.at( j ).WriteTag( m_Params->m_UpdatedTags->at( i ) ) )
								m_Params->m_UpdatedTags->at( i ).SetDirtyFlag( "0" );	// no longer dirty
						}
					}
#else
					if ( m_Params->m_UpdatedTags->at( i ).WriteInfo() == true )
						m_Params->m_UpdatedTags->at( i ).SetDirtyFlag( "0" );	// no longer dirty
#endif

				}
			}
			else
				m_Params->m_UpdatedTags->at( i ).SetDirtyFlag( "1" );

			m_Params->m_Library->SetSongInfo( m_Params->m_UpdatedTags->at( i ) );

			// post progress to the functor
			m_Progress = ( 100 * i ) / m_Params->m_UpdatedTags->size();
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
    
	if ( m_Params->m_Functor )
	{
		if ( !m_Stop || ( m_Stop && m_Params->m_CallFunctorOnAbort ) )
			m_Params->m_Functor->OnTaskEnd( this );
	}
}

///////////////////////////////////////////////////
