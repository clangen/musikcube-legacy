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

#include "musikConfig.h"
#include "musikTask.h"
#include <OpenThreads/Mutex>
#include <queue>

///////////////////////////////////////////////////

#ifndef C_MUSIK_TASKQUEUE_H
#define C_MUSIK_TASKQUEUE_H

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

class MUSIKEXPORT TaskQueue
{
	friend class TaskQueueWorker;

public:

	TaskQueue();
	~TaskQueue();

	void Flush();

	void Push( Task* ptrThread );
	size_t size();
	Task* front();
	void pop();

private:

	queue<Task*> m_Queue;

	class TaskQueueWorker : public Task
	{
		friend class TaskQueue;

	public:

		virtual ~TaskQueueWorker();

		void SetQueue( TaskQueue* queue );
		void run();
		void Flush(){ m_FlushQueue = true; }

	private:

		bool m_FlushQueue;
		TaskQueue* m_TaskQueue;

	} m_Worker;

	OpenThreads::Mutex m_Mutex;

};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

