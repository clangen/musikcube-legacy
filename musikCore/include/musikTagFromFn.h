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

#ifndef C_MUSIK_TAGFROMFN_H
#define C_MUSIK_TAGFROMFN_H

///////////////////////////////////////////////////

#include "musikConfig.h"

#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musikCore {

///////////////////////////////////////////////////

/**
 Parameters for a TagFromFnTask

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT TagFromFn
{
public: 

	TagFromFn();
	TagFromFn( Library* pLibrary, SongArray* songs, Functor* pFunctor, String& mask, bool undr_to_spc );
	~TagFromFn();

	/**
	 * Required. Library to save information back to.
	 */
	Library* m_Library;
	/**
	 * Optional. Functor to call for events
	 */
	Functor* m_Functor;
	/**
	 * Required. Array of songs that the user wishes to rename
	 */	
	SongArray* m_Songs;
	/**
	 * Required. Mask used for renaming
	 */
	String m_Mask;
	/**
	 * Optional. Delete the song array upon completion? Defaults true.
	 */
	bool m_DeleteSongs;
	/**
	 * Optinal. Call m_Functor->OnThreadEnd() if process is aborted? 
	 * defaults false.
	 */
	bool m_CallFunctorOnAbort;
	/**
	 * Convert underscores to spaces.
	 */
	bool m_UndrToSpace;
};

///////////////////////////////////////////////////

/**
A task (thread) that batch parses filenames, and, using a mask,
retrieves song information.

@author Casey Langen
@version alpha 0.7
 */
class MUSIKEXPORT TagFromFnTask : public Task
{

public:

	TagFromFnTask();
	~TagFromFnTask();

	/**
	 * Start execution. m_Params must be initialized prior to calling.
	 */
	void run();

	/**
	 * Required. Paramters used for task execution. Must be initialized 
	 * before  calling run() or the task will simply return.
	 */
	TagFromFn* m_Params;
};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
