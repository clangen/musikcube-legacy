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

#ifndef C_MUSIK_TAGTOFN_H
#define C_MUSIK_TAGTOFN_H

///////////////////////////////////////////////////

#include "musikConfig.h"

#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
 Parameters for a TagToFnTask, which allows batch
 renaming of managed files based on tag information
 and a specified mask.

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT TagToFn
{
public: 

	TagToFn();
	TagToFn( Library* pLibrary, SongArray* songs, Functor* pFunctor, String& mask, bool space_to_under );
	~TagToFn();

	/**
	 * Library to save information back to.
	 */
	Library* m_Library;
	/**
	 * Functor to call for events
	 */
	Functor* m_Functor;
	/**
	 * Array of songs that the user wishes to rename
	 */	
	SongArray* m_Songs;
	/**
	 * Mask used for renaming
	 */
	String m_Mask;
	/**
	 * Delete the song array upon completion?
	 */
	bool m_DeleteSongs;
	/**
	 * Call m_Functor->OnThreadEnd() if process is aborted?
	 */
	bool m_CallFunctorOnAbort;
	/**
	 *
	 */
	bool m_SpaceToUndr;
};

///////////////////////////////////////////////////

/**
A task (thread) that batch renames files based on their
filename and a mask.

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT TagToFnTask : public Task
{

public:

	TagToFnTask();
	~TagToFnTask();

	/**
	 * Starts task execution. m_Params must be set prior to calling.
	 */
	void run();

	/**
	 * Paramters for execution. If uninitialized nothing will happen
	 * and the task will simply return.
	 */
	TagToFn* m_Params;

private:

	void SpcToUndr( SongInfo& info );
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
