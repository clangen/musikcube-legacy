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
#include <iostream>
#include "../include/musikTask.h"

///////////////////////////////////////////////////

using namespace musikCore;

///////////////////////////////////////////////////

Task::~Task()
{
}

///////////////////////////////////////////////////

Task::Task()
{
	m_Stop = false;
	m_Finished = false;
	m_Active = false;
	m_Progress = 0;
}

///////////////////////////////////////////////////

void Task::Start()
{
	start();
}

///////////////////////////////////////////////////

bool Task::StopWait( int timeout_secs )
{
	if ( m_Active )
	{
		m_Stop = true;
		int timeout = 0;
		while ( !m_Finished )
		{
			timeout++;
			Sleep( 100 );

			if ( timeout * 100 >= ( timeout_secs * 1000 ) )
			{
				cout << "musikCore::Task: I appear to be deadlocked. Terminating\n";
				m_Finished = true;
				m_Active = false;
				return false;
			}
		}
	}

	m_Finished = true;
	m_Active = false;
	return true;
}

///////////////////////////////////////////////////
