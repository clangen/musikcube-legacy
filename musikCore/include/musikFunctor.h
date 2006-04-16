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

#ifndef C_MUSIK_FUNCTOR
#define C_MUSIK_FUNCTOR

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikTask.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
An object that acts as a virtual interface between musikCore
and an application or plugin.

Task objects, including Player are constructed with
a Functor as an argument. A practical example of the usage:

When a new song starts, Player calls its internal functor's 
OnNewSong() function. This function in return posts an event to 
an event to it's parent frame.

All the functions are optional to override and only exist to provide
an easy means for any type of interface to manage Tasks.

@author Casey Langen
*/
class MUSIKEXPORT Functor
{
public:

	/**
	 * Default constructor.
	 */
	Functor();
	virtual ~Functor();

	/**
	 * Called by Player when a new song starts
	 */
	virtual void OnNewSong();
	/**
	 * Called by Player when playback is paused
	 */
	virtual void OnPause();
	/**
	 * Called by Player when playback is resumed
	 */
	virtual void OnResume();
	/**
	 * Called by Player when playback is stopped
	 */
	virtual void OnStop();
	/**
	 * Task objects, with the exception of CmusikPlayerWorker
	 * should call this at the beginning of their Run() 
	 * function
	 */
	virtual void OnTaskStart();
	/**
	 * Task objects, with the exception of CmusikPlayerWorker,
	 * should call this function and pass "this" as the parameter. 
	 * This allows a UI to be notified when a task has completed so
	 * it can delete the object, if necessary, decrement a counter, etc.
	 *
	 * \param task_addr address of the task that called the function
	 */
	virtual void OnTaskEnd( Task* task_addr = NULL );
	/**
	 * Cube uses this as a completion % gauge
	 *
	 * \param progress percent of progress
	 * \param task_addr address of task that posted called the function
	 */
	virtual void OnTaskProgress( size_t progress, Task* task_addr = NULL );
	/**
	 * A new song attempted to start playback but failed.
	 */
	virtual void OnPlaybackFailed();
	/**
	 * The player loaded a new equalizer
	 */
	virtual void OnNewEqualizer();
	/**
	 * The thread in question requests verification. This is considered
	 * musikCube specific, but can be applicable elsewhere
	 *
	 * \param pl_addr 
	 * \return 
	 */
	virtual bool VerifyPlaylist( void* pl_addr = NULL );
	/**
	 * A musikCore::Alarm will have the player call this event when it
	 * it is triggered. musikCore::Player::TriggerAlarm() is public,
	 * so other controls can send this event manually, but this is
	 * not recommended.
	 */
	virtual void OnAlarmTrigger();

	/**
	  * Player has just seeked.
	  */
	virtual void OnSeek();

};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
