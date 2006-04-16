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
#include "../include/musikRemoveOld.h"
#include "../include/musikPlaylist.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// RemoveOld

///////////////////////////////////////////////////

RemoveOld::RemoveOld()
{
	m_Functor = NULL;
	m_Library = NULL;
	m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

RemoveOld::RemoveOld( Library* pLibrary, Functor* pFunctor )
{
	m_Functor = pFunctor;
	m_Library = pLibrary;
	m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

// RemoveOldTask

///////////////////////////////////////////////////

RemoveOldTask::RemoveOldTask()
	: Task()
{
	m_Params = NULL;
	m_Type = MUSIK_TASK_TYPE_REMOVEOLD;
}

///////////////////////////////////////////////////

RemoveOldTask::~RemoveOldTask()
{    
	if ( m_Params )
	{
		delete m_Params;
	}

    cout << "musikCore::RemoveOldTask: deleted\n";
}

///////////////////////////////////////////////////

void RemoveOldTask::run()
{
	m_Stop = false;
	m_Finished = false;
	m_Active = true;

    cout << "musikCore::RemoveOldTask: started\n";
    
	if ( m_Params )
	{
        if ( m_Params->m_Functor )
            m_Params->m_Functor->OnTaskStart();
        
		size_t last_prog = 0;

		StringArray all_files;
		m_Params->m_Library->GetAllDistinct( MUSIK_LIBRARY_TYPE_FILENAME, all_files, false );

		m_Params->m_Library->BeginTransaction();
		for( size_t i = 0; i < all_files.size(); i++ )
		{
			// check abort flag
			if ( m_Stop )
			{
				cout << "musikCore::RemoveOldTask: worker function aborted\n";
				break;
			}

			// see if the file exists
            if ( all_files.at( i ).Left( 7 ).ToLower() == _T( "http://" ) )
                continue;
            
			if ( !Filename::FileExists( all_files.at ( i ) ) )
				m_Params->m_Library->RemoveSong( all_files.at ( i ) );

			// figure out percentage done, then post the 
			// progress event to the functor, if it exists.
			m_Progress = ( 100 * i ) / all_files.size();
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
