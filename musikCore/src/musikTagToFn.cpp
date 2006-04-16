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
#include "../include/musikTagToFn.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

// TagToFn

///////////////////////////////////////////////////

TagToFn::TagToFn()
{
	m_Library = NULL;
	m_Functor = NULL;
	m_Songs = NULL;

	m_DeleteSongs = true;
	m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

TagToFn::TagToFn( Library* pLibrary, SongArray* songs, Functor* pFunctor, String& mask, bool space_to_under )
{
	m_Library = pLibrary;
	m_Functor = pFunctor;
	m_Songs = songs;
	m_Mask = mask;
	m_SpaceToUndr = space_to_under;

	m_DeleteSongs = true;
	m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

TagToFn::~TagToFn()
{
	if ( m_DeleteSongs && m_Songs )
		delete m_Songs;
}

///////////////////////////////////////////////////

// TagToFnTask

///////////////////////////////////////////////////

TagToFnTask::TagToFnTask()
	: Task()
{
	m_Params = NULL;
	m_Type = MUSIK_TASK_TYPE_TAGTOFN;	
}

///////////////////////////////////////////////////

TagToFnTask::~TagToFnTask()
{
    if ( m_Params )
	{
		delete m_Params;
	}
    
	cout << "musikCore::TagToFnTask: deleted\n";
}

///////////////////////////////////////////////////

void TagToFnTask::run()
{
	m_Stop = false;
	m_Finished = false;
	m_Active = true;

	if ( m_Params )
	{
        if ( m_Params->m_Functor )
            m_Params->m_Functor->OnTaskStart();
        
		size_t last_prog = 0;

		SongInfo info;
		m_Params->m_Library->BeginTransaction();
		for( size_t i = 0; i < m_Params->m_Songs->size(); i++ )
		{
			// check abort flag
			if ( m_Stop )
			{
				cout << "musikCore::TagToFnTask: worker function aborted\n";
				break;
			}

			m_Params->m_Library->GetSongInfoFromID( m_Params->m_Songs->at( i ).GetID(), info );
			Filename fn_rename( info.GetFilename() );

			// convert spaces to underscores
			SongInfo temp = info;
			if ( m_Params->m_SpaceToUndr )
				SpcToUndr( temp );

			if ( fn_rename.RenameFromSongInfo( m_Params->m_Mask, temp ) )
			{
				info.SetFilename( temp.GetFilename() );
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

void TagToFnTask::SpcToUndr( SongInfo& info )
{
	String strTemp;

	strTemp = info.GetTitle();
	strTemp.Replace( _T( " " ), _T( "_" ) );
	info.SetTitle( strTemp );

	strTemp = info.GetArtist();
	strTemp.Replace( _T( " " ), _T( "_" ) );
	info.SetArtist( strTemp );

	strTemp = info.GetAlbum();
	strTemp.Replace( _T( " " ), _T( "_" ) );
	info.SetAlbum( strTemp );

	strTemp = info.GetGenre();
	strTemp.Replace( _T( " " ), _T( "_" ) );
	info.SetGenre( strTemp );

	strTemp = info.GetYear();
	strTemp.Replace( _T( " " ), _T( "_" ) );
	info.SetYear( strTemp );
}

///////////////////////////////////////////////////
