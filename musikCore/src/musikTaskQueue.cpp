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
#include "../include/musikTaskQueue.h"
#include <iostream>
using namespace musikCore;

///////////////////////////////////////////////////

void TaskQueue::TaskQueueWorker::SetQueue( TaskQueue* queue )
{
	m_TaskQueue = queue;
}

///////////////////////////////////////////////////

void TaskQueue::TaskQueueWorker::run()
{
	m_FlushQueue = false;
	m_Active = true;

	Task* ptrCurr = NULL;

	while ( 1 )
	{       
		if ( m_TaskQueue->size() )
		{
			ptrCurr = m_TaskQueue->front();
			ptrCurr->start();
			
			while ( !ptrCurr->IsFinished() )
			{
				if ( m_FlushQueue || m_Stop )
				{
					ptrCurr->StopWait( 2 );
					break;
				}

				Sleep( 250 );
			}

			delete ptrCurr;
			m_TaskQueue->pop();
		}

		// check to see if the queue needs to be flushed.
		// flushing the queue will delete all the
		// respective tasks...
		if ( m_FlushQueue )
		{
			while ( m_TaskQueue->size()  )
			{
				delete m_TaskQueue->front();
				m_TaskQueue->pop();
			}

			m_FlushQueue = false;
		}

		// check termination request
		if ( m_Stop )
		{
			m_Finished = true;
			return;
		}

		Sleep( 250 );
	}

	m_Finished = true;
}

///////////////////////////////////////////////////

TaskQueue::TaskQueueWorker::~TaskQueueWorker()
{
	cout << "musikCore::TaskQueue::TaskQueueWorker: exiting\n";
}

///////////////////////////////////////////////////

TaskQueue::TaskQueue()
{
	m_Worker.SetQueue( this );
	m_Worker.start();
}

///////////////////////////////////////////////////

TaskQueue::~TaskQueue()
{
	Flush();

	cout << "musikCore::TaskQueue: killing task queue worker\n";
	m_Worker.StopWait( 1 );
}

///////////////////////////////////////////////////

void TaskQueue::Flush()
{
	cout << "musikCore::TaskQueue: flushing queue...";
	// block current context until the
	// queue has been flushed
	if ( size() )
	{
		m_Worker.Flush();
		while ( size() )
			Sleep( 250 );
	}
	cout << "flushed.\n";
}

///////////////////////////////////////////////////

void TaskQueue::Push( Task* ptrThread )
{
	m_Queue.push( ptrThread );
}

///////////////////////////////////////////////////

size_t TaskQueue::size()
{
	return m_Queue.size();
}

///////////////////////////////////////////////////

void TaskQueue::pop()
{
	m_Queue.pop();
}

///////////////////////////////////////////////////

Task* TaskQueue::front()
{
	Task* pRet;
	pRet = m_Queue.front();

	return pRet;
}

///////////////////////////////////////////////////
