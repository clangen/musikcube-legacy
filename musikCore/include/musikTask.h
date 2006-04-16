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

#ifndef C_MUSIK_TASK
#define C_MUSIK_TASK

///////////////////////////////////////////////////

#include "musikConfig.h"

#include <OpenThreads/Thread>
using namespace OpenThreads;

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

enum
{
	MUSIK_TASK_TYPE_UNKNOWN = -1,
	MUSIK_TASK_TYPE_BATCHADD = 0,
	MUSIK_TASK_TYPE_BATCHRETAG,
	MUSIK_TASK_TYPE_REMOVEOLD,
	MUSIK_TASK_TYPE_BATCHCHGCASE,
	MUSIK_TASK_TYPE_TAGFROMFN,
	MUSIK_TASK_TYPE_TAGTOFN,
	MUSIK_TASK_TYPE_PLAYER_WORKER,
	MUSIK_TASK_TYPE_RELOAD_TAGS,
	MUSIK_TASK_TYPE_LAST
};

///////////////////////////////////////////////////

/**
An object oriented thread. Any class wishing to become a thread
may inherit from Task. Override the virtual "run()" function
and implement your own operation.

@author Casey Langen
*/
class MUSIKEXPORT Task : public Thread
{
public:

	/**
	 * Default constructor.
	 */
	Task();
	virtual ~Task();

	/**
	 * Returns the type of task we are. The m_Type paramater
	 * should be set by the inherited class.
	 *
	 * \return type of task we are.
	 */
	int GetType() const { return m_Type; }
	/**
	 * Stop the thread and wait timeout_secs seconds before 
	 * forcing the m_Active flag false and returning. Once
	 * returned the object should be safe to delete.
	 *
	 * \param timeout_secs number of seconds to wait.
	 */
	bool StopWait( int timeout_secs );
	/**
	 * Gets the current percentage complete.
	 *\return the percentage complete.
	 */
	size_t GetProgress() const { return m_Progress; }

	/**
	 * Returns true if the task is finished.
	 */
	bool IsFinished() const { return m_Finished; }
	/**
	 * Returns true if the task is active
	 */
	bool IsActive() const { return m_Active; }
	/**
	 * Sets the stop flag then returns without waiting to assure
	 * the thread terminates.
	 */
	void Stop(){ m_Stop = true; }

	void Start();

protected:

	/**
	 * Is the thread currently active?
	 */
	bool m_Active;
	/**
	 * Has the thread set itself as finished? It is up to the developer
	 * to set this to true once his void run() has completed.
	 */
	bool m_Finished;
	/**
	 * The abort flag
	 */
	bool m_Stop;
	/**
	 * Type of task. It is up to the developer to set this. 
	 */
	int m_Type;
	/**
	 * Current progress in percent (0-100)
	 */
	size_t m_Progress;
};

typedef std::vector<Task*> TaskPtrArray;

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
