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
#include "../include/musikTagFromFn.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

using namespace musikCore;

/////////////////////////////////////////////////// 

// TagFromFn

///////////////////////////////////////////////////

TagFromFn::TagFromFn()
{
	m_Library = NULL;
	m_Functor = NULL;
	m_Songs = NULL;

	m_DeleteSongs = true;
	m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

TagFromFn::TagFromFn( Library* pLibrary, SongArray* songs, Functor* pFunctor, String& mask, bool undr_to_spc )
{
	m_Library = pLibrary;
	m_Functor = pFunctor;
	m_Songs = songs;
	m_Mask = mask;
	m_DeleteSongs = true;
	m_CallFunctorOnAbort = false;
	m_UndrToSpace = undr_to_spc;
}

///////////////////////////////////////////////////

TagFromFn::~TagFromFn()
{
	if ( m_DeleteSongs && m_Songs )
		delete m_Songs;
}

///////////////////////////////////////////////////

// TagFromFnTask

///////////////////////////////////////////////////

TagFromFnTask::TagFromFnTask()
	: Task()
{
	m_Params = NULL;
	m_Type = MUSIK_TASK_TYPE_TAGFROMFN;
}

///////////////////////////////////////////////////

TagFromFnTask::~TagFromFnTask()
{
	if ( m_Params )
	{
		delete m_Params;
	}
    
	cout << "musikCore::TagFromFnTask: deleted\n";
}

///////////////////////////////////////////////////

void TagFromFnTask::run()
{
	m_Stop = false;
	m_Finished = false;
	m_Active = true;

	if ( m_Params )
	{
        if ( m_Params->m_Functor )
            m_Params->m_Functor->OnTaskStart();
        
		size_t last_prog = 0;

		m_Params->m_Library->BeginTransaction();
		for( size_t i = 0; i < m_Params->m_Songs->size(); i++ )
		{
			// check abort flag
			if ( m_Stop )
			{
				cout << "musikCore::TagFromFnTask: worker function aborted\n";
				break;
			}

			// get the tag from a filename... if successful update
			// the database...
			SongInfo info;
			m_Params->m_Library->GetSongInfoFromID( m_Params->m_Songs->at( i ).GetID(), info );

			Filename fn( info.GetFilename() );
			if ( fn.GetSongInfo( m_Params->m_Mask, info, false, true, m_Params->m_UndrToSpace ) )
			{
				info.SetDirtyFlag( "1" );
				m_Params->m_Library->SetSongInfo( info );
			}

			// post progress to the functor
			m_Progress = ( 100 * i ) / m_Params->m_Songs->size();
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
		if ( !m_Stop || m_Params->m_CallFunctorOnAbort )
			m_Params->m_Functor->OnTaskEnd( this );	
	}
}

///////////////////////////////////////////////////
